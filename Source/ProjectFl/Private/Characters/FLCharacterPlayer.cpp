// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FLCharacterPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/FLPlayerController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

AFLCharacterPlayer::AFLCharacterPlayer()
{
	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.f);

	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // 머리 높이


	SpringArmComponent->TargetArmLength = 300.f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AFLCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!IMC || !MoveAction || !LookAction || !JumpAction || !AttackAction)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputAsset is null!"));
		return;
	}

	UEnhancedInputComponent* EIC =
		Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EIC)
	{
		return;
	}

	EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFLCharacterPlayer::Move);
	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFLCharacterPlayer::Look);
	EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &AFLCharacterPlayer::DoJumpStart);
	EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFLCharacterPlayer::DoJumpEnd);
	EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &AFLCharacterPlayer::Attack);
}

void AFLCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	AFLPlayerController* PC = Cast<AFLPlayerController>(GetController());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is null!"));
		return;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("LocalPlayer is null!"));
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
	{
		Subsystem->AddMappingContext(IMC, 0);
	}
}

void AFLCharacterPlayer::Look(const FInputActionValue& Value)
{
	FVector2D Axis = Value.Get<FVector2D>();

	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AFLCharacterPlayer::Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Axis.Y);
	AddMovementInput(Right, Axis.X);

}

void AFLCharacterPlayer::DoJumpStart()
{
	Jump();

	UE_LOG(LogTemp, Warning, TEXT("Jump!"));
}

void AFLCharacterPlayer::DoJumpEnd()
{
	StopJumping();

	UE_LOG(LogTemp, Warning, TEXT("Jump End!"));
}

void AFLCharacterPlayer::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack!"));

	if (!AttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackMontage is null!"));
		return;
	}

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		AnimInst->Montage_Play(AttackMontage);
	}
}