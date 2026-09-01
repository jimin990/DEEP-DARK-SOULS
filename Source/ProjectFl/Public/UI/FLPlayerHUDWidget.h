// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "FLPlayerHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UAbilitySystemComponent;
class UImage;
class UTextBlock;
class UFLInventoryComponent;

UCLASS()
class PROJECTFL_API UFLPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitAbilitySystem(UAbilitySystemComponent* InASC);

protected:
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HealCooldownBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HealCooldownText;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	float Health = 0.f;
	float MaxHealth = 1.f;
	float Stamina = 0.f;
	float MaxStamina = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	FGameplayTag HealCooldownTag;

	void UpdateHealthBar();
	void UpdateStaminaBar();
	void UpdateHealCooldown();

	bool GetCooldownTimeRemainingAndDuration(
		FGameplayTag CooldownTag,
		float& OutRemaining,
		float& OutDuration
	) const;

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnStaminaChanged(const FOnAttributeChangeData& Data);
	void OnMaxStaminaChanged(const FOnAttributeChangeData& Data);

public:
	void InitInventory(UFLInventoryComponent* InInventoryComponent);

protected:
	virtual void NativeDestruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EquippedWeaponIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EquippedWeaponNameText;

	UPROPERTY()
	TObjectPtr<UFLInventoryComponent> InventoryComponent;

	void UpdateEquippedWeapon();
};
