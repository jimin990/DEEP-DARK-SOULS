// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "FLAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class PROJECTFL_API UFLAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UFLAttributeSet();

	// Attribute 안에 수치가 변경될대 실행되는 함수
	virtual void PostGameplayEffectExecute(
		const struct FGameplayEffectModCallbackData& Data
	) override;

protected:
	// 체력 수치가 변경될때 처리 함수
	void HandleHealthChanged(
		const struct FGameplayEffectModCallbackData& Data
	);

public:
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UFLAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UFLAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UFLAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UFLAttributeSet, MaxStamina)
};
