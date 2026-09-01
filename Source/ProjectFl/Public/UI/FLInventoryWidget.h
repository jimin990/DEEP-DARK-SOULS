// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FLInventoryWidget.generated.h"

class UUniformGridPanel;
class UFLWeaponSlotWidget;
class UFLInventoryComponent;

UCLASS()
class PROJECTFL_API UFLInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitInventory(UFLInventoryComponent* InInventoryComponent);

protected:
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGrid;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFLWeaponSlotWidget> EquippedWeaponSlot;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UFLWeaponSlotWidget> WeaponSlotWidgetClass;

	UPROPERTY()
	TObjectPtr<UFLInventoryComponent> InventoryComponent;

	void RefreshInventory();
};