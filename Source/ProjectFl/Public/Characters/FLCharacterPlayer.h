// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FLCharacterBase.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "FLCharacterPlayer.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class UFLInputConfigDataAsset;

UCLASS()
class PROJECTFL_API AFLCharacterPlayer : public AFLCharacterBase
{
	GENERATED_BODY()
	
public:
	AFLCharacterPlayer();
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext>  IMC;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> HealAction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	void Look(const FInputActionValue& Value);

	void Move(const FInputActionValue& Value);

	void Attack();

	void Dodge();

	void Heal();

	//GAS
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Input")
	FGameplayTag AttackInputTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Input")
	FGameplayTag DodgeInputTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Input")
	FGameplayTag HealInputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UFLInputConfigDataAsset> InputConfig;

	void AbilityInputPressed(FGameplayTag InputTag);

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> InventoryAction;

	void ToggleInventory();

public:
	virtual void Die() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
	float RespawnDelay = 3.f;
};
