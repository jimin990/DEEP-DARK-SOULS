// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FLGameplayAbility_Attack.generated.h"

class UGameplayEffect;
class UAbilityTask_WaitGameplayEvent;
class UAbilityTask_PlayMontageAndWait;
class AFLCharacterBase;

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

	//ComboSystem
public:
	int32 ComboIndex = 0;

	bool bCanReceiveComboInput = false;
	bool bComboInputBuffered = false;
	bool bChangingComboMontage = false;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboInputTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboOpenTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> ComboCloseTask;

	void PlayComboMontage();

	UFUNCTION()
	void OnComboInput(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowOpen(FGameplayEventData Payload);

	UFUNCTION()
	void OnComboWindowClose(FGameplayEventData Payload);

	// 콤보 공격중 방향 지정
	void ApplyAttackDirection(AFLCharacterBase* InCharacter);

	// Sense에게 데미지 알리기 
	void ReportDamageToPerception(
		AActor* TargetActor,
		float DamageAmount
	);
};
