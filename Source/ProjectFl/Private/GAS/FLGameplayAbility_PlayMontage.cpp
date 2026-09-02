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

void UFLGameplayAbility_PlayMontage::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		TriggerEventData
	);

	if (!Montage)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("%s: Montage is null"),
			*GetNameSafe(GetClass())
		);

		EndAbility(
			Handle,
			ActorInfo,
			ActivationInfo,
			true,
			true
		);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("%s: CommitAbility failed"),
			*GetNameSafe(GetClass())
		);

		EndAbility(
			Handle,
			ActorInfo,
			ActivationInfo,
			true,
			true
		);
		return;
	}

	MontageTask =
		UAbilityTask_PlayMontageAndWait::
		CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			Montage,
			PlayRate
		);

	// 생성한 다음에 검사해야 한다.
	if (!MontageTask)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("%s: MontageTask creation failed"),
			*GetNameSafe(GetClass())
		);

		EndAbility(
			Handle,
			ActorInfo,
			ActivationInfo,
			true,
			true
		);
		return;
	}

	// Task 생성 성공 후 자신에게 이펙트 적용
	ApplySelfEffects();

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

	MontageTask->OnBlendOut.AddDynamic(
		this,
		&UFLGameplayAbility_PlayMontage::OnAttackMontageBlendOut
	);

	MontageTask->ReadyForActivation();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s: Montage started"),
		*GetNameSafe(GetClass())
	);
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
	if (!IsActive())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Montage Completed!!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UFLGameplayAbility_PlayMontage::OnAttackMontageInterrupted()
{
	if (!IsActive())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Montage Interrupted!!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFLGameplayAbility_PlayMontage::OnAttackMontageCancelled()
{
	if (!IsActive())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Montage Cancelled!!"));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFLGameplayAbility_PlayMontage::OnAttackMontageBlendOut()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("PlayMontage Ability: OnBlendOut")
	);

	if (!IsActive())
	{
		return;
	}

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void UFLGameplayAbility_PlayMontage::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	UAbilitySystemComponent* ASC =
		GetAbilitySystemComponentFromActorInfo();

	const FGameplayTag HitReactTag =
		FGameplayTag::RequestGameplayTag(
			TEXT("State.HitReact"),
			false
		);

	const int32 BeforeCount =
		ASC && HitReactTag.IsValid()
		? ASC->GetGameplayTagCount(HitReactTag)
		: -1;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("PlayMontage EndAbility / Tag count before: %d"),
		BeforeCount
	);

	Super::EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		bReplicateEndAbility,
		bWasCancelled
	);

	const int32 AfterCount =
		ASC && HitReactTag.IsValid()
		? ASC->GetGameplayTagCount(HitReactTag)
		: -1;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("PlayMontage EndAbility / Tag count after: %d"),
		AfterCount
	);

	MontageTask = nullptr;
}