#include "GAS/FLGameplayAbility_Attack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Characters/FLCharacterBase.h"
#include "Combat/FLCombatComponent.h"
#include "DrawDebugHelpers.h"
#include "GAS/FLAttributeSet.h"
#include "GameplayEffect.h"
#include "Perception/AISense_Damage.h"
#include "Weapons/FLWeaponDataAsset.h"

UFLGameplayAbility_Attack::UFLGameplayAbility_Attack()
{
	InstancingPolicy =
		EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGameplayAbility_Attack::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		TriggerEventData
	);


	UE_LOG(
		LogTemp,
		Warning,
		TEXT("GA_Attack 1")
	);

	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UFLCombatComponent* CombatComponent =
		Character->GetCombatComponent();

	UFLWeaponDataAsset* WeaponData =
		CombatComponent
		? CombatComponent->GetCurrentWeaponData()
		: nullptr;

	if (!WeaponData ||
		!WeaponData->ComboAttacks.IsValidIndex(0) ||
		!WeaponData->ComboAttacks[0].AttackMontage)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("GA_Attack failed: invalid weapon or combo data")
		);

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("GA_Attack failed: CommitAbility")
		);

		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("GA_Attack Activated"));

	ComboIndex = 0;
	bCanReceiveComboInput = false;
	HitActors.Reset();

	const FGameplayTag ComboInputTag =
		FGameplayTag::RequestGameplayTag(
			TEXT("Event.Attack.Input")
		);

	const FGameplayTag ComboOpenTag =
		FGameplayTag::RequestGameplayTag(
			TEXT("Event.Attack.Combo.Open")
		);

	const FGameplayTag ComboCloseTag =
		FGameplayTag::RequestGameplayTag(
			TEXT("Event.Attack.Combo.Close")
		);

	ComboInputTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			ComboInputTag
		);

	if (ComboInputTask)
	{
		ComboInputTask->EventReceived.AddDynamic(
			this,
			&UFLGameplayAbility_Attack::OnComboInput
		);

		ComboInputTask->ReadyForActivation();
	}

	ComboOpenTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			ComboOpenTag
		);

	if (ComboOpenTask)
	{
		ComboOpenTask->EventReceived.AddDynamic(
			this,
			&UFLGameplayAbility_Attack::OnComboWindowOpen
		);

		ComboOpenTask->ReadyForActivation();
	}

	ComboCloseTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			ComboCloseTag
		);

	if (ComboCloseTask)
	{
		ComboCloseTask->EventReceived.AddDynamic(
			this,
			&UFLGameplayAbility_Attack::OnComboWindowClose
		);

		ComboCloseTask->ReadyForActivation();
	}

	PlayComboMontage();
}

void UFLGameplayAbility_Attack::PlayComboMontage()
{
	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		EndAbility(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			true,
			true
		);
		return;
	}

	UFLCombatComponent* CombatComponent =
		Character->GetCombatComponent();

	UFLWeaponDataAsset* WeaponData =
		CombatComponent
		? CombatComponent->GetCurrentWeaponData()
		: nullptr;

	if (!WeaponData ||
		!WeaponData->ComboAttacks.IsValidIndex(ComboIndex))
	{
		EndAbility(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			true,
			true
		);
		return;
	}

	const FFLWeaponAttackData& AttackData =
		WeaponData->ComboAttacks[ComboIndex];

	if (!AttackData.AttackMontage)
	{
		EndAbility(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			true,
			true
		);
		return;
	}

	HitActors.Reset();
	bCanReceiveComboInput = false;

	SetupTraceEventTask(AttackData.AttackTraceEventTag);

	// 이전 콤보 Task가 새 몽타주 중단을
	// 실제 피격 중단으로 처리하지 못하도록 연결을 해제한다.
	UnbindMontageTaskDelegates();
	MontageTask = nullptr;

	MontageTask =
		UAbilityTask_PlayMontageAndWait::
		CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AttackData.AttackMontage,
			1.f
		);

	if (!MontageTask)
	{
		EndAbility(
			CurrentSpecHandle,
			CurrentActorInfo,
			CurrentActivationInfo,
			true,
			true
		);
		return;
	}

	MontageTask->OnCompleted.AddDynamic(
		this,
		&UFLGameplayAbility_Attack::OnAttackMontageCompleted
	);

	MontageTask->OnInterrupted.AddDynamic(
		this,
		&UFLGameplayAbility_Attack::OnAttackMontageInterrupted
	);

	MontageTask->OnCancelled.AddDynamic(
		this,
		&UFLGameplayAbility_Attack::OnAttackMontageCancelled
	);

	MontageTask->ReadyForActivation();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Play Combo Montage / Index: %d"),
		ComboIndex
	);
}

void UFLGameplayAbility_Attack::SetupTraceEventTask(
	const FGameplayTag& TraceEventTag
)
{
	if (TraceEventTask)
	{
		TraceEventTask->EndTask();
		TraceEventTask = nullptr;
	}

	if (!TraceEventTag.IsValid())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("AttackTraceEventTag is invalid")
		);
		return;
	}

	TraceEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			TraceEventTag
		);

	if (!TraceEventTask)
	{
		return;
	}

	TraceEventTask->EventReceived.AddDynamic(
		this,
		&UFLGameplayAbility_Attack::AttackTrace
	);

	TraceEventTask->ReadyForActivation();
}

