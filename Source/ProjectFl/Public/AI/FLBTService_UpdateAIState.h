// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AI/FLAIState.h"
#include "FLBTService_UpdateAIState.generated.h"

UCLASS()
class PROJECTFL_API UFLBTService_UpdateAIState : public UBTService
{
	GENERATED_BODY()
	
public:
	UFLBTService_UpdateAIState();

protected:
	virtual void TickNode(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds
	) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistanceKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsDeadKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AIStateKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	float CombatRange = 100.f;
};
