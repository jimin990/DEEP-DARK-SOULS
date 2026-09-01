// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLBTTask_CombatStrafe.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UFLBTTask_CombatStrafe::UFLBTTask_CombatStrafe()
{
    NodeName = TEXT("Focus Target");

    TargetKey.AddObjectFilter(
        this,
        GET_MEMBER_NAME_CHECKED(UFLBTTask_CombatStrafe, TargetKey),
        AActor::StaticClass()
    );
}

EBTNodeResult::Type UFLBTTask_CombatStrafe::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory
)
{
    AAIController* AIController = OwnerComp.GetAIOwner();

    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    if (!BB)
    {
        return EBTNodeResult::Failed;
    }

    AActor* TargetActor =
        Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));

    if (!TargetActor)
    {
        return EBTNodeResult::Failed;
    }

    AIController->SetFocus(
        TargetActor,
        EAIFocusPriority::Gameplay
    );

    return EBTNodeResult::Succeeded;
}