// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AIController.h"
#include "FLBTTask_CombatStrafe.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class PROJECTFL_API UFLBTTask_CombatStrafe : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UFLBTTask_CombatStrafe();

protected:
    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory
    ) override;

protected:
    UPROPERTY(EditAnywhere, Category = "AI")
    FBlackboardKeySelector TargetKey;
};