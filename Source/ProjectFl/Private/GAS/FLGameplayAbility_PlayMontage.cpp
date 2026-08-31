// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FLGameplayAbility_PlayMontage.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/FLAttributeSet.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/FLCharacterBase.h"
#include "GameplayTagContainer.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"

UFLGameplayAbility_PlayMontage::UFLGameplayAbility_PlayMontage()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGameplayAbility_PlayMontage::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Montage is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ApplySelfEffects();

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		Montage,
		PlayRate
	);

	// 몽타주가 끝나면 동작하는 함수 바인딩
	MontageTask->OnCompleted.AddDynamic(
		this,
		&UFLGameplayAbility_PlayMontage::OnAttackMontageCompleted
	);

	MontageTask->OnInterrupted.AddDynamic(
		this,
		&UFLGameplayAbility_PlayMontage::OnAttackMontageInterrupted
	);

	MontageTask->OnCancelled.AddDynamic(
		this,
		&UFLGameplayAbility_PlayMontage::OnAttackMontageCancelled
	);

	MontageTask->ReadyForActivation();
}

void UFLGameplayAbility_PlayMontage::ApplySelfEffects()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	if (!ASC)
	{
		return;
	}

	FGameplayEffectContextHandle ContextHandle =
		ASC->MakeEffectContext();

	ContextHandle.AddSourceObject(this);
	ContextHandle.AddInstigator(
		GetAvatarActorFromActorInfo(),
		GetAvatarActorFromActorInfo()
	);

	for (const TSubclassOf<UGameplayEffect>& EffectClass : SelfEffectClasses)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectSpecHandle SpecHandle =
			ASC->MakeOutgoingSpec(
				EffectClass,
				GetAbilityLevel(),
				ContextHandle
			);

		if (!SpecHandle.IsValid())
		{
			continue;
		}

		ASC->ApplyGameplayEffectSpecToSelf(
			*SpecHandle.Data.Get()
		);
	}
}

void UFLGameplayAbility_PlayMontage::OnAttackMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Montage Completed!!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UFLGameplayAbility_PlayMontage::OnAttackMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("Montage Interrupted!!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFLGameplayAbility_PlayMontage::OnAttackMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("Montage Cancelled!!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}