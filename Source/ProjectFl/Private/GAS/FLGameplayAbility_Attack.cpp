// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FLGameplayAbility_Attack.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/FLAttributeSet.h"

UFLGameplayAbility_Attack::UFLGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("GA_Attack Activated"));

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

	if (ASC && DamageEffectClass)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle =
			ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

		if (SpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	const UFLAttributeSet* FLAttributeSet =
		ASC->GetSet<UFLAttributeSet>();

	if (FLAttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("Health: %f"), FLAttributeSet->GetHealth());
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}
