// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FLAttackAnimNotifyState.h"
#include "Characters/FLCharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"

void UFLAttackAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UFLAttackAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);

    if (!MeshComp)
    {
        return;
    }

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner)
    {
        return;
    }

    const FGameplayTag AttackTraceEventTag =
        FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.Trace"));

    FGameplayEventData EventData;
    EventData.EventTag = AttackTraceEventTag;
    EventData.Instigator = Owner;
    EventData.Target = Owner;

    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
        Owner,
        AttackTraceEventTag,
        EventData
    );
}

void UFLAttackAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

}
