// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FLGameplayAbility_Attack.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GAS/FLAttributeSet.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Characters/FLCharacterBase.h"
#include "GameplayTagContainer.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Combat/FLCombatComponent.h"
#include "Weapons/FLWeaponDataAsset.h"

UFLGameplayAbility_Attack::UFLGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("GA_Attack Activated"));

	// 현재 콤보 및 시작 콤보
	ComboIndex = 0;

	//현재 콤보를 받을 수 있는 상태인지
	bCanReceiveComboInput = false;

	// 현재 콤보 입력 시간에 입력이 들어왔는지 확인
	bComboInputBuffered = false;
	
	// Ability가 활성화 되면 다음 입력을 받을 준비
	ComboInputTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.Input"))
	);

	// 입력이 들어오면 동작할 함수 바인딩
	ComboInputTask->EventReceived.AddDynamic(
		this,
		&UFLGameplayAbility_Attack::OnComboInput
	);

	ComboInputTask->ReadyForActivation();

	// 몽타주 도중 콤보를 허용할 영역을 노티파이에서 받을 준비
	ComboOpenTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.Combo.Open"))
	);

	// 콤보 허용 노티파이를 받으면 동작할 함수 바인딩
	ComboOpenTask->EventReceived.AddDynamic(
		this,
		&UFLGameplayAbility_Attack::OnComboWindowOpen
	);

	ComboOpenTask->ReadyForActivation();

	// 콤보를 더 이상 받지 않을 영역을 노티파이에서 받을 준비
	ComboCloseTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.Combo.Close"))
	);

	// 콤버 비허용 태그가 날아오면 동작할 함수 바인딩
	ComboCloseTask->EventReceived.AddDynamic(
		this,
		&UFLGameplayAbility_Attack::OnComboWindowClose
	);

	ComboCloseTask->ReadyForActivation();

	// Trace
	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(ActorInfo->AvatarActor.Get());

	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UFLCombatComponent* CombatComponent = Character->GetCombatComponent();

	if (!CombatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Attack failed: CombatComponent is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UFLWeaponDataAsset* CurrentWeaponData = CombatComponent->GetCurrentWeaponData();

	if (!CurrentWeaponData || !CurrentWeaponData->ComboAttacks[ComboIndex].AttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Attack failed: CurrentWeaponData or Montage is null"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 이벤트 등록
	TraceEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		CurrentWeaponData->ComboAttacks[ComboIndex].AttackTraceEventTag
	);

	if (TraceEventTask)
	{
		TraceEventTask->EventReceived.AddDynamic(
			this,
			&UFLGameplayAbility_Attack::AttackTrace
		);

		TraceEventTask->ReadyForActivation();
	}

	PlayComboMontage();
}

void UFLGameplayAbility_Attack::AttackTrace(FGameplayEventData Payload)
{
	//UE_LOG(LogTemp, Log, TEXT("Trace"));

	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is null"));
		return;
	}

	UFLCombatComponent* CombatComponent = Character->GetCombatComponent();

	if (!CombatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("CombatComponent is null"));
		return;
	}

	FVector Start;
	FVector End;
	float Radius = 0.f;

	if (!CombatComponent->GetAttackTraceInfo(Start, End, Radius,ComboIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("GA_Attack Trace failed"));
		return;
	}

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	//UE_LOG(LogTemp, Warning, TEXT("Start:%s, End: %s, Radius: %f"),*Start.ToString(), *End.ToString(), Radius);

	DrawDebugSphere(GetWorld(), Start, Radius, 12, FColor::Green, false, 1.f);
	DrawDebugSphere(GetWorld(), End, Radius, 12, FColor::Red, false, 1.f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 1.f, 0, 2.f);

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* TargetActor = Hit.GetActor();

		if (!TargetActor || HitActors.Contains(TargetActor))
		{
			continue;
		}

		HitActors.Add(TargetActor);

		UE_LOG(LogTemp, Warning, TEXT("%s is Attacked"), *TargetActor->GetName());
		ApplyDamageEffectToTarget(TargetActor);
	}
}

