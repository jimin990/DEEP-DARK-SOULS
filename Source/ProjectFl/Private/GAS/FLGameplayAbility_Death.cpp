// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FLGameplayAbility_Death.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "Characters/FLCharacterBase.h"

#include "AIController.h"

UFLGameplayAbility_Death::UFLGameplayAbility_Death()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    AFLCharacterBase* Character =
        Cast<AFLCharacterBase>(ActorInfo->AvatarActor.Get());

    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ApplyDeadEffect();

    Character->Die();

    if (!DeathMontage)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
        return;
    }

    MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
        this,
        NAME_None,
        DeathMontage,
        PlayRate
    );

    MontageTask->OnCompleted.AddDynamic(
        this,
        &UFLGameplayAbility_Death::OnDeathMontageCompleted
    );

    MontageTask->OnInterrupted.AddDynamic(
        this,
        &UFLGameplayAbility_Death::OnDeathMontageInterrupted
    );

    MontageTask->OnCancelled.AddDynamic(
        this,
        &UFLGameplayAbility_Death::OnDeathMontageCancelled
    );

    MontageTask->ReadyForActivation();
}

void UFLGameplayAbility_Death::ApplyDeadEffect()
{
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC || !DeadEffectClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("ASC, DeadEffectClass is null"));
        return;
    }

    FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
    ContextHandle.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle =
        ASC->MakeOutgoingSpec(
            DeadEffectClass,
            GetAbilityLevel(),
            ContextHandle
        );

    if (!SpecHandle.IsValid())
    {
        return;
    }

    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UFLGameplayAbility_Death::OnDeathMontageCompleted()
{
    UE_LOG(LogTemp, Warning, TEXT("OnDeathMontageCompleted"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UFLGameplayAbility_Death::OnDeathMontageInterrupted()
{
    UE_LOG(LogTemp, Warning, TEXT("OnDeathMontageInterrupted"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFLGameplayAbility_Death::OnDeathMontageCancelled()
{
    UE_LOG(LogTemp, Warning, TEXT("OnDeathMontageCancelled"));
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
