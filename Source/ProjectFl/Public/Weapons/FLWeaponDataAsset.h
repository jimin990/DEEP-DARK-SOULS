// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "FLWeaponDataAsset.generated.h"

class UStaticMesh;
class UFLAttackDataAsset;

USTRUCT(BlueprintType)
struct FFLWeaponAttackData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimMontage> AttackMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName TraceStartSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName TraceEndSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float TraceRadius = 20.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TSubclassOf<UGameplayEffect>> TargetEffects;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag HitReactEventTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag AttackTraceEventTag;
};

UCLASS()
class PROJECTFL_API UFLWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UStaticMesh> WeaponMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FName AttachSocketName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FTransform AttachOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float WeaponPower = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float WeaponPowerMultiplier = 1.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
    TArray<FFLWeaponAttackData> ComboAttacks;

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    FText WeaponName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UTexture2D> WeaponIcon;
};
