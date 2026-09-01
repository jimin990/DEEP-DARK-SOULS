// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FLCharacterBase.h"
#include "FLCharacterBoss.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFL_API AFLCharacterBoss : public AFLCharacterBase
{
	GENERATED_BODY()
	
public:
	AFLCharacterBoss();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Die() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	float DestroyDelayAfterDeath = 5.f;
};
