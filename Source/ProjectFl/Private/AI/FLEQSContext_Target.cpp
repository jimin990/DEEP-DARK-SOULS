// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLEQSContext_Target.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Pawn.h"

void UFLEQSContext_Target::ProvideContext(
    FEnvQueryInstance& QueryInstance,
    FEnvQueryContextData& ContextData
) const
{
    UObject* QueryOwner = QueryInstance.Owner.Get();

    AAIController* AIController = Cast<AAIController>(QueryOwner);

    if (!AIController)
    {
        APawn* Pawn = Cast<APawn>(QueryOwner);

        if (Pawn)
        {
            AIController = Cast<AAIController>(Pawn->GetController());
        }
    }

    if (!AIController)
    {
        return;
    }

    UBlackboardComponent* BB = AIController->GetBlackboardComponent();

    if (!BB)
    {
        return;
    }

    AActor* TargetActor =
        Cast<AActor>(BB->GetValueAsObject(TEXT("Target")));

    if (!TargetActor)
    {
        return;
    }

    UEnvQueryItemType_Actor::SetContextHelper(
        ContextData,
        TargetActor
    );
}