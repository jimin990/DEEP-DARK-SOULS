// Fill out your copyright notice in the Description page of Project Settings.


#include "Triggers/FLBossRoomTrigger.h"

#include "Characters/FLCharacterBoss.h"
#include "Characters/FLCharacterPlayer.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Player/FLPlayerController.h"
#include "Sound/SoundBase.h"
#include "Engine/TargetPoint.h"

AFLBossRoomTrigger::AFLBossRoomTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent =
		CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	SetRootComponent(BoxComponent);

	BoxComponent->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly
	);

	BoxComponent->SetCollisionObjectType(
		ECC_WorldDynamic
	);

	BoxComponent->SetCollisionResponseToAllChannels(
		ECR_Ignore
	);

	BoxComponent->SetCollisionResponseToChannel(
		ECC_Pawn,
		ECR_Overlap
	);
}

void AFLBossRoomTrigger::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(
		this,
		&AFLBossRoomTrigger::OnBoxBeginOverlap
	);

	BoxComponent->OnComponentEndOverlap.AddDynamic(
		this,
		&AFLBossRoomTrigger::OnBoxEndOverlap
	);
}

AFLCharacterBoss* AFLBossRoomTrigger::SpawnBoss()
{
	if (bBossSpawned)
	{
		return SpawnedBoss;
	}

	if (!BossClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BossClass is not assigned"));
		return nullptr;
	}

	if (!BossSpawnPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("BossSpawnPoint is not assigned"));
		return nullptr;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	SpawnedBoss = World->SpawnActor<AFLCharacterBoss>(
		BossClass,
		BossSpawnPoint->GetActorTransform(),
		SpawnParameters
	);

	if (!SpawnedBoss)
	{
		UE_LOG(LogTemp, Error, TEXT("Boss spawn failed"));
		return nullptr;
	}

	bBossSpawned = true;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Boss spawned at %s"),
		*BossSpawnPoint->GetActorLocation().ToString()
	);

	return SpawnedBoss;
}

void AFLBossRoomTrigger::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AFLCharacterPlayer* Player =
		Cast<AFLCharacterPlayer>(OtherActor);

	if (!Player)
	{
		return;
	}

	AFLPlayerController* PC =
		Cast<AFLPlayerController>(Player->GetController());

	if (!PC)
	{
		return;
	}

	AFLCharacterBoss* Boss = SpawnBoss();

	if (!Boss)
	{
		return;
	}

	PC->ShowBossHealthBar(Boss);
	PC->PlayBossBGM(
		BossBGM,
		BGMFadeInTime,
		BGMVolume
	);
}

void AFLBossRoomTrigger::OnBoxEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!bHideWhenExit)
	{
		return;
	}

	AFLCharacterPlayer* Player =
		Cast<AFLCharacterPlayer>(OtherActor);

	if (!Player)
	{
		return;
	}

	if (AFLPlayerController* PC =
		Cast<AFLPlayerController>(Player->GetController()))
	{
		PC->HideBossHealthBar();
	}
}