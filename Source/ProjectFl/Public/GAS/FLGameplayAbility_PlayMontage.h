// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FLGameplayAbility_PlayMontage.generated.h"

class UGameplayEffect;
class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class PROJECTFL_API UFLGameplayAbility_PlayMontage : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFLGameplayAbility_PlayMontage();

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> SelfEffectClass;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PlayRate = 1.f;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UFUNCTION()
	void OnAttackMontageCompleted();

	UFUNCTION()
	void OnAttackMontageInterrupted();

	UFUNCTION()
	void OnAttackMontageCancelled();
};
