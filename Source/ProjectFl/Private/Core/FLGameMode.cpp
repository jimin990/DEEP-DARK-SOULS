// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/FLGameMode.h"
#include "Player/FLPlayerState.h"

AFLGameMode::AFLGameMode()
{
	PlayerStateClass = AFLPlayerState::StaticClass();
}

void AFLGameMode::RespawnPlayer(AController* Controller)
{
	if (!Controller)
	{
		return;
	}

	RestartPlayer(Controller);
}