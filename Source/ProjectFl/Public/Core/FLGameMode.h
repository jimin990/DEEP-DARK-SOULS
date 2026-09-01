// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FLGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFL_API AFLGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AFLGameMode();

	void RespawnPlayer(AController* Controller);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Respawn")
	float RespawnDelay = 3.f;
};
