// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FLCharacterBoss.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FLPlayerController.h"

AFLCharacterBoss::AFLCharacterBoss()
{
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void AFLCharacterBoss::BeginPlay()
{
	Super::BeginPlay();
}

void AFLCharacterBoss::Die()
{
	Super::Die();

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* BrainComponent = AIController->GetBrainComponent())
		{
			BrainComponent->StopLogic(TEXT("Boss Dead"));
		}

		AIController->StopMovement();
	}

	SetLifeSpan(DestroyDelayAfterDeath);
	
	AFLPlayerController* PC =
		Cast<AFLPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (PC)
	{
		PC->ShowGameEndWidget();
	}
}