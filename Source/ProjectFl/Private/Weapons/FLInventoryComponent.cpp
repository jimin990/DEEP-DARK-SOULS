// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FLInventoryComponent.h"
#include "Weapons/FLWeaponDataAsset.h"
#include "Characters/FLCharacterPlayer.h"
#include "Combat/FLCombatComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"

UFLInventoryComponent::UFLInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	InventoryWeapons.SetNum(MaxWeaponSlotCount);
}

bool UFLInventoryComponent::AddWeapon(UFLWeaponDataAsset* NewWeaponData)
{
	if (!NewWeaponData)
	{
		return false;
	}

	if (!EquippedWeapon)
	{
		EquippedWeapon = NewWeaponData;

		if (AFLCharacterPlayer* Character = GetCurrentPlayerCharacter())
		{
			ApplyEquippedWeaponToCharacter(Character);
		}

		OnInventoryChanged.Broadcast();
		return true;
	}

	const int32 EmptySlotIndex = FindEmptySlot();

	if (EmptySlotIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full"));
		return false;
	}

	InventoryWeapons[EmptySlotIndex] = NewWeaponData;

	OnInventoryChanged.Broadcast();
	return true;
}

bool UFLInventoryComponent::EquipWeaponByIndex(int32 SlotIndex)
{
	if (!InventoryWeapons.IsValidIndex(SlotIndex))
	{
		return false;
	}

	UFLWeaponDataAsset* NewWeapon = InventoryWeapons[SlotIndex];

	if (!NewWeapon)
	{
		return false;
	}

	InventoryWeapons[SlotIndex] = EquippedWeapon;
	EquippedWeapon = NewWeapon;

	if (AFLCharacterPlayer* Character = GetCurrentPlayerCharacter())
	{
		ApplyEquippedWeaponToCharacter(Character);
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UFLInventoryComponent::UnequipWeapon()
{
	if (!EquippedWeapon)
	{
		return false;
	}

	const int32 EmptySlotIndex = FindEmptySlot();

	if (EmptySlotIndex == INDEX_NONE)
	{
		return false;
	}

	InventoryWeapons[EmptySlotIndex] = EquippedWeapon;
	EquippedWeapon = nullptr;

	if (AFLCharacterPlayer* Character = GetCurrentPlayerCharacter())
	{
		if (UFLCombatComponent* CombatComponent = Character->GetCombatComponent())
		{
			CombatComponent->UnequipWeapon();
		}
	}

	OnInventoryChanged.Broadcast();
	return true;
}

void UFLInventoryComponent::ApplyEquippedWeaponToCharacter(AFLCharacterPlayer* Character)
{
	if (!Character)
	{
		return;
	}

	UFLCombatComponent* CombatComponent = Character->GetCombatComponent();

	if (!CombatComponent)
	{
		return;
	}

	CombatComponent->EquipWeapon(EquippedWeapon);
}

UFLWeaponDataAsset* UFLInventoryComponent::GetEquippedWeapon() const
{
	return EquippedWeapon;
}

UFLWeaponDataAsset* UFLInventoryComponent::GetWeaponAt(int32 SlotIndex) const
{
	if (!InventoryWeapons.IsValidIndex(SlotIndex))
	{
		return nullptr;
	}

	return InventoryWeapons[SlotIndex];
}

int32 UFLInventoryComponent::GetWeaponCount() const
{
	return InventoryWeapons.Num();
}

AFLCharacterPlayer* UFLInventoryComponent::GetCurrentPlayerCharacter() const
{
	const APlayerState* OwnerPlayerState = Cast<APlayerState>(GetOwner());

	if (!OwnerPlayerState)
	{
		return nullptr;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPlayerState->GetOwner());

	if (!PC)
	{
		return nullptr;
	}

	return Cast<AFLCharacterPlayer>(PC->GetPawn());
}

int32 UFLInventoryComponent::FindEmptySlot() const
{
	for (int32 Index = 0; Index < InventoryWeapons.Num(); ++Index)
	{
		if (!InventoryWeapons[Index])
		{
			return Index;
		}
	}

	return INDEX_NONE;
}