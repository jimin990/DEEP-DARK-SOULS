// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLBTService_UpdateCombatRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UFLBTService_UpdateCombatRange::UFLBTService_UpdateCombatRange()
{
	NodeName = TEXT("Update Combat Range");
	Interval = 0.2f;
	RandomDeviation = 0.05f;
}

void UFLBTService_UpdateCombatRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* AIPawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!AIPawn || !BB)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!TargetActor)
	{
		BB->SetValueAsBool(IsInCombatRangeKey.SelectedKeyName, false);
		BB->SetValueAsFloat(DistanceKey.SelectedKeyName, 0.f);
		return;
	}

	const float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());

	BB->SetValueAsFloat(DistanceKey.SelectedKeyName, Distance);

	const int32 AIState = Distance <= CombatRange ? 2 : 1;

	BB->SetValueAsInt(IsInCombatRangeKey.SelectedKeyName, AIState);

	//BB->SetValueAsInt(IsInCombatRangeKey.SelectedKeyName, Distance <= CombatRange ? 2 : 1);

	/*UE_LOG(LogTemp, Warning, TEXT("Distance: %f / CombatRange: %f / AIState: %d"),
		Distance,
		CombatRange,
		AIState);*/
}
