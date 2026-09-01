// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLMonsterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Characters/FLCharacterMonster.h"

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

void AFLMonsterAIController::OnTargetPerceptionUpdated(
    AActor* Actor,
    FAIStimulus Stimulus
)
{
    if (!IsValid(Actor))
    {
        return;
    }

    APawn* PlayerPawn =
        UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (Actor != PlayerPawn)
    {
        return;
    }

    UBlackboardComponent* BB = GetBlackboardComponent();

    if (!BB)
    {
        return;
    }

    const bool bIsSightStimulus =
        Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>();

    const bool bIsDamageStimulus =
        Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>();

    if (!bIsSightStimulus && !bIsDamageStimulus)
    {
        return;
    }

    // 시야 또는 데미지로 플레이어를 감지한 경우
    if (Stimulus.WasSuccessfullySensed())
    {
        BB->SetValueAsObject(TEXT("Target"), Actor);

        if (AFLCharacterMonster* Monster =
            Cast<AFLCharacterMonster>(GetPawn()))
        {
            Monster->ShowHealthBar();
        }

        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Target acquired: %s / Sense: %s"),
            *Actor->GetName(),
            bIsSightStimulus ? TEXT("Sight") : TEXT("Damage")
        );

        return;
    }

    // 시야에서 사라져도 Target을 즉시 제거하지 않는다.
    if (bIsSightStimulus)
    {
        UE_LOG(
            LogTemp,
            Log,
            TEXT("Target left sight, but target is retained: %s"),
            *Actor->GetName()
        );
    }
}
