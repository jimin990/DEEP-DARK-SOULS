// Fill out your copyright notice in the Description page of Project Settings.

#include "Combat/FLCombatComponent.h"
#include "Characters/FLCharacterBase.h"
#include "Weapons/FLWeaponDataAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

UFLCombatComponent::UFLCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UFLCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}

void UFLCombatComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<AFLCharacterBase>(GetOwner());

    // 무기 데이터가 없다면 공격 불가
    if (!WeaponData)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponData is null"));
    }

    EquipWeapon(WeaponData);
}

bool UFLCombatComponent::GetAttackTraceInfo(
    FVector& OutStart,
    FVector& OutEnd,
    float& OutRadius,
    int32 CurComboIndex
) const
{
    UFLWeaponDataAsset* CurWeaponData = GetCurrentWeaponData();

    if (!OwnerCharacter || !CurWeaponData)
    {
        return false;
    }

    USceneComponent* TraceComponent = OwnerCharacter->GetMesh();

    if (OwnerCharacter->WeaponMeshComponent)
    {
        TraceComponent = OwnerCharacter->WeaponMeshComponent;
    }

    if (!TraceComponent)
    {
        return false;
    }

    OutStart = TraceComponent->GetSocketLocation(CurWeaponData->ComboAttacks[CurComboIndex].TraceStartSocketName);
    OutEnd = TraceComponent->GetSocketLocation(CurWeaponData->ComboAttacks[CurComboIndex].TraceEndSocketName);
    OutRadius = CurWeaponData->ComboAttacks[CurComboIndex].TraceRadius;
    
    return true;
}

UFLWeaponDataAsset* UFLCombatComponent::GetCurrentWeaponData() const
{
    if (!CurrentWeaponData)
    {
        UE_LOG(LogTemp, Warning, TEXT("CurrentWeaponData is Null!"));
        return nullptr;
    }

    return CurrentWeaponData;
}

void UFLCombatComponent::EquipWeapon(UFLWeaponDataAsset* InWeaponData)
{
    if (!OwnerCharacter || !InWeaponData)
    {
        return;
    }

    CurrentWeaponData = InWeaponData;

    if (OwnerCharacter->WeaponMeshComponent)
    {
        OwnerCharacter->WeaponMeshComponent->SetStaticMesh(InWeaponData->WeaponMesh);

        OwnerCharacter->WeaponMeshComponent->AttachToComponent(
            OwnerCharacter->GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            InWeaponData->AttachSocketName
        );

        OwnerCharacter->WeaponMeshComponent->SetRelativeTransform(InWeaponData->AttachOffset);
        OwnerCharacter->WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
/*
void UFLCombatComponent::EquipWeapon(UFLWeaponDataAsset* NewWeaponData)
{
    if (!NewWeaponData || !OwnerCharacter)
    {
        return;
    }

    WeaponData = NewWeaponData;

    if (OwnerCharacter->WeaponMeshComponent)
    {
        OwnerCharacter->WeaponMeshComponent->SetStaticMesh(
            WeaponData->WeaponMesh
        );

        OwnerCharacter->WeaponMeshComponent->AttachToComponent(
            OwnerCharacter->GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            WeaponData->AttachSocketName
        );
    }
}
*/

void UFLCombatComponent::UnequipWeapon()
{
    WeaponData = nullptr;
    CurrentWeaponData = nullptr;

    if (OwnerCharacter && OwnerCharacter->WeaponMeshComponent)
    {
        OwnerCharacter->WeaponMeshComponent->SetStaticMesh(nullptr);
    }
}