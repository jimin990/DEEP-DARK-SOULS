// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FLPlayerAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "KismetAnimationLibrary.h"

void UFLPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (OwnerCharacter = Cast<ACharacter>(GetOwningActor()))
	{
		MovementComp = OwnerCharacter->GetCharacterMovement();
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

		FRotator ControlRot = OwnerCharacter->GetControlRotation();
		FRotator PlayerRot = OwnerCharacter->GetActorRotation();
		FRotator DeltaRot = (ControlRot - PlayerRot).GetNormalized();

		AimYaw = DeltaRot.Yaw;
		AimPitch = DeltaRot.Pitch;

		Lean = UKismetAnimationLibrary::CalculateDirection(
			Velocity,
			OwnerCharacter->GetActorRotation()
		);

		SlopeForwardAngle = GroundSpeed;
	}

	UAbilitySystemComponent* ASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerCharacter);

	if (!ASC)
	{
		bIsDead = false;
		return;
	}

	const FGameplayTag DeadTag =
		FGameplayTag::RequestGameplayTag(TEXT("State.Dead"), false);

	bIsDead = DeadTag.IsValid() && ASC->HasMatchingGameplayTag(DeadTag);
}
