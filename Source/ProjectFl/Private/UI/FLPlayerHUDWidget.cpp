// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FLPlayerHUDWidget.h"
#include "Components/ProgressBar.h"
#include "GAS/FLAttributeSet.h"

void UFLPlayerHUDWidget::InitAbilitySystem(UAbilitySystemComponent* InASC)
{
	UE_LOG(LogTemp, Warning, TEXT("Init Widget"));
	ASC = InASC;

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