// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FLWeaponSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Weapons/FLInventoryComponent.h"
#include "Weapons/FLWeaponDataAsset.h"

void UFLWeaponSlotWidget::InitSlot(
	UFLInventoryComponent* InInventoryComponent,
	UFLWeaponDataAsset* InWeaponData,
	int32 InSlotIndex,
	bool bInEquippedSlot
)
{
	InventoryComponent = InInventoryComponent;
	WeaponData = InWeaponData;
	SlotIndex = InSlotIndex;
	bEquippedSlot = bInEquippedSlot;

	RefreshSlot();
}

FReply UFLWeaponSlotWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		EquipFromSlot();
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UFLWeaponSlotWidget::RefreshSlot()
{
	if (!WeaponData)
	{
		if (WeaponIcon)
		{
			WeaponIcon->SetBrushFromTexture(nullptr);
		}

		if (WeaponNameText)
		{
			WeaponNameText->SetText(FText::GetEmpty());
		}

		return;
	}

	if (WeaponIcon)
	{
		WeaponIcon->SetBrushFromTexture(WeaponData->WeaponIcon);
	}

	if (WeaponNameText)
	{
		WeaponNameText->SetText(WeaponData->WeaponName);
	}
}

void UFLWeaponSlotWidget::EquipFromSlot()
{
	if (!InventoryComponent || !WeaponData)
	{
		return;
	}

	if (bEquippedSlot)
	{
		return;
	}

	InventoryComponent->EquipWeaponByIndex(SlotIndex);
}