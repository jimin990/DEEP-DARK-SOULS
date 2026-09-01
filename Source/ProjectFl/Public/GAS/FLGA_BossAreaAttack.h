// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "FLGA_BossAreaAttack.generated.h"

class UAnimMontage;
class UGameplayEffect;
class UNiagaraSystem;
class UAbilityTask_PlayMontageAndWait;

UCLASS()
class PROJECTFL_API UFLGA_BossAreaAttack : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UFLGA_BossAreaAttack();

protected:
    virtual void ActivateAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        const FGameplayEventData* TriggerEventData
    ) override;

    virtual void EndAbility(
        const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo,
        bool bReplicateEndAbility,
        bool bWasCancelled
    ) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Area Attack")
    TObjectPtr<UAnimMontage> AreaAttackMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Area Attack")
    float AttackRadius = 400.f;

    UPROPERTY(EditDefaultsOnly, Category = "Area Attack")
    float WarningDelay = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Area Attack")
    FVector WarningEffectSize = FVector(1, 1, 1);

    UPROPERTY(EditDefaultsOnly, Category = "Area Attack")
    FVector EffectSize = FVector(1, 1, 1);

    UPROPERTY(EditDefaultsOnly, Category = "Area Attack")
    TArray<TSubclassOf<UGameplayEffect>> TargetEffects;

    UPROPERTY(EditDefaultsOnly, Category = "Area Attack")
    FGameplayTag HitReactEventTag;

    UPROPERTY(EditDefaultsOnly, Category = "Area Attack|FX")
    TObjectPtr<UNiagaraSystem> WarningEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Area Attack|FX")
    TObjectPtr<UNiagaraSystem> ImpactEffect;

private:
    UPROPERTY()
    TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

    FTimerHandle AreaAttackTimerHandle;

    FVector AttackLocation;

private:
    void SpawnWarningEffect();
    void ExecuteAreaAttack();
    void ApplyEffectsToTarget(AActor* TargetActor);

    UFUNCTION()
    void OnMontageCompleted();

    UFUNCTION()
    void OnMontageInterrupted();

    UFUNCTION()
    void OnMontageCancelled();
};