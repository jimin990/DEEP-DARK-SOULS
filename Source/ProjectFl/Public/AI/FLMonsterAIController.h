// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FLMonsterAIController.generated.h"

class UBehaviorTree;

UCLASS()
class PROJECTFL_API AFLMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
};
