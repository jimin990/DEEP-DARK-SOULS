// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FLBossRoomTrigger.generated.h"

class UBoxComponent;
class USceneComponent;
class USoundBase;
class AFLCharacterBoss;
class ATargetPoint;

UCLASS()
class PROJECTFL_API AFLBossRoomTrigger : public AActor
{
	GENERATED_BODY()

public:
	AFLBossRoomTrigger();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Boss Room")
	TObjectPtr<UBoxComponent> BoxComponent;

	// 스폰할 보스 BP 클래스
	UPROPERTY(EditAnywhere, Category = "Boss Room|Spawn")
	TSubclassOf<AFLCharacterBoss> BossClass;

	// 실제로 생성된 보스
	UPROPERTY(Transient)
	TObjectPtr<AFLCharacterBoss> SpawnedBoss;

	UPROPERTY(EditAnywhere, Category = "Boss Room")
	bool bHideWhenExit = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Room|Sound")
	TObjectPtr<USoundBase> BossBGM;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Room|Sound")
	float BGMFadeInTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss Room|Sound")
	float BGMVolume = 1.f;

private:
	bool bBossSpawned = false;

	AFLCharacterBoss* SpawnBoss();

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

	// 월드에 배치한 TargetPoint를 지정
	UPROPERTY(EditInstanceOnly, Category = "Boss Room|Spawn")
	TObjectPtr<ATargetPoint> BossSpawnPoint;
};