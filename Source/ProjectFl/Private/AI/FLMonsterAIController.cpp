// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLMonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AFLMonsterAIController::BeginPlay()
{
    Super::BeginPlay();

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }

	UBlackboardComponent* BB = GetBlackboardComponent();

	if (!BehaviorTreeAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("BehaviorTreeAsset is null"));
		return;
	}

	if (!BB)
	{
		UE_LOG(LogTemp, Warning, TEXT("BlackboardComponent is null"));
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController exists, but PlayerPawn is null"));
		return;
	}

	BB->SetValueAsObject(TEXT("Target"), PlayerPawn);
}