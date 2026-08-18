// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FLPlayerAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS()
class PROJECTFL_API UFLPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference")
	TObjectPtr<ACharacter> Character;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reference")
	TObjectPtr<UCharacterMovementComponent> MovementComp;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	bool bIsIdle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	float AimYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Data")
	float AimPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Data")
	FVector Velocity;
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