void UFLGameplayAbility_Attack::OnAttackMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Trace Completed!!"));

	ComboIndex = 0;
	bCanReceiveComboInput = false;
	HitActors.Reset();

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UFLGameplayAbility_Attack::OnAttackMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("Trace Interrupted!!"));

	if (bChangingComboMontage)
	{
		bChangingComboMontage = false;
		return;
	}

	HitActors.Reset();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFLGameplayAbility_Attack::OnAttackMontageCancelled()
{
	UE_LOG(LogTemp, Warning, TEXT("Trace Cancelled!!"));

	HitActors.Reset();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UFLGameplayAbility_Attack::ApplyDamageEffectToTarget(AActor* TargetActor)
{
	
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor is null!!"));
		return;
	}

	UAbilitySystemComponent* SourceASC =
		GetAbilitySystemComponentFromActorInfo();

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!SourceASC || !TargetASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("SourceASC || TargetASC is null!!"));
		return;
	}

	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character is null!!"));
		return;
	}

	UFLCombatComponent* CombatComponent = Character->GetCombatComponent();

	if (!CombatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("CombatComponent is null!!"));
		return;
	}

	FGameplayEffectContextHandle ContextHandle =
		SourceASC->MakeEffectContext();

	ContextHandle.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> EffectClass : CombatComponent->WeaponData->ComboAttacks[ComboIndex].TargetEffects)
	{
		if (!EffectClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("EffectClass is null!!"));
			continue;
		}

		FGameplayEffectSpecHandle SpecHandle =
			SourceASC->MakeOutgoingSpec(
				EffectClass,
				GetAbilityLevel(),
				ContextHandle
			);

		// 확인 로그
		if (SpecHandle.IsValid())
		{
			SourceASC->ApplyGameplayEffectSpecToTarget(
				*SpecHandle.Data.Get(),
				TargetASC
			);

			const UFLAttributeSet* TargetAttributeSet =
				TargetASC->GetSet<UFLAttributeSet>();

			if (TargetAttributeSet)
			{
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("Target: %s / Health: %f / MaxHealth: %f"),
					*TargetActor->GetName(),
					TargetAttributeSet->GetHealth(),
					TargetAttributeSet->GetMaxHealth()
				);
			}
			else
			{
				UE_LOG(
					LogTemp,
					Warning,
					TEXT("Target: %s / AttributeSet is null"),
					*TargetActor->GetName()
				);
			}
		}
	}

	// 히트 리액트 수정 필요
	if (CombatComponent->WeaponData->ComboAttacks[ComboIndex].HitReactEventTag.IsValid())
	{
		FGameplayEventData EventData;
		EventData.EventTag = CombatComponent->WeaponData->ComboAttacks[ComboIndex].HitReactEventTag;
		EventData.Instigator = GetAvatarActorFromActorInfo();
		EventData.Target = TargetActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			TargetActor,
			CombatComponent->WeaponData->ComboAttacks[ComboIndex].HitReactEventTag,
			EventData
		);
	}
}

void UFLGameplayAbility_Attack::PlayComboMontage()
{
	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UFLCombatComponent* CombatComponent = Character->GetCombatComponent();

	if (!CombatComponent || !CombatComponent->GetCurrentWeaponData())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	UFLWeaponDataAsset* WeaponData = CombatComponent->GetCurrentWeaponData();

	if (!WeaponData->ComboAttacks.IsValidIndex(ComboIndex))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}

	const FFLWeaponAttackData& AttackData = WeaponData->ComboAttacks[ComboIndex];

	if (!AttackData.AttackMontage)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	HitActors.Reset();
	bCanReceiveComboInput = false;
	bComboInputBuffered = false;

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		AttackData.AttackMontage,
		1.f
	);

	MontageTask->OnCompleted.AddDynamic(this, &UFLGameplayAbility_Attack::OnAttackMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UFLGameplayAbility_Attack::OnAttackMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UFLGameplayAbility_Attack::OnAttackMontageCancelled);

	MontageTask->ReadyForActivation();
}

void UFLGameplayAbility_Attack::OnComboInput(FGameplayEventData Payload)
{
	if (!bCanReceiveComboInput)
	{
		return;
	}

	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		return;
	}

	UFLCombatComponent* CombatComponent = Character->GetCombatComponent();
	UFLWeaponDataAsset* WeaponData =
		CombatComponent ? CombatComponent->GetCurrentWeaponData() : nullptr;

	if (!WeaponData)
	{
		return;
	}

	const int32 NextComboIndex = ComboIndex + 1;

	if (!WeaponData->ComboAttacks.IsValidIndex(NextComboIndex))
	{
		bCanReceiveComboInput = false;
		return;
	}

	// 다음 콤보 방향 적용
	ApplyAttackDirection(Character);

	bCanReceiveComboInput = false;
	bComboInputBuffered = false;
	bChangingComboMontage = true;

	ComboIndex = NextComboIndex;

	PlayComboMontage();
}

void UFLGameplayAbility_Attack::OnComboWindowOpen(FGameplayEventData Payload)
{
	bCanReceiveComboInput = true;
}

void UFLGameplayAbility_Attack::OnComboWindowClose(FGameplayEventData Payload)
{
	bCanReceiveComboInput = false;
}

void UFLGameplayAbility_Attack::ApplyAttackDirection(AFLCharacterBase* InCharacter)
{
	if (!InCharacter)
	{
		return;
	}

	FVector Direction = InCharacter->GetLastMoveInputWorldDirection();

	if (Direction.IsNearlyZero())
	{
		return;
	}

	Direction.Z = 0.f;
	Direction.Normalize();

	const FRotator TargetRotation = Direction.Rotation();

	InCharacter->SetActorRotation(FRotator(
		0.f,
		TargetRotation.Yaw,
		0.f
	));
}