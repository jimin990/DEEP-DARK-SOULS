// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FLGameplayAbility_HitReact.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Characters/FLCharacterBase.h"

UFLGameplayAbility_HitReact::UFLGameplayAbility_HitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGameplayAbility_HitReact::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 이렇게 몽타주가 있는 부분은 결합을 낮출수 있는 방법 있을까 고민해보기
	AFLCharacterBase* FLPlayer = Cast<AFLCharacterBase>(ActorInfo->AvatarActor.Get());

	UE_LOG(LogTemp, Warning, TEXT("Hit!!"));

	if (!FLPlayer->HitReactMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitReactMontage is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		FLPlayer->HitReactMontage,
		1.f
	);

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask->OnCompleted.AddDynamic(
		this,
		&UFLGameplayAbility_HitReact::OnMontageCompleted
	);

	MontageTask->OnInterrupted.AddDynamic(
		this,
		&UFLGameplayAbility_HitReact::OnMontageInterrupted
	);

	MontageTask->OnCancelled.AddDynamic(
		this,
		&UFLGameplayAbility_HitReact::OnMontageCancelled
	);

	MontageTask->ReadyForActivation();
}

void UFLGameplayAbility_HitReact::OnMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("HitReact Completed!!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UFLGameplayAbility_HitReact::OnMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("HitReact Interrupted!!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFLGameplayAbility_HitReact::OnMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("HitReact Cancelled!!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}