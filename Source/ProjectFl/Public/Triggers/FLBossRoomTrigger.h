// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FLBossRoomTrigger.generated.h"

class UBoxComponent;
class USoundBase;
class UAudioComponent;

UCLASS()
class PROJECTFL_API AFLBossRoomTrigger : public AActor
{
    GENERATED_BODY()

public:
    AFLBossRoomTrigger();

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UBoxComponent> BoxComponent;

    UPROPERTY(EditInstanceOnly, Category = "Boss")
    TObjectPtr<AActor> BossActor;

    UPROPERTY(EditAnywhere, Category = "Boss")
    bool bHideWhenExit = false;

private:
    UFUNCTION()
    void OnBoxBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnBoxEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Room|Sound")
    TObjectPtr<USoundBase> BossBGM;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Room|Sound")
    float BGMFadeInTime = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Room|Sound")
    float BGMVolume = 1.f;
};