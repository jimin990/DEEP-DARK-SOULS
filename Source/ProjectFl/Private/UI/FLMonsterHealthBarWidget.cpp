// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FLMonsterHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "GAS/FLAttributeSet.h"

void UFLMonsterHealthBarWidget::InitAbilitySystem(UAbilitySystemComponent* InASC)
{
	ASC = InASC;

	if (!ASC)
	{
		return;
	}

	Health = ASC->GetNumericAttribute(UFLAttributeSet::GetHealthAttribute());
	MaxHealth = ASC->GetNumericAttribute(UFLAttributeSet::GetMaxHealthAttribute());

	UpdateHealthBar();

	ASC->GetGameplayAttributeValueChangeDelegate(
		UFLAttributeSet::GetHealthAttribute()
	).AddUObject(this, &UFLMonsterHealthBarWidget::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UFLAttributeSet::GetMaxHealthAttribute()
	).AddUObject(this, &UFLMonsterHealthBarWidget::OnMaxHealthChanged);
}

void UFLMonsterHealthBarWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	Health = Data.NewValue;
	UpdateHealthBar();
}

void UFLMonsterHealthBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	MaxHealth = Data.NewValue;
	UpdateHealthBar();
}

void UFLMonsterHealthBarWidget::UpdateHealthBar()
{
	if (HealthBar)
	{
		HealthBar->SetPercent(MaxHealth > 0.f ? Health / MaxHealth : 0.f);
	}
}