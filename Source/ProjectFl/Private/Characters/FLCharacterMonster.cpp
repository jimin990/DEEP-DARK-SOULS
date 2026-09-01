// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FLCharacterMonster.h"
#include "Components/WidgetComponent.h"
#include "UI/FLMonsterHealthBarWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"

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

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* BrainComponent = AIController->GetBrainComponent())
		{
			BrainComponent->StopLogic(TEXT("Monster Dead"));
		}

		AIController->StopMovement();
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