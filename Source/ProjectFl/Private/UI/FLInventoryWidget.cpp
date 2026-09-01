// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FLInventoryWidget.h"
#include "UI/FLWeaponSlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Weapons/FLInventoryComponent.h"
#include "Weapons/FLWeaponDataAsset.h"

void UFLInventoryWidget::InitInventory(UFLInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddUObject(
			this,
			&UFLInventoryWidget::RefreshInventory
		);
	}

	RefreshInventory();
}

void UFLInventoryWidget::NativeDestruct()
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UFLInventoryWidget::RefreshInventory()
{
	if (!InventoryComponent || !InventoryGrid || !WeaponSlotWidgetClass)
	{
		return;
	}

	InventoryGrid->ClearChildren();

	if (EquippedWeaponSlot)
	{
		EquippedWeaponSlot->InitSlot(
			InventoryComponent,
			InventoryComponent->GetEquippedWeapon(),
			INDEX_NONE,
			true
		);
	}

	constexpr int32 ColumnCount = 4;
	const int32 SlotCount = InventoryComponent->GetWeaponCount();

	for (int32 Index = 0; Index < SlotCount; ++Index)
	{
		UFLWeaponSlotWidget* SlotWidget =
			CreateWidget<UFLWeaponSlotWidget>(this, WeaponSlotWidgetClass);

		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->InitSlot(
			InventoryComponent,
			InventoryComponent->GetWeaponAt(Index),
			Index,
			false
		);

		const int32 Row = Index / ColumnCount;
		const int32 Column = Index % ColumnCount;

		InventoryGrid->AddChildToUniformGrid(SlotWidget, Row, Column);
	}
}