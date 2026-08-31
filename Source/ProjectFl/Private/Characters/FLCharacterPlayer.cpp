// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FLCharacterPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/FLPlayerController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Input/FLInputConfigDataAsset.h"

AFLCharacterPlayer::AFLCharacterPlayer()
{
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

	if (!IMC || !MoveAction || !LookAction || !DodgeAction || !AttackAction)
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
	EIC->BindAction(DodgeAction, ETriggerEvent::Started, this, &AFLCharacterPlayer::Dodge);
	EIC->BindAction(AttackAction, ETriggerEvent::Started, this, &AFLCharacterPlayer::Attack);
	EIC->BindAction(HealAction, ETriggerEvent::Started, this, &AFLCharacterPlayer::Heal);

	// InputConfig 를 읽고 태그를 인자로 사용하는 함수로 바인딩
	/*
	if (InputConfig)
	{
		for (const FFLInputActionTag& InputActionTag : InputConfig->AbilityInputActions)
		{
			if (!InputActionTag.InputAction || !InputActionTag.InputTag.IsValid())
			{
				continue;
			}

			EIC->BindAction(
				InputActionTag.InputAction,
				ETriggerEvent::Started,
				this,
				&AFLCharacterPlayer::AbilityInputPressed,
				InputActionTag.InputTag
			);
		}
	}
	*/
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

	const FVector MoveDirection =
		(Forward * Axis.Y + Right * Axis.X).GetSafeNormal();

	if (!MoveDirection.IsNearlyZero())
	{
		LastMoveInputWorldDirection = MoveDirection;
	}

	AddMovementInput(Forward, Axis.Y);
	AddMovementInput(Right, Axis.X);

}

void AFLCharacterPlayer::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack!"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !AttackInputTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC or InputTag is invalid"));
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.DynamicAbilityTags.HasTagExact(AttackInputTag))
		{
			continue;
		}

		const bool bActivated = ASC->TryActivateAbility(Spec.Handle);

		if (!bActivated)
		{
			const FGameplayTag ComboInputTag =
				FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.Input"));

			FGameplayEventData EventData;
			EventData.EventTag = ComboInputTag;
			EventData.Instigator = this;
			EventData.Target = this;

			const int32 ActivatedCount =
				ASC->HandleGameplayEvent(ComboInputTag, &EventData);

			UE_LOG(
				LogTemp,
				Warning,
				TEXT("Combo Input Event Sent / Count: %d"),
				ActivatedCount
			);
		}

		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Attack Ability not found by InputTag: %s"),
		*AttackInputTag.ToString()
	);
}

void AFLCharacterPlayer::Dodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge!"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !DodgeInputTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC or InputTag is invalid"));
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.DynamicAbilityTags.HasTagExact(DodgeInputTag))
		{
			continue;
		}

		const bool bActivated = ASC->TryActivateAbility(Spec.Handle);

		return;
	}
}

void AFLCharacterPlayer::Heal()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge!"));

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !HealInputTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("ASC or InputTag is invalid"));
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.DynamicAbilityTags.HasTagExact(HealInputTag))
		{
			continue;
		}

		const bool bActivated = ASC->TryActivateAbility(Spec.Handle);

		return;
	}
}

// 인풋 액션이랑 연결 만드는 중 
void AFLCharacterPlayer::AbilityInputPressed(FGameplayTag InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (!Spec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			continue;
		}

		const bool bActivated = ASC->TryActivateAbility(Spec.Handle);

		if (!bActivated && InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(TEXT("Input.Attack"))))
		{
			const FGameplayTag ComboInputTag =
				FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.Input"));

			FGameplayEventData EventData;
			EventData.EventTag = ComboInputTag;
			EventData.Instigator = this;
			EventData.Target = this;

			ASC->HandleGameplayEvent(ComboInputTag, &EventData);
		}

		return;
	}
}
