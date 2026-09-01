// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/FLGA_BossAreaAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/FLCharacterBase.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/EngineTypes.h"
#include "CollisionShape.h"
#include "Engine/OverlapResult.h"

UFLGA_BossAreaAttack::UFLGA_BossAreaAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGA_BossAreaAttack::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    const FGameplayEventData* TriggerEventData
)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AFLCharacterBase* Character =
        Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AttackLocation = Character->GetActorLocation();

    SpawnWarningEffect();

    UWorld* World = Character->GetWorld();

    if (!World)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    World->GetTimerManager().SetTimer(
        AreaAttackTimerHandle,
        this,
        &UFLGA_BossAreaAttack::ExecuteAreaAttack,
        WarningDelay,
        false
    );

    if (!AreaAttackMontage)
    {
        return;
    }

    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,
        AreaAttackMontage,
        1.f
    );

    MontageTask->OnCompleted.AddDynamic(
        this,
        &UFLGA_BossAreaAttack::OnMontageCompleted
    );

    MontageTask->OnInterrupted.AddDynamic(
        this,
        &UFLGA_BossAreaAttack::OnMontageInterrupted
    );

    MontageTask->OnCancelled.AddDynamic(
        this,
        &UFLGA_BossAreaAttack::OnMontageCancelled
    );

    MontageTask->ReadyForActivation();
}

void UFLGA_BossAreaAttack::SpawnWarningEffect()
{
    UWorld* World = GetWorld();

    if (!World || !WarningEffect)
    {
        return;
    }

    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        World,
        WarningEffect,
        AttackLocation,
        FRotator::ZeroRotator,
        WarningEffectSize
    );

    /*DrawDebugSphere(
        World,
        AttackLocation,
        AttackRadius,
        32,
        FColor::Orange,
        false,
        WarningDelay,
        0,
        3.f
    );*/
}

void UFLGA_BossAreaAttack::ExecuteAreaAttack()
{
    AFLCharacterBase* Character =
        Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

    if (!Character || Character->IsActorBeingDestroyed())
    {
        EndAbility(
            CurrentSpecHandle,
            CurrentActorInfo,
            CurrentActivationInfo,
            true,
            true
        );
        return;
    }

    UWorld* World = Character->GetWorld();

    if (!World || World->bIsTearingDown)
    {
        return;
    }

    if (ImpactEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World,
            ImpactEffect,
            AttackLocation,
            FRotator::ZeroRotator,
            EffectSize
        );
    }

    TArray<FOverlapResult> OverlapResults;

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(Character);

    const bool bHit = World->OverlapMultiByChannel(
        OverlapResults,
        AttackLocation,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(AttackRadius),
        Params
    );

    /*DrawDebugSphere(
        World,
        AttackLocation,
        AttackRadius,
        32,
        FColor::Red,
        false,
        1.0f,
        0,
        4.f
    );*/

    if (!bHit)
    {
        return;
    }

    TSet<AActor*> HitActors;

    for (const FOverlapResult& Overlap : OverlapResults)
    {
        AActor* TargetActor = Overlap.GetActor();

        if (!TargetActor || HitActors.Contains(TargetActor))
        {
            continue;
        }

        HitActors.Add(TargetActor);
        ApplyEffectsToTarget(TargetActor);
    }
}

void UFLGA_BossAreaAttack::ApplyEffectsToTarget(AActor* TargetActor)
{
    if (!TargetActor)
    {
        return;
    }

    UAbilitySystemComponent* SourceASC =
        GetAbilitySystemComponentFromActorInfo();

    UAbilitySystemComponent* TargetASC =
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

    if (!SourceASC || !TargetASC)
    {
        return;
    }

    const FGameplayTag InvincibleTag =
        FGameplayTag::RequestGameplayTag(TEXT("State.Invincible"), false);

    if (InvincibleTag.IsValid() && TargetASC->HasMatchingGameplayTag(InvincibleTag))
    {
        return;
    }

    FGameplayEffectContextHandle ContextHandle =
        SourceASC->MakeEffectContext();

    ContextHandle.AddSourceObject(this);

    for (TSubclassOf<UGameplayEffect> EffectClass : TargetEffects)
    {
        if (!EffectClass)
        {
            continue;
        }

        FGameplayEffectSpecHandle SpecHandle =
            SourceASC->MakeOutgoingSpec(
                EffectClass,
                GetAbilityLevel(),
                ContextHandle
            );

        if (SpecHandle.IsValid())
        {
            SourceASC->ApplyGameplayEffectSpecToTarget(
                *SpecHandle.Data.Get(),
                TargetASC
            );
        }
    }

    if (HitReactEventTag.IsValid())
    {
        FGameplayEventData EventData;
        EventData.EventTag = HitReactEventTag;
        EventData.Instigator = GetAvatarActorFromActorInfo();
        EventData.Target = TargetActor;

        UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
            TargetActor,
            HitReactEventTag,
            EventData
        );
    }
}

void UFLGA_BossAreaAttack::OnMontageCompleted()
{
    EndAbility(
        CurrentSpecHandle,
        CurrentActorInfo,
        CurrentActivationInfo,
        false,
        false
    );
}

void UFLGA_BossAreaAttack::OnMontageInterrupted()
{
    EndAbility(
        CurrentSpecHandle,
        CurrentActorInfo,
        CurrentActivationInfo,
        true,
        true
    );
}

void UFLGA_BossAreaAttack::OnMontageCancelled()
{
    EndAbility(
        CurrentSpecHandle,
        CurrentActorInfo,
        CurrentActivationInfo,
        true,
        true
    );
}

void UFLGA_BossAreaAttack::EndAbility(
    const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility,
    bool bWasCancelled
)
{
    if (UWorld* World = GetWorld())
    {
        if (!World->bIsTearingDown)
        {
            World->GetTimerManager().ClearTimer(AreaAttackTimerHandle);
        }
    }

    Super::EndAbility(
        Handle,
        ActorInfo,
        ActivationInfo,
        bReplicateEndAbility,
        bWasCancelled
    );
}