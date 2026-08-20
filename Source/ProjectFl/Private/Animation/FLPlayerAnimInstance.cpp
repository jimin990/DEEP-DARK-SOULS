// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FLPlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFLPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (Character = Cast<ACharacter>(GetOwningActor()))
	{
		MovementComp = Character->GetCharacterMovement();
	}
}

void UFLPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (MovementComp)
	{
		Velocity = MovementComp->Velocity;
		GroundSpeed = Velocity.Size2D();
		bIsIdle = GroundSpeed < 3.f && MovementComp->GetCurrentAcceleration() == FVector::ZeroVector;
		bIsFalling = MovementComp->IsFalling();

		FRotator ControlRot = Character->GetControlRotation();
		FRotator PlayerRot = Character->GetActorRotation();
		FRotator DeltaRot = (ControlRot - PlayerRot).GetNormalized();

		AimYaw = DeltaRot.Yaw;
		AimPitch = DeltaRot.Pitch;
	}
}
