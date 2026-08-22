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

UFLGameplayAbility_Attack::UFLGameplayAbility_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFLGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogTemp, Warning, TEXT("GA_Attack Activated"));

	if (!AttackMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("AttackMontage is null"));
		return;
	}

	// 이벤트 등록
	const FGameplayTag AttackTraceEventTag =
		FGameplayTag::RequestGameplayTag(TEXT("Event.Attack.Trace"));

	TraceEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		AttackTraceEventTag
	);

	if (TraceEventTask)
	{
		TraceEventTask->EventReceived.AddDynamic(
			this,
			&UFLGameplayAbility_Attack::AttackTrace
		);

		TraceEventTask->ReadyForActivation();
	}

	// 이렇게 몽타주가 있는 부분은 결합을 낮출수 있는 방법 있을까 고민해보기
	AFLCharacterBase* FLPlayer = Cast<AFLCharacterBase>(ActorInfo->AvatarActor.Get());

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		AttackMontage,
		1.f
	);

	if (!MontageTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// 몽타주가 끝나면 동작하는 함수 바인딩
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
}

void UFLGameplayAbility_Attack::AttackTrace(FGameplayEventData Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("Trace!!"));

	AFLCharacterBase* FLCharacter =
		Cast<AFLCharacterBase>(GetAvatarActorFromActorInfo());

	FVector Start = FLCharacter->WeaponMeshComponent->GetSocketLocation(FLCharacter->DefaultTraceStartSocketName);
	FVector End = FLCharacter->WeaponMeshComponent->GetSocketLocation(FLCharacter->DefaultTraceEndSocketName);
	float Radius = FLCharacter->DefaultTraceRadius;

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(FLCharacter);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

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
		ApplyDamageEffectToTarget(TargetActor);
	}
}

void UFLGameplayAbility_Attack::OnAttackMontageCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("Trace Completed!!"));

	HitActors.Reset();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UFLGameplayAbility_Attack::OnAttackMontageInterrupted()
{
	UE_LOG(LogTemp, Warning, TEXT("Trace Interrupted!!"));

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
	if (!TargetActor || !DamageEffectClass)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC =
		GetAbilitySystemComponentFromActorInfo();

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!SourceASC || !TargetASC)
	{
		return;
	}

	FGameplayEffectContextHandle ContextHandle =
		SourceASC->MakeEffectContext();

	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle =
		SourceASC->MakeOutgoingSpec(
			DamageEffectClass,
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
