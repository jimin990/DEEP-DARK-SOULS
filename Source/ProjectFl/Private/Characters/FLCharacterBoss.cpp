// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FLCharacterBoss.h"
#include "GameFramework/CharacterMovementComponent.h"

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
