// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FLBTTask_ActivateAbilityByTag.h"
#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UFLBTTask_ActivateAbilityByTag::UFLBTTask_ActivateAbilityByTag()
{
	NodeName = TEXT("Activate Ability By Tag");
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UFLBTTask_ActivateAbilityByTag::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController || !AIController->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	CachedASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIController->GetPawn());

	if (!CachedASC)
	{
		return EBTNodeResult::Failed;
	}

	CachedOwnerComp = &OwnerComp;

	CachedASC->OnAbilityEnded.AddUObject(
		this,
		&UFLBTTask_ActivateAbilityByTag::OnAbilityEnded
	);

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(AbilityTag);

	//const bool bActivated = CachedASC->TryActivateAbilitiesByTag(TagContainer);

	for (const FGameplayAbilitySpec& Spec : CachedASC->GetActivatableAbilities())
	{
		if (Spec.DynamicAbilityTags.HasTagExact(AbilityTag))
		{
			const bool bActivated = CachedASC->TryActivateAbility(Spec.Handle);

			if (!bActivated)
			{
				CachedASC->OnAbilityEnded.RemoveAll(this);
				CachedASC = nullptr;
				CachedOwnerComp = nullptr;

				return EBTNodeResult::Failed;
			}

			break;
		}
	}

	/*if (!bActivated)
	{
		CachedASC->OnAbilityEnded.RemoveAll(this);
		CachedASC = nullptr;
		CachedOwnerComp = nullptr;

		return EBTNodeResult::Failed;
	}*/

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UFLBTTask_ActivateAbilityByTag::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	/*if (CachedASC)
	{
		FGameplayTagContainer TagsToCancel;
		TagsToCancel.AddTag(AbilityTag);

		CachedASC->CancelAbilities(
			&TagsToCancel,
			nullptr,
			nullptr
		);

		CachedASC->OnAbilityEnded.RemoveAll(this);
	}

	CachedASC = nullptr;
	CachedOwnerComp = nullptr;

	return EBTNodeResult::Aborted;*/

	UE_LOG(LogTemp, Warning, TEXT("Task abort!"));
	return EBTNodeResult::Failed;
}

void UFLBTTask_ActivateAbilityByTag::OnAbilityEnded(
	const FAbilityEndedData& AbilityEndedData
)
{
	if (!CachedOwnerComp)
	{
		return;
	}

	if (!AbilityEndedData.AbilityThatEnded)
	{
		return;
	}

	if (!AbilityEndedData.AbilityThatEnded->AbilityTags.HasTag(AbilityTag))
	{
		return;
	}

	if (CachedASC)
	{
		CachedASC->OnAbilityEnded.RemoveAll(this);
	}

	UBehaviorTreeComponent* OwnerComp = CachedOwnerComp;

	CachedASC = nullptr;
	CachedOwnerComp = nullptr;

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UFLBTTask_ActivateAbilityByTag::OnTaskFinished(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	EBTNodeResult::Type TaskResult
)
{
	if (CachedASC)
	{
		CachedASC->OnAbilityEnded.RemoveAll(this);
	}

	CachedASC = nullptr;
	CachedOwnerComp = nullptr;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}