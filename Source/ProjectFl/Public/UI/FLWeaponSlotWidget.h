// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FLWeaponSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UFLInventoryComponent;
class UFLWeaponDataAsset;

UCLASS()
class PROJECTFL_API UFLWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitSlot(
		UFLInventoryComponent* InInventoryComponent,
		UFLWeaponDataAsset* InWeaponData,
		int32 InSlotIndex,
		bool bInEquippedSlot
	);

protected:
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WeaponNameText;

	UPROPERTY()
	TObjectPtr<UFLInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UFLWeaponDataAsset> WeaponData;

	int32 SlotIndex = INDEX_NONE;
	bool bEquippedSlot = false;

	void RefreshSlot();
	void EquipFromSlot();
};