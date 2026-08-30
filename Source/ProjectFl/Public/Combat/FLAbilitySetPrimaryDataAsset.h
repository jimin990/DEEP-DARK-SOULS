// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "FLAbilitySetPrimaryDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FFLAbilitySetItem
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    TSubclassOf<UGameplayAbility> AbilityClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    int32 AbilityLevel = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tag")
    FGameplayTag ActivationTag;
};

UCLASS()
class PROJECTFL_API UFLAbilitySetPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TArray<FFLAbilitySetItem> Abilities;
};
