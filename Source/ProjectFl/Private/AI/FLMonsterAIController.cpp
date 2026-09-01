// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLMonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"

AFLMonsterAIController::AFLMonsterAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));

	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	SightConfig->PeripheralVisionAngleDegrees = 100.f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Sight Perception
	AIPerceptionComponent->ConfigureSense(*SightConfig);

	// Damage Perception
	AIPerceptionComponent->ConfigureSense(*DamageConfig);

	AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AFLMonsterAIController::BeginPlay()
{
    Super::BeginPlay();

    if (BehaviorTreeAsset)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }

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
	if (!BB || !Actor)
	{
		return;
	}

	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		BB->SetValueAsObject(TEXT("Target"), Actor);

		UE_LOG(LogTemp, Warning, TEXT("Damage Sense Target: %s"), *Actor->GetName());
		return;
	}

	// 기존 Sight 처리
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("Stimulus is Succecssfully!"))
			BB->SetValueAsObject(TEXT("Target"), Actor);
	}
	else
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Stimulus is failed!"))
			BB->ClearValue(TEXT("Target"));*/
	}
}
