// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "FLGA_BossChargeAttack.generated.h"

class UGameplayEffect;
class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class PROJECTFL_API UFLGA_BossChargeAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFLGA_BossChargeAttack();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	TObjectPtr<UAnimMontage> ChargeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	float ChargeDistance = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	float ChargeDuration = 0.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	float TraceRadius = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	TArray<TSubclassOf<UGameplayEffect>> TargetEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Charge")
	FGameplayTag HitReactEventTag;

private:
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	FTimerHandle ChargeTimerHandle;
	FTimerHandle ChargeEndTimerHandle;

	FVector ChargeDirection = FVector::ZeroVector;

	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	void StartCharge();
	void TickCharge();
	void EndCharge();

	void ApplyEffectsToTarget(AActor* TargetActor);

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageCancelled();

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;
};