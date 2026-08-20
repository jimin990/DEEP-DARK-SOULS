// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FLGameplayAbility_Attack.generated.h"

class UGameplayEffect;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class PROJECTFL_API UFLGameplayAbility_Attack : public UGameplayAbility
{
	GENERATED_BODY()
	
	UFLGameplayAbility_Attack();

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

private:
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> TraceEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UFUNCTION()
	void AttackTrace(FGameplayEventData Payload);

	UFUNCTION()
	void OnAttackMontageCompleted();

	UFUNCTION()
	void OnAttackMontageInterrupted();

	UFUNCTION()
	void OnAttackMontageCancelled();

public:
	void ApplyDamageEffectToTarget(AActor* TargetActor);
};
