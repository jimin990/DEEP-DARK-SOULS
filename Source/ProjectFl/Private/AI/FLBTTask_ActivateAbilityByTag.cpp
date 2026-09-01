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

	bCreateNodeInstance = true;
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

	for (const FGameplayAbilitySpec& Spec : CachedASC->GetActivatableAbilities())
	{
		if (!Spec.DynamicAbilityTags.HasTagExact(AbilityTag))
		{
			continue;
		}

		const bool bActivated = CachedASC->TryActivateAbility(Spec.Handle);

		if (!bActivated)
		{
			CachedASC->OnAbilityEnded.RemoveAll(this);
			CachedASC = nullptr;
			CachedOwnerComp = nullptr;
			ActivatedAbilityHandle = FGameplayAbilitySpecHandle();

			return EBTNodeResult::Failed;
		}

		ActivatedAbilityHandle = Spec.Handle;

		return EBTNodeResult::InProgress;
	}

	CachedASC->OnAbilityEnded.RemoveAll(this);
	CachedASC = nullptr;
	CachedOwnerComp = nullptr;
	ActivatedAbilityHandle = FGameplayAbilitySpecHandle();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Ability not found by tag: %s"),
		*AbilityTag.ToString()
	);

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UFLBTTask_ActivateAbilityByTag::AbortTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	UAbilitySystemComponent* ASC = CachedASC.Get();
	const FGameplayAbilitySpecHandle AbilityHandle = ActivatedAbilityHandle;

	if (ASC)
	{
		ASC->OnAbilityEnded.RemoveAll(this);
	}

	CachedASC = nullptr;
	CachedOwnerComp = nullptr;
	ActivatedAbilityHandle = FGameplayAbilitySpecHandle();

	if (ASC && AbilityHandle.IsValid())
	{
		ASC->CancelAbilityHandle(AbilityHandle);
	}

	UE_LOG(LogTemp, Warning, TEXT("Task abort!"));

	return EBTNodeResult::Aborted;
}

void UFLBTTask_ActivateAbilityByTag::OnAbilityEnded(
	const FAbilityEndedData& AbilityEndedData
)
{
	if (!CachedOwnerComp)
	{
		return;
	}

	if (AbilityEndedData.AbilitySpecHandle != ActivatedAbilityHandle)
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
	ActivatedAbilityHandle = FGameplayAbilitySpecHandle();

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
	ActivatedAbilityHandle = FGameplayAbilitySpecHandle();

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}