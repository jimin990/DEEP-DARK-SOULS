// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FLGameplayAbility_Death.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAnimMontage;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PROJECTFL_API UFLGameplayAbility_Death : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
    UFLGameplayAbility_Death();

    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
    TObjectPtr<UAnimMontage> DeathMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
    TSubclassOf<UGameplayEffect> DeadEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
    float PlayRate = 1.f;

private:
    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

    void ApplyDeadEffect();

    UFUNCTION()
    void OnDeathMontageCompleted();

    UFUNCTION()
    void OnDeathMontageInterrupted();

    UFUNCTION()
    void OnDeathMontageCancelled();
};
