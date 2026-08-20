// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FLCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "GAS/FLAttributeSet.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"
#include "Weapons/FLWeaponDataAsset.h"

// Sets default values
AFLCharacterBase::AFLCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.f);

	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;;

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UFLAttributeSet>(TEXT("AttributeSet"));

	WeaponMeshComponent =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
}

// Called when the game starts or when spawned
void AFLCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	GiveDefaultAbilities();

	if (WeaponData)
	{
		EquipWeapon(WeaponData);
	}
}

// Called every frame
void AFLCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFLCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AFLCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float AFLCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return 0.0f;
}

// 무기 수정 필요 Interface로 빼던지 해야할듯
void AFLCharacterBase::EquipWeapon(UFLWeaponDataAsset* InWeaponData)
{
	WeaponMeshComponent->SetStaticMesh(InWeaponData->WeaponMesh);

	WeaponMeshComponent->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		DefaultTraceStartSocketName
	);

	WeaponMeshComponent->SetRelativeTransform(InWeaponData->AttachOffset);

	DefaultTraceStartSocketName = WeaponData ->TraceStartSocketName;
	DefaultTraceEndSocketName = WeaponData->TraceEndSocketName;
	DefaultTraceRadius = WeaponData->TraceRadius;
}

void AFLCharacterBase::GiveDefaultAbilities()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		if (!AbilityClass)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

