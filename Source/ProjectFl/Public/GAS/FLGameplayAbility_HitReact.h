// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FLGameplayAbility_HitReact.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class PROJECTFL_API UFLGameplayAbility_HitReact : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFLGameplayAbility_HitReact();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();
};
