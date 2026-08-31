// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "FLMonsterHealthBarWidget.generated.h"

class UProgressBar;
class UAbilitySystemComponent;

UCLASS()
class PROJECTFL_API UFLMonsterHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitAbilitySystem(UAbilitySystemComponent* InASC);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	float Health = 0.f;
	float MaxHealth = 1.f;

	void UpdateHealthBar();

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
};
