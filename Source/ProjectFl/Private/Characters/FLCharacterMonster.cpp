// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FLCharacterMonster.h"
#include "Components/WidgetComponent.h"
#include "UI/FLMonsterHealthBarWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "AbilitySystemComponent.h"
#include "GAS/FLAttributeSet.h"
#include "BehaviorTree/BlackboardComponent.h"

AFLCharacterMonster::AFLCharacterMonster()
{
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(
		TEXT("HealthBarWidgetComponent")
	);

	HealthBarWidgetComponent->SetupAttachment(GetMesh());

	HealthBarWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(120.f, 12.f));
	HealthBarWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFLCharacterMonster::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetNumericAttributeBase(
			UFLAttributeSet::GetMaxHealthAttribute(),
			InitialHealth
		);

		AbilitySystemComponent->SetNumericAttributeBase(
			UFLAttributeSet::GetHealthAttribute(),
			InitialHealth
		);
	}

	if (HealthBarWidgetComponent)
	{
		UFLMonsterHealthBarWidget* HealthBarWidget =
			Cast<UFLMonsterHealthBarWidget>(
				HealthBarWidgetComponent->GetUserWidgetObject()
			);

		if (HealthBarWidget)
		{
			HealthBarWidget->InitAbilitySystem(GetAbilitySystemComponent());
		}

		HideHealthBar();
	}
}

void AFLCharacterMonster::Die()
{
	Super::Die();

	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false);
	}

	AAIController* AIController =
		Cast<AAIController>(GetController());

	if (AIController)
	{
		// 이동 요청 취소
		AIController->StopMovement();

		// 타깃을 바라보는 Focus 해제
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		AIController->ClearFocus(EAIFocusPriority::Default);

		// Blackboard 타깃 제거
		if (UBlackboardComponent* BB =
			AIController->GetBlackboardComponent())
		{
			BB->ClearValue(TEXT("Target"));
			BB->SetValueAsBool(TEXT("IsDead"), true);
		}

		if (UBrainComponent* BrainComponent =
			AIController->GetBrainComponent())
		{
			// BTTask에서 종료하도록 
			//BrainComponent->StopLogic(TEXT("Monster Dead"));
		}
	}

	SetLifeSpan(DestroyDelayAfterDeath);
}

void AFLCharacterMonster::ShowHealthBar()
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(true);
	}
}

void AFLCharacterMonster::HideHealthBar()
{
	if (HealthBarWidgetComponent)
	{
		HealthBarWidgetComponent->SetVisibility(false);
	}
}