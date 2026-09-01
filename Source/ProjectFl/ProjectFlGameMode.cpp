// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectFlGameMode.h"
#include "Player/FLPlayerState.h"

AProjectFlGameMode::AProjectFlGameMode()
{
	PlayerStateClass = AFLPlayerState::StaticClass();
}
