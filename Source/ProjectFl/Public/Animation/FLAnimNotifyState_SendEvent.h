// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Abilities/GameplayAbility.h"
#include "FLAnimNotifyState_SendEvent.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class PROJECTFL_API UFLAnimNotifyState_SendEvent : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag Begin_EventTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag Tick_EventTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag End_EventTag;

public:
    virtual void NotifyBegin(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float TotalDuration
    ) override;

    virtual void NotifyTick(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float FrameDeltaTime
    ) override;

    virtual void NotifyEnd(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation
    ) override;

    void SendEvent(const USkeletalMeshComponent* MeshComp, FGameplayTag InGameplayTag);
};
