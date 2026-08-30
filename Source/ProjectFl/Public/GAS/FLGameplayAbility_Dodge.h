// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FLGameplayAbility_Dodge.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFL_API UFLGameplayAbility_Dodge : public UGameplayAbility
{
	GENERATED_BODY()

	UFLGameplayAbility_Dodge();

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
	
};
