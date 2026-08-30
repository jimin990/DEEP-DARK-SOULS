// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "FLMonsterAIController.generated.h"

class UBehaviorTree;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class PROJECTFL_API AFLMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AFLMonsterAIController();

public:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
