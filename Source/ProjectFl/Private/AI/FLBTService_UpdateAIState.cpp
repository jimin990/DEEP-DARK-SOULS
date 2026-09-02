// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLBTService_UpdateAIState.h"
#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameplayTagContainer.h"

UFLBTService_UpdateAIState::UFLBTService_UpdateAIState()
{
    NodeName = TEXT("Update AI State");
    Interval = 0.2f;
    RandomDeviation = 0.05f;
}

void UFLBTService_UpdateAIState::TickNode(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds
)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* AIPawn = AIController ? AIController->GetPawn() : nullptr;
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

    if (!AIPawn || !BB)
    {
        return;
    }

    UAbilitySystemComponent* ASC =
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIPawn);

    const FGameplayTag DeadTag =
        FGameplayTag::RequestGameplayTag(TEXT("State.Dead"), false);

    const bool bIsDead =
        ASC && DeadTag.IsValid() && ASC->HasMatchingGameplayTag(DeadTag);

    const FGameplayTag HitReactTag =
        FGameplayTag::RequestGameplayTag(TEXT("State.HitReact"), false);

    const bool bIsHitReact =
        ASC && HitReactTag.IsValid() && ASC->HasMatchingGameplayTag(HitReactTag);

    const bool bCanUseNormalAttack =
        ASC && !ASC->HasMatchingGameplayTag(
            FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Attack"))
        );

    const bool bCanUseChargeAttack =
        ASC && !ASC->HasMatchingGameplayTag(
            FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Boss.Charge"))
        );

    const bool bCanUseAreaAttack =
        ASC && !ASC->HasMatchingGameplayTag(
            FGameplayTag::RequestGameplayTag(TEXT("Cooldown.Boss.Area"))
        );

    BB->SetValueAsBool(CanUseNormalAttackKey.SelectedKeyName, bCanUseNormalAttack);
    BB->SetValueAsBool(CanUseChargeAttackKey.SelectedKeyName, bCanUseChargeAttack);
    BB->SetValueAsBool(CanUseAreaAttackKey.SelectedKeyName, bCanUseAreaAttack);

    BB->SetValueAsBool(IsDeadKey.SelectedKeyName, bIsDead);

    if (bIsDead)
    {
        BB->SetValueAsEnum(
            AIStateKey.SelectedKeyName,
            static_cast<uint8>(EFLAIState::Dead)
        );

        return;
    }

    if (bIsHitReact)
    {
        BB->SetValueAsEnum(
            AIStateKey.SelectedKeyName,
            static_cast<uint8>(EFLAIState::HitReact)
        );
        return;
    }

    AActor* TargetActor =
        Cast<AActor>(BB->GetValueAsObject(TargetKey.SelectedKeyName));

    if (!TargetActor)
    {
        BB->SetValueAsFloat(DistanceKey.SelectedKeyName, 0.f);

        BB->SetValueAsEnum(
            AIStateKey.SelectedKeyName,
            static_cast<uint8>(EFLAIState::Patrol)
        );

        return;
    }

    const float Distance = FVector::Dist(
        AIPawn->GetActorLocation(),
        TargetActor->GetActorLocation()
    );

    BB->SetValueAsFloat(DistanceKey.SelectedKeyName, Distance);

    UE_LOG(LogTemp, Warning, TEXT("Distance %f"), Distance);

    const EFLAIState NewState =
        Distance <= CombatRange
        ? EFLAIState::Combat
        : EFLAIState::Chase;

    BB->SetValueAsEnum(
        AIStateKey.SelectedKeyName,
        static_cast<uint8>(NewState)
    );
}