void UFLGameplayAbility_Attack::UnbindMontageTaskDelegates()
{
	if (!MontageTask)
	{
		return;
	}

	MontageTask->OnCompleted.RemoveDynamic(
		this,
		&UFLGameplayAbility_Attack::OnAttackMontageCompleted
	);

	MontageTask->OnInterrupted.RemoveDynamic(
		this,
		&UFLGameplayAbility_Attack::OnAttackMontageInterrupted
	);

	MontageTask->OnCancelled.RemoveDynamic(
		this,
		&UFLGameplayAbility_Attack::OnAttackMontageCancelled
	);
}

void UFLGameplayAbility_Attack::OnComboInput(
	FGameplayEventData Payload
)
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

	UFLCombatComponent* CombatComponent =
		Character->GetCombatComponent();

	UFLWeaponDataAsset* WeaponData =
		CombatComponent
		? CombatComponent->GetCurrentWeaponData()
		: nullptr;

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

	// 플레이어만 입력 방향으로 콤보 방향을 보정한다.
	// AI는 AIController의 Focus 방향을 사용한다.
	if (Character->IsPlayerControlled())
	{
		ApplyAttackDirection(Character);
	}

	bCanReceiveComboInput = false;
	ComboIndex = NextComboIndex;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Combo changed: %d"),
		ComboIndex
	);

	PlayComboMontage();
}

void UFLGameplayAbility_Attack::OnComboWindowOpen(
	FGameplayEventData Payload
)
{
	bCanReceiveComboInput = true;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Combo Window Open / Index: %d"),
		ComboIndex
	);

	RequestNextComboForAI();
}

void UFLGameplayAbility_Attack::OnComboWindowClose(
	FGameplayEventData Payload
)
{
	bCanReceiveComboInput = false;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Combo Window Close / Index: %d"),
		ComboIndex
	);
}

void UFLGameplayAbility_Attack::RequestNextComboForAI()
{
	if (!bAutoContinueComboForAI)
	{
		return;
	}

	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character || Character->IsPlayerControlled())
	{
		return;
	}

	UFLCombatComponent* CombatComponent =
		Character->GetCombatComponent();

	UFLWeaponDataAsset* WeaponData =
		CombatComponent
		? CombatComponent->GetCurrentWeaponData()
		: nullptr;

	if (!WeaponData)
	{
		return;
	}

	const int32 NextComboIndex = ComboIndex + 1;

	if (!WeaponData->ComboAttacks.IsValidIndex(NextComboIndex))
	{
		return;
	}

	UAbilitySystemComponent* ASC =
		GetAbilitySystemComponentFromActorInfo();

	if (!ASC)
	{
		return;
	}

	const FGameplayTag ComboInputTag =
		FGameplayTag::RequestGameplayTag(
			TEXT("Event.Attack.Input")
		);

	if (!ComboInputTag.IsValid())
	{
		return;
	}

	FGameplayEventData EventData;
	EventData.EventTag = ComboInputTag;
	EventData.Instigator = Character;
	EventData.Target = Character;

	ASC->HandleGameplayEvent(
		ComboInputTag,
		&EventData
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("AI Combo Input Sent / CurrentIndex: %d"),
		ComboIndex
	);
}

void UFLGameplayAbility_Attack::AttackTrace(
	FGameplayEventData Payload
)
{
	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		return;
	}

	UFLCombatComponent* CombatComponent =
		Character->GetCombatComponent();

	if (!CombatComponent)
	{
		return;
	}

	FVector Start = FVector::ZeroVector;
	FVector End = FVector::ZeroVector;
	float Radius = 0.f;

	if (!CombatComponent->GetAttackTraceInfo(
		Start,
		End,
		Radius,
		ComboIndex
	))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("GA_Attack Trace failed")
		);
		return;
	}

	UWorld* World = Character->GetWorld();

	if (!World)
	{
		return;
	}

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

	/*DrawDebugSphere(
		World,
		Start,
		Radius,
		12,
		FColor::Green,
		false,
		1.f
	);

	DrawDebugSphere(
		World,
		End,
		Radius,
		12,
		FColor::Red,
		false,
		1.f
	);

	DrawDebugLine(
		World,
		Start,
		End,
		FColor::Yellow,
		false,
		1.f,
		0,
		2.f
	);*/

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* TargetActor = Hit.GetActor();

		if (!TargetActor ||
			TargetActor == Character ||
			HitActors.Contains(TargetActor))
		{
			continue;
		}

		UAbilitySystemComponent* TargetASC =
			UAbilitySystemBlueprintLibrary::
			GetAbilitySystemComponent(TargetActor);

		if (!TargetASC)
		{
			continue;
		}

		HitActors.Add(TargetActor);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("%s is Attacked / Combo: %d"),
			*TargetActor->GetName(),
			ComboIndex
		);

		ApplyDamageEffectToTarget(TargetActor);
	}
}

