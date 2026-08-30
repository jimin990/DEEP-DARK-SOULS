// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FLAnimNotifyState_SendEvent.h"
#include "Characters/FLCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void UFLAnimNotifyState_SendEvent::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

    SendEvent(MeshComp, Begin_EventTag);
}

void UFLAnimNotifyState_SendEvent::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    SendEvent(MeshComp, Tick_EventTag);
}

void UFLAnimNotifyState_SendEvent::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

    SendEvent(MeshComp, End_EventTag);

}

void UFLAnimNotifyState_SendEvent::SendEvent(const USkeletalMeshComponent* MeshComp, FGameplayTag InGameplayTag)
{
    if (!InGameplayTag.IsValid())
    {
        return;
    }

    if (!MeshComp)
    {
        return;
    }

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner)
    {
        return;
    }

    FGameplayEventData EventData;
    EventData.EventTag = InGameplayTag;
    EventData.Instigator = Owner;
    EventData.Target = Owner;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        Owner,
        InGameplayTag,
        EventData
    );
}

