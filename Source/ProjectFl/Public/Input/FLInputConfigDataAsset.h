// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "FLInputConfigDataAsset.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FFLInputActionTag
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

UCLASS()
class PROJECTFL_API UFLInputConfigDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "InputActionTag")
	TArray<FFLInputActionTag> AbilityInputActions;
};
