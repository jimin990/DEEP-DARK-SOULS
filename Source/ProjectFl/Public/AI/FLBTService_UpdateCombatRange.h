// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "FLBTService_UpdateCombatRange.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFL_API UFLBTService_UpdateCombatRange : public UBTService
{
	GENERATED_BODY()
	
public:
	UFLBTService_UpdateCombatRange();

protected:
	virtual void TickNode(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds
	) override;

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector DistanceKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector IsInCombatRangeKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float CombatRange = 300.f;
};
