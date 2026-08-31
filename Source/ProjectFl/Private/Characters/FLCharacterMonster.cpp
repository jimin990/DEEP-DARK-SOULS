// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FLCharacterMonster.h"
#include "Components/WidgetComponent.h"
#include "UI/FLMonsterHealthBarWidget.h"

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
	}
}