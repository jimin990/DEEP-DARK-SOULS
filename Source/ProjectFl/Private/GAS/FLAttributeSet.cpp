// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FLAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "Perception/AISense_Damage.h"

UFLAttributeSet::UFLAttributeSet()
{
	InitMaxHealth(100.f);
	InitHealth(100.f);

	InitMaxStamina(100.f);
	InitStamina(100.f);
}

void UFLAttributeSet::PostGameplayEffectExecute(
	const FGameplayEffectModCallbackData& Data
)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		HandleHealthChanged(Data);
		return;
	}

	if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Stamina Changed: %.1f / %.1f"),
			GetStamina(),
			GetMaxStamina()
		);

		return;
	}
}

void UFLAttributeSet::HandleHealthChanged(const FGameplayEffectModCallbackData& Data)
{
	SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

	if (GetHealth() > 0.f)
	{
		// 데미지를 감지해서 Damage Sense를 보냄
		AActor* TargetActor = GetOwningActor();
		AActor* InstigatorActor = nullptr;

		if (Data.EffectSpec.GetContext().GetOriginalInstigator())
		{
			InstigatorActor = Data.EffectSpec.GetContext().GetOriginalInstigator();
		}

		if (TargetActor && InstigatorActor)
		{
			UAISense_Damage::ReportDamageEvent(
				TargetActor->GetWorld(),
				TargetActor,
				InstigatorActor,
				GetHealth(),
				InstigatorActor->GetActorLocation(),
				TargetActor->GetActorLocation()
			);
		}

		return;
	}

	AActor* OwningActor = GetOwningActor();
	if (!OwningActor)
	{
		return;
	}

	const FGameplayTag DeathEventTag =
		FGameplayTag::RequestGameplayTag(TEXT("Event.Death"), false);

	if (!DeathEventTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Event.Death GameplayTag is invalid"));
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = DeathEventTag;
	EventData.Instigator = Data.EffectSpec.GetContext().GetInstigator();
	EventData.Target = OwningActor;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		OwningActor,
		DeathEventTag,
		EventData
	);

	UE_LOG(LogTemp, Warning, TEXT("Death Event Sent"));
}
