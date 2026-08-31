// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLBTTask_StopAI.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UFLBTTask_StopAI::UFLBTTask_StopAI()
{
	NodeName = TEXT("Stop AI");
}

EBTNodeResult::Type UFLBTTask_StopAI::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UE_LOG(LogTemp, Log, TEXT("Stop AI"));

    AAIController* AIController = OwnerComp.GetAIOwner();

    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    AIController->StopMovement();
    AIController->ClearFocus(EAIFocusPriority::Gameplay);

    if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
    {
        BB->ClearValue(TEXT("Target"));
    }

    OwnerComp.StopTree();

    return EBTNodeResult::Succeeded;
}