void UFLGameplayAbility_Attack::ApplyDamageEffectToTarget(
	AActor* TargetActor
)
{
	if (!TargetActor)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC =
		GetAbilitySystemComponentFromActorInfo();

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::
		GetAbilitySystemComponent(TargetActor);

	if (!SourceASC || !TargetASC)
	{
		return;
	}

	AFLCharacterBase* Character =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	if (!Character)
	{
		return;
	}

	UFLCombatComponent* CombatComponent =
		Character->GetCombatComponent();

	UFLWeaponDataAsset* WeaponData =
		CombatComponent
		? CombatComponent->GetCurrentWeaponData()
		: nullptr;

	if (!WeaponData ||
		!WeaponData->ComboAttacks.IsValidIndex(ComboIndex))
	{
		return;
	}

	const FFLWeaponAttackData& AttackData =
		WeaponData->ComboAttacks[ComboIndex];

	const FGameplayTag InvincibleTag =
		FGameplayTag::RequestGameplayTag(
			TEXT("State.Invincible"),
			false
		);

	if (InvincibleTag.IsValid() &&
		TargetASC->HasMatchingGameplayTag(InvincibleTag))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Target is invincible")
		);
		return;
	}

	FGameplayEffectContextHandle ContextHandle =
		SourceASC->MakeEffectContext();

	ContextHandle.AddSourceObject(this);
	ContextHandle.AddInstigator(Character, Character);

	for (const TSubclassOf<UGameplayEffect>& EffectClass :
		AttackData.TargetEffects)
	{
		if (!EffectClass)
		{
			continue;
		}

		FGameplayEffectSpecHandle SpecHandle =
			SourceASC->MakeOutgoingSpec(
				EffectClass,
				GetAbilityLevel(),
				ContextHandle
			);

		if (!SpecHandle.IsValid())
		{
			continue;
		}

		SourceASC->ApplyGameplayEffectSpecToTarget(
			*SpecHandle.Data.Get(),
			TargetASC
		);
	}

	const UFLAttributeSet* TargetAttributeSet =
		TargetASC->GetSet<UFLAttributeSet>();

	if (TargetAttributeSet)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Target: %s / Health: %.1f / MaxHealth: %.1f"),
			*TargetActor->GetName(),
			TargetAttributeSet->GetHealth(),
			TargetAttributeSet->GetMaxHealth()
		);
	}

	if (AttackData.HitReactEventTag.IsValid())
	{
		FGameplayEventData EventData;
		EventData.EventTag = AttackData.HitReactEventTag;
		EventData.Instigator = Character;
		EventData.Target = TargetActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			TargetActor,
			AttackData.HitReactEventTag,
			EventData
		);
	}

	ReportDamageToPerception(TargetActor, 1.f);
}

void UFLGameplayAbility_Attack::ApplyAttackDirection(
	AFLCharacterBase* InCharacter
)
{
	if (!InCharacter)
	{
		return;
	}

	FVector Direction =
		InCharacter->GetLastMoveInputWorldDirection();

	Direction.Z = 0.f;

	if (Direction.IsNearlyZero())
	{
		return;
	}

	Direction.Normalize();

	const FRotator TargetRotation = Direction.Rotation();

	InCharacter->SetActorRotation(
		FRotator(0.f, TargetRotation.Yaw, 0.f)
	);
}

void UFLGameplayAbility_Attack::OnAttackMontageCompleted()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Attack Montage Completed")
	);

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		false,
		false
	);
}

void UFLGameplayAbility_Attack::OnAttackMontageInterrupted()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Attack Montage Interrupted")
	);

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		true,
		true
	);
}

void UFLGameplayAbility_Attack::OnAttackMontageCancelled()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Attack Montage Cancelled")
	);

	EndAbility(
		CurrentSpecHandle,
		CurrentActorInfo,
		CurrentActivationInfo,
		true,
		true
	);
}

void UFLGameplayAbility_Attack::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("GA_Attack Ended / Cancelled: %s"),
		bWasCancelled ? TEXT("true") : TEXT("false")
	);

	HitActors.Reset();
	ComboIndex = 0;
	bCanReceiveComboInput = false;

	Super::EndAbility(
		Handle,
		ActorInfo,
		ActivationInfo,
		bReplicateEndAbility,
		bWasCancelled
	);

	MontageTask = nullptr;
	TraceEventTask = nullptr;
	ComboInputTask = nullptr;
	ComboOpenTask = nullptr;
	ComboCloseTask = nullptr;
}

void UFLGameplayAbility_Attack::ReportDamageToPerception(
	AActor* TargetActor,
	float DamageAmount
)
{
	if (!TargetActor || DamageAmount <= 0.f)
	{
		return;
	}

	AActor* InstigatorActor =
		GetAvatarActorFromActorInfo();

	if (!InstigatorActor)
	{
		return;
	}

	UWorld* World = TargetActor->GetWorld();

	if (!World)
	{
		return;
	}

	UAISense_Damage::ReportDamageEvent(
		World,
		TargetActor,
		InstigatorActor,
		DamageAmount,
		InstigatorActor->GetActorLocation(),
		TargetActor->GetActorLocation()
	);
}