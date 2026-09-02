// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FLCharacterBase.h"
#include "FLCharacterMonster.generated.h"

class UWidgetComponent;

UCLASS()
class PROJECTFL_API AFLCharacterMonster : public AFLCharacterBase
{
	GENERATED_BODY()

public:
	AFLCharacterMonster();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

public:
	virtual void Die() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	float DestroyDelayAfterDeath = 5.f;

public:
	void ShowHealthBar();
	void HideHealthBar();

protected:
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Attributes"
	)
	float InitialHealth = 30.f;
};