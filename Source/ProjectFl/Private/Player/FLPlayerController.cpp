// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FLPlayerController.h"
#include "UI/FLPlayerHUDWidget.h"
#include "Characters/FLCharacterBase.h"

void AFLPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UFLPlayerHUDWidget>(this, HUDWidgetClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();

			AFLCharacterBase* FLCharacter = Cast<AFLCharacterBase>(GetPawn());
			if (FLCharacter)
			{
				HUDWidget->InitAbilitySystem(FLCharacter->GetAbilitySystemComponent());
			}
		}
	}
}

void AFLPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AFLCharacterBase* FLCharacter = Cast<AFLCharacterBase>(InPawn);

	if (HUDWidget && FLCharacter)
	{
		HUDWidget->InitAbilitySystem(FLCharacter->GetAbilitySystemComponent());
	}
}

