// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FLGA_BossChargeAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/FLCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

UFLGA_BossChargeAttack::UFLGA_BossChargeAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGA_BossChargeAttack::ActivateAbility(
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

	if (!Character || !ChargeMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	HitActors.Reset();

	ChargeDirection = Character->GetActorForwardVector();
	ChargeDirection.Z = 0.f;
	ChargeDirection.Normalize();

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		ChargeMontage,
		1.f
	);

	MontageTask->OnCompleted.AddDynamic(this, &UFLGA_BossChargeAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UFLGA_BossChargeAttack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UFLGA_BossChargeAttack::OnMontageCancelled);

	MontageTask->ReadyForActivation();

	StartCharge();
}

void UFLGA_BossChargeAttack::StartCharge()
{
	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		return;
	}

	const float TickInterval = 0.02f;

	Character->GetWorldTimerManager().SetTimer(
		ChargeTimerHandle,
		this,
		&UFLGA_BossChargeAttack::TickCharge,
		TickInterval,
		true
	);

	Character->GetWorldTimerManager().SetTimer(
		ChargeEndTimerHandle,
		this,
		&UFLGA_BossChargeAttack::EndCharge,
		ChargeDuration,
		false
	);
}

void UFLGA_BossChargeAttack::TickCharge()
{
	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		return;
	}

	const float DeltaTime = 0.02f;
	const float Speed = ChargeDistance / ChargeDuration;

	const FVector MoveDelta = ChargeDirection * Speed * DeltaTime;

	Character->AddActorWorldOffset(
		MoveDelta,
		true
	);

	const FVector Start = Character->GetActorLocation();
	const FVector End = Start + ChargeDirection * 120.f;

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	const bool bHit = Character->GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(TraceRadius),
		Params
	);

	/*DrawDebugSphere(Character->GetWorld(), Start, TraceRadius, 12, FColor::Green, false, 0.1f);
	DrawDebugSphere(Character->GetWorld(), End, TraceRadius, 12, FColor::Red, false, 0.1f);
	DrawDebugLine(Character->GetWorld(), Start, End, FColor::Yellow, false, 0.1f, 0, 2.f);*/

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* TargetActor = Hit.GetActor();

		if (!TargetActor || HitActors.Contains(TargetActor))
		{
			continue;
		}

		HitActors.Add(TargetActor);
		ApplyEffectsToTarget(TargetActor);
	}
}

void UFLGA_BossChargeAttack::ApplyEffectsToTarget(AActor* TargetActor)
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

void UFLGA_BossChargeAttack::EndCharge()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(ChargeTimerHandle);
	World->GetTimerManager().ClearTimer(ChargeEndTimerHandle);
}

void UFLGA_BossChargeAttack::OnMontageCompleted()
{
	EndCharge();

	HitActors.Reset();

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void UFLGA_BossChargeAttack::OnMontageInterrupted()
{
	EndCharge();

	HitActors.Reset();

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		true,
		true
	);
}

void UFLGA_BossChargeAttack::OnMontageCancelled()
{
	EndCharge();

	HitActors.Reset();

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		true,
		true
	);
}

void UFLGA_BossChargeAttack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	EndCharge();

	HitActors.Reset();

	Super::EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		bReplicateEndAbility,
		bWasCancelled
	);
}