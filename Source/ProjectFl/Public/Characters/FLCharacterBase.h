// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "FLCharacterBase.generated.h"

class UAbilitySystemComponent;
class UFLAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UStaticMesh;
class UStaticMeshComponent;
class UFLWeaponDataAsset;
class UFLCombatComponent;
class UFLAbilitySetPrimaryDataAsset;

UCLASS()
class PROJECTFL_API AFLCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFLCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

//Gameplay Ability System
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY()
	TObjectPtr<UFLAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Abilities")
	TObjectPtr<UFLAbilitySetPrimaryDataAsset> AbilitySet;

	void GiveDefaultAbilities();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> HitReactMontage;

public:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UFLCombatComponent> CombatComponent;

	UFLCombatComponent* GetCombatComponent() const;

public:
	// 공격 중 들어온 방향을 위한 마지막 입력 방향 저장
	FVector LastMoveInputWorldDirection = FVector::ZeroVector;

	FVector GetLastMoveInputWorldDirection() const
	{
		return LastMoveInputWorldDirection;
	}
};
