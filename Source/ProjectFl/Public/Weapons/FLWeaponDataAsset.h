// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FLWeaponDataAsset.generated.h"

class UStaticMesh;

UCLASS()
class PROJECTFL_API UFLWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Mesh|SocketName")
	FName TraceStartSocketName;

	UPROPERTY(EditAnywhere, Category = "Mesh|SocketName")
	FName TraceEndSocketName;
	 
	UPROPERTY(EditAnywhere, Category = "Mesh|SocketName")
	float TraceRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FTransform AttachOffset;
};
