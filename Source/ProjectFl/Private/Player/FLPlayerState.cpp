// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FLPlayerState.h"
#include "Weapons/FLInventoryComponent.h"

AFLPlayerState::AFLPlayerState()
{
	InventoryComponent = CreateDefaultSubobject<UFLInventoryComponent>(TEXT("InventoryComponent"));
}

UFLInventoryComponent* AFLPlayerState::GetInventoryComponent() const
{
	return InventoryComponent;
}