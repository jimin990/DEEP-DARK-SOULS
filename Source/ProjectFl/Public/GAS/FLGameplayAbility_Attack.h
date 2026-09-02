// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FLGameplayAbility_Attack.generated.h"

class AFLCharacterBase;
class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS()
class PROJECTFL_API UFLGameplayAbility_Attack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFLGameplayAbility_Attack();

	virtual void ActivateAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(
		FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	void ApplyDamageEffectToTarget(AActor* TargetActor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	bool bAutoContinueComboForAI = true;

private:
	UPROPERTY()
	TSet<TObjectPtr<AActor>> HitActors;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> TraceEventTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboInputTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboOpenTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboCloseTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	int32 ComboIndex = 0;
	bool bCanReceiveComboInput = false;

private:
	void PlayComboMontage();
	void SetupTraceEventTask(const FGameplayTag& TraceEventTag);
	void UnbindMontageTaskDelegates();
	void RequestNextComboForAI();
	void ApplyAttackDirection(AFLCharacterBase* InCharacter);

	void ReportDamageToPerception(
		AActor* TargetActor,
		float DamageAmount
	);

	UFUNCTION()
	void AttackTrace(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboInput(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowOpen(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowClose(FGameplayEventData Payload);

	UFUNCTION()
	void OnAttackMontageCompleted();

	UFUNCTION()
	void OnAttackMontageInterrupted();

	UFUNCTION()
	void OnAttackMontageCancelled();
};