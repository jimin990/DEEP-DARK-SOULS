// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FLPlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GAS/FLAttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Weapons/FLInventoryComponent.h"
#include "Weapons/FLWeaponDataAsset.h"

void UFLPlayerHUDWidget::InitAbilitySystem(UAbilitySystemComponent* InASC)
{
	UE_LOG(LogTemp, Warning, TEXT("Init Widget"));

	ASC = InASC;

	if (!HealCooldownTag.IsValid())
	{
		HealCooldownTag =
			FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Heal"), false);
	}

	if (!ASC)
	{
		return;
	}

	Health = ASC->GetNumericAttribute(UFLAttributeSet::GetHealthAttribute());
	MaxHealth = ASC->GetNumericAttribute(UFLAttributeSet::GetMaxHealthAttribute());
	Stamina = ASC->GetNumericAttribute(UFLAttributeSet::GetStaminaAttribute());
	MaxStamina = ASC->GetNumericAttribute(UFLAttributeSet::GetMaxStaminaAttribute());

	UpdateHealthBar();
	UpdateStaminaBar();
	UpdateHealCooldown();

	ASC->GetGameplayAttributeValueChangeDelegate(
		UFLAttributeSet::GetHealthAttribute()
	).AddUObject(this, &UFLPlayerHUDWidget::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UFLAttributeSet::GetMaxHealthAttribute()
	).AddUObject(this, &UFLPlayerHUDWidget::OnMaxHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UFLAttributeSet::GetStaminaAttribute()
	).AddUObject(this, &UFLPlayerHUDWidget::OnStaminaChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UFLAttributeSet::GetMaxStaminaAttribute()
	).AddUObject(this, &UFLPlayerHUDWidget::OnMaxStaminaChanged);
}

void UFLPlayerHUDWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateHealCooldown();
}

void UFLPlayerHUDWidget::UpdateHealthBar()
{
	if (HealthBar)
	{
		HealthBar->SetPercent(MaxHealth > 0.f ? Health / MaxHealth : 0.f);
	}
}

void UFLPlayerHUDWidget::UpdateStaminaBar()
{
	if (StaminaBar)
	{
		StaminaBar->SetPercent(MaxStamina > 0.f ? Stamina / MaxStamina : 0.f);
	}
}

void UFLPlayerHUDWidget::UpdateHealCooldown()
{
	if (!HealCooldownBar && !HealCooldownText)
	{
		return;
	}

	float Remaining = 0.f;
	float Duration = 0.f;

	const bool bHasCooldown =
		GetCooldownTimeRemainingAndDuration(
			HealCooldownTag,
			Remaining,
			Duration
		);

	if (!bHasCooldown || Duration <= 0.f)
	{
		if (HealCooldownBar)
		{
			HealCooldownBar->SetPercent(0.f);
		}

		if (HealCooldownText)
		{
			HealCooldownText->SetText(FText::GetEmpty());
		}

		return;
	}

	const float Percent = Remaining / Duration;

	if (HealCooldownBar)
	{
		HealCooldownBar->SetPercent(Percent);
	}

	if (HealCooldownText)
	{
		HealCooldownText->SetText(
			FText::AsNumber(FMath::CeilToInt(Remaining))
		);
	}
}

bool UFLPlayerHUDWidget::GetCooldownTimeRemainingAndDuration(
	FGameplayTag CooldownTag,
	float& OutRemaining,
	float& OutDuration
) const
{
	OutRemaining = 0.f;
	OutDuration = 0.f;

	if (!ASC || !CooldownTag.IsValid())
	{
		return false;
	}

	FGameplayTagContainer CooldownTags;
	CooldownTags.AddTag(CooldownTag);

	const FGameplayEffectQuery Query =
		FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTags);

	const TArray<TPair<float, float>> TimePairs =
		ASC->GetActiveEffectsTimeRemainingAndDuration(Query);

	for (const TPair<float, float>& TimePair : TimePairs)
	{
		const float Remaining = TimePair.Key;
		const float Duration = TimePair.Value;

		if (Remaining > OutRemaining)
		{
			OutRemaining = Remaining;
			OutDuration = Duration;
		}
	}

	return OutRemaining > 0.f;
}

void UFLPlayerHUDWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	Health = Data.NewValue;
	UpdateHealthBar();
}

void UFLPlayerHUDWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	MaxHealth = Data.NewValue;
	UpdateHealthBar();
}

void UFLPlayerHUDWidget::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	Stamina = Data.NewValue;
	UpdateStaminaBar();
}

void UFLPlayerHUDWidget::OnMaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	MaxStamina = Data.NewValue;
	UpdateStaminaBar();
}

void UFLPlayerHUDWidget::InitInventory(
	UFLInventoryComponent* InInventoryComponent
)
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveAll(this);
	}

	InventoryComponent = InInventoryComponent;

	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.AddUObject(
			this,
			&UFLPlayerHUDWidget::UpdateEquippedWeapon
		);
	}

	UpdateEquippedWeapon();
}

void UFLPlayerHUDWidget::NativeDestruct()
{
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UFLPlayerHUDWidget::UpdateEquippedWeapon()
{
	if (!EquippedWeaponIcon || !EquippedWeaponNameText)
	{
		return;
	}

	UFLWeaponDataAsset* EquippedWeapon =
		InventoryComponent ? InventoryComponent->GetEquippedWeapon() : nullptr;

	if (!EquippedWeapon)
	{
		EquippedWeaponIcon->SetVisibility(ESlateVisibility::Hidden);
		EquippedWeaponNameText->SetText(FText::GetEmpty());
		return;
	}

	EquippedWeaponIcon->SetVisibility(ESlateVisibility::Visible);
	EquippedWeaponIcon->SetBrushFromTexture(EquippedWeapon->WeaponIcon);
	EquippedWeaponNameText->SetText(EquippedWeapon->WeaponName);
}