// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FLCombatComponent.generated.h"

class AFLCharacterBase;
class UFLWeaponDataAsset;
class UFLAttackDataAsset;
class UStaticMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTFL_API UFLCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFLCombatComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    virtual void BeginPlay() override;

public:
    bool GetAttackTraceInfo(
        FVector& OutStart,
        FVector& OutEnd,
        float& OutRadius,
        int32 CurComboIndex
    ) const;

private:
    UPROPERTY()
    TObjectPtr<AFLCharacterBase> OwnerCharacter;

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UFLWeaponDataAsset> WeaponData;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UFLWeaponDataAsset> CurrentWeaponData;

    UFLWeaponDataAsset* GetCurrentWeaponData() const;

public:
    void EquipWeapon(UFLWeaponDataAsset* NewWeaponData);
    void UnequipWeapon();
};
