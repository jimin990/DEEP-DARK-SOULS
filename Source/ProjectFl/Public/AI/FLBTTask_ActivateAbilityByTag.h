// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbility.h"
#include "FLBTTask_ActivateAbilityByTag.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFL_API UFLBTTask_ActivateAbilityByTag : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UFLBTTask_ActivateAbilityByTag();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	virtual EBTNodeResult::Type AbortTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	virtual void OnTaskFinished(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		EBTNodeResult::Type TaskResult
	) override;

private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);

protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag AbilityTag;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedASC;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	
private:
	FGameplayAbilitySpecHandle ActivatedAbilityHandle;
};
