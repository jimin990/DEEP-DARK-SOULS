// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FLInventoryComponent.generated.h"

class UFLWeaponDataAsset;
class AFLCharacterPlayer;

DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTFL_API UFLInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFLInventoryComponent();

	bool AddWeapon(UFLWeaponDataAsset* NewWeaponData);
	bool EquipWeaponByIndex(int32 SlotIndex);
	bool UnequipWeapon();

	void ApplyEquippedWeaponToCharacter(AFLCharacterPlayer* Character);

	UFLWeaponDataAsset* GetEquippedWeapon() const;
	UFLWeaponDataAsset* GetWeaponAt(int32 SlotIndex) const;
	int32 GetWeaponCount() const;

	FOnInventoryChanged OnInventoryChanged;

private:
	static constexpr int32 MaxWeaponSlotCount = 40;

	UPROPERTY()
	TArray<TObjectPtr<UFLWeaponDataAsset>> InventoryWeapons;

	UPROPERTY()
	TObjectPtr<UFLWeaponDataAsset> EquippedWeapon;

	AFLCharacterPlayer* GetCurrentPlayerCharacter() const;
	int32 FindEmptySlot() const;
};