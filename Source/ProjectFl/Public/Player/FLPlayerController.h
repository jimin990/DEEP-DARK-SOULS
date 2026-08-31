// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FLPlayerController.generated.h"

class UFLPlayerHUDWidget;

UCLASS()
class PROJECTFL_API AFLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UFLPlayerHUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UFLPlayerHUDWidget> HUDWidget;
	
};
