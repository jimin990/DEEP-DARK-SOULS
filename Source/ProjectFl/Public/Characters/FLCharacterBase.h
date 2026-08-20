// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "FLCharacterBase.generated.h"

class UAbilitySystemComponent;
class UFLAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UStaticMesh;
class UStaticMeshComponent;
class UFLWeaponDataAsset;

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
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UFLAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	void GiveDefaultAbilities();

public:
	UPROPERTY(EditAnywhere, Category = "Montage")
	TObjectPtr<UAnimMontage> AttackMontage;

public:
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float DefaultTraceRadius = 20.f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UFLWeaponDataAsset> WeaponData;

	void EquipWeapon(UFLWeaponDataAsset* InWeaponData);

public:
	UPROPERTY(EditAnywhere, Category = "Combat|Trace")
	FName DefaultTraceStartSocketName;

	UPROPERTY(EditAnywhere, Category = "Combat|Trace")
	FName DefaultTraceEndSocketName;
};
