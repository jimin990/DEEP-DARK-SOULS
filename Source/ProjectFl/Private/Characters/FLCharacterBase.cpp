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
#include "Combat/FLCombatComponent.h"
#include "Combat/FLAbilitySetPrimaryDataAsset.h"

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

	// Combat
	CombatComponent = CreateDefaultSubobject<UFLCombatComponent>(TEXT("CombatComponent"));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UFLAttributeSet>(TEXT("AttributeSet"));

	WeaponMeshComponent =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	// 튀김 방지 - 추후 무기 클래스로 빼거나, 에셋설정으로 수정하자
	WeaponMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
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

void AFLCharacterBase::GiveDefaultAbilities()
{
	if (!AbilitySystemComponent || !AbilitySet)
	{
		return;
	}

	for (const FFLAbilitySetItem& AbilityItem : AbilitySet->Abilities)
	{
		if (!AbilityItem.AbilityClass)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec(
			AbilityItem.AbilityClass,
			AbilityItem.AbilityLevel
		);

		if (AbilityItem.ActivationTag.IsValid())
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityItem.ActivationTag);
		}

		AbilitySystemComponent->GiveAbility(AbilitySpec);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Give Ability: %s / Tag: %s"),
			*AbilityItem.AbilityClass->GetName(),
			*AbilityItem.ActivationTag.ToString()
		);
	}
}

UFLCombatComponent* AFLCharacterBase::GetCombatComponent() const
{
	return CombatComponent;
}

