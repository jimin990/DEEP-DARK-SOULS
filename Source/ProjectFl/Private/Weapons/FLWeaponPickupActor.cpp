// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/FLWeaponPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Weapons/FLWeaponDataAsset.h"
#include "Characters/FLCharacterPlayer.h"
#include "Player/FLPlayerState.h"
#include "Weapons/FLInventoryComponent.h"

AFLWeaponPickupActor::AFLWeaponPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);

	SphereComponent->SetSphereRadius(80.f);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SphereComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFLWeaponPickupActor::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponData && WeaponData->WeaponMesh)
	{
		MeshComponent->SetStaticMesh(WeaponData->WeaponMesh);
	}

	SphereComponent->OnComponentBeginOverlap.AddDynamic(
		this,
		&AFLWeaponPickupActor::OnOverlapBegin
	);
}

void AFLWeaponPickupActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!WeaponData)
	{
		return;
	}

	AFLCharacterPlayer* PlayerCharacter = Cast<AFLCharacterPlayer>(OtherActor);

	if (!PlayerCharacter)
	{
		return;
	}

	AFLPlayerState* FLPlayerState = PlayerCharacter->GetPlayerState<AFLPlayerState>();

	if (!FLPlayerState)
	{
		return;
	}

	UFLInventoryComponent* InventoryComponent = FLPlayerState->GetInventoryComponent();

	if (!InventoryComponent)
	{
		return;
	}

	if (InventoryComponent->AddWeapon(WeaponData))
	{
		Destroy();
	}
}