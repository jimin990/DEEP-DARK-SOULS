// Fill out your copyright notice in the Description page of Project Settings.


#include "Triggers/FLBossRoomTrigger.h"
#include "Characters/FLCharacterPlayer.h"
#include "Components/BoxComponent.h"
#include "Player/FLPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

AFLBossRoomTrigger::AFLBossRoomTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    SetRootComponent(BoxComponent);

    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxComponent->SetCollisionObjectType(ECC_WorldDynamic);
    BoxComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
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

void AFLBossRoomTrigger::OnBoxBeginOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    AFLCharacterPlayer* Player = Cast<AFLCharacterPlayer>(OtherActor);

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

    PC->ShowBossHealthBar(BossActor);

    PC->PlayBossBGM(BossBGM, BGMFadeInTime, BGMVolume);
}

void AFLBossRoomTrigger::OnBoxEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    if (!bHideWhenExit)
    {
        return;
    }

    AFLCharacterPlayer* Player = Cast<AFLCharacterPlayer>(OtherActor);

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

    PC->HideBossHealthBar();
}