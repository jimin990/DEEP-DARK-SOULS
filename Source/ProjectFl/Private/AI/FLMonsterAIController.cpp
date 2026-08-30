// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLMonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AFLMonsterAIController::AFLMonsterAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AFLMonsterAIController::BeginPlay()
{
    Super::BeginPlay();

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }

	/*UBlackboardComponent* BB = GetBlackboardComponent();

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

	BB->SetValueAsObject(TEXT("Target"), PlayerPawn);*/

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
		this,
		&AFLMonsterAIController::OnTargetPerceptionUpdated
	);
}

void AFLMonsterAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Actor != PlayerPawn)
	{
		return;
	}

	UBlackboardComponent* BB = GetBlackboardComponent();

	if (!BB)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Stimulus is Succecssfully!"))
		BB->SetValueAsObject(TEXT("Target"), Actor);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Stimulus is failed!"))
		BB->ClearValue(TEXT("Target"));
	}
}
