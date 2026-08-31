// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FLBTTask_StopAI.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFL_API UFLBTTask_StopAI : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UFLBTTask_StopAI();

protected:
    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory
    ) override;
};
