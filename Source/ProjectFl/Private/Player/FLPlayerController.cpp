// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FLPlayerController.h"
#include "UI/FLPlayerHUDWidget.h"
#include "Characters/FLCharacterBase.h"
#include "UI/FLBossHealthBarWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/FLAttributeSet.h"
#include "UI/FLInventoryWidget.h"
#include "Player/FLPlayerState.h"
#include "Weapons/FLInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

void AFLPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UFLPlayerHUDWidget>(this, HUDWidgetClass);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();

			AFLCharacterBase* FLCharacter = Cast<AFLCharacterBase>(GetPawn());
			if (FLCharacter)
			{
				HUDWidget->InitAbilitySystem(FLCharacter->GetAbilitySystemComponent());
			}
		}
	}

    AFLPlayerState* FLPlayerState = GetPlayerState<AFLPlayerState>();

    if (FLPlayerState)
    {
        if (UFLInventoryComponent* InventoryComponent = FLPlayerState->GetInventoryComponent())
        {
            HUDWidget->InitInventory(InventoryComponent);
        }
    }
}

void AFLPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AFLCharacterBase* FLCharacter = Cast<AFLCharacterBase>(InPawn);

	if (HUDWidget && FLCharacter)
	{
		HUDWidget->InitAbilitySystem(FLCharacter->GetAbilitySystemComponent());
	}

    AFLPlayerState* FLPlayerState = GetPlayerState<AFLPlayerState>();

    if (HUDWidget && FLPlayerState)
    {
        if (UFLInventoryComponent* InventoryComponent = FLPlayerState->GetInventoryComponent())
        {
            HUDWidget->InitInventory(InventoryComponent);
        }
    }
}

void AFLPlayerController::ShowBossHealthBar(AActor* BossActor)
{
    if (!BossActor)
    {
        return;
    }

    UAbilitySystemComponent* BossASC =
        UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(BossActor);

    if (!BossASC)
    {
        return;
    }

    if (!BossHealthBarWidget)
    {
        if (!BossHealthBarWidgetClass)
        {
            return;
        }

        BossHealthBarWidget =
            CreateWidget<UFLBossHealthBarWidget>(
                this,
                BossHealthBarWidgetClass
            );

        if (BossHealthBarWidget)
        {
            BossHealthBarWidget->AddToViewport();
        }
    }

    if (BossHealthBarWidget)
    {
        BossHealthBarWidget->SetVisibility(ESlateVisibility::Visible);

        // 이후 보스 이름 값 추가
        //BossHealthBarWidget->SetBossName(FText::FromString(BossActor->GetName()));
    }

    UnbindBossAttributes();

    CurrentBossASC = BossASC;

    BindBossAttributes(CurrentBossASC);

    UpdateBossHealthBar();
}

void AFLPlayerController::HideBossHealthBar()
{
    UnbindBossAttributes();

    CurrentBossASC = nullptr;

    if (BossHealthBarWidget)
    {
        BossHealthBarWidget->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void AFLPlayerController::BindBossAttributes(UAbilitySystemComponent* BossASC)
{
    if (!BossASC)
    {
        return;
    }

    BossHealthChangedHandle =
        BossASC->GetGameplayAttributeValueChangeDelegate(
            UFLAttributeSet::GetHealthAttribute()
        ).AddUObject(
            this,
            &AFLPlayerController::OnBossHealthChanged
        );

    BossMaxHealthChangedHandle =
        BossASC->GetGameplayAttributeValueChangeDelegate(
            UFLAttributeSet::GetMaxHealthAttribute()
        ).AddUObject(
            this,
            &AFLPlayerController::OnBossMaxHealthChanged
        );
}

void AFLPlayerController::UnbindBossAttributes()
{
    if (!CurrentBossASC)
    {
        return;
    }

    CurrentBossASC->GetGameplayAttributeValueChangeDelegate(
        UFLAttributeSet::GetHealthAttribute()
    ).Remove(BossHealthChangedHandle);

    CurrentBossASC->GetGameplayAttributeValueChangeDelegate(
        UFLAttributeSet::GetMaxHealthAttribute()
    ).Remove(BossMaxHealthChangedHandle);

    BossHealthChangedHandle.Reset();
    BossMaxHealthChangedHandle.Reset();
}

void AFLPlayerController::OnBossHealthChanged(
    const FOnAttributeChangeData& Data
)
{
    UpdateBossHealthBar();
}

void AFLPlayerController::OnBossMaxHealthChanged(
    const FOnAttributeChangeData& Data
)
{
    UpdateBossHealthBar();
}

void AFLPlayerController::UpdateBossHealthBar()
{
    if (!CurrentBossASC || !BossHealthBarWidget)
    {
        return;
    }

    const float Health =
        CurrentBossASC->GetNumericAttribute(
            UFLAttributeSet::GetHealthAttribute()
        );

    const float MaxHealth =
        CurrentBossASC->GetNumericAttribute(
            UFLAttributeSet::GetMaxHealthAttribute()
        );

    const float Percent =
        MaxHealth > 0.f ? Health / MaxHealth : 0.f;

    BossHealthBarWidget->SetHealthPercent(Percent);
}

void AFLPlayerController::ToggleInventory()
{
    if (!InventoryWidget)
    {
        if (!InventoryWidgetClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("InventoryWidgetClass is null"));
            return;
        }

        InventoryWidget = CreateWidget<UFLInventoryWidget>(
            this,
            InventoryWidgetClass
        );

        if (!InventoryWidget)
        {
            return;
        }

        AFLPlayerState* FLPlayerState = GetPlayerState<AFLPlayerState>();

        if (!FLPlayerState)
        {
            return;
        }

        UFLInventoryComponent* InventoryComponent =
            FLPlayerState->GetInventoryComponent();

        if (!InventoryComponent)
        {
            return;
        }

        InventoryWidget->InitInventory(InventoryComponent);
        InventoryWidget->AddToViewport();
        InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
    }

    const bool bIsVisible =
        InventoryWidget->GetVisibility() == ESlateVisibility::Visible;

    if (bIsVisible)
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
    }
    else
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Visible);

        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        SetInputMode(InputMode);
        bShowMouseCursor = true;
    }
}

void AFLPlayerController::PlayBossBGM(
    USoundBase* InBossBGM,
    float FadeInTime,
    float Volume
)
{
    if (!InBossBGM)
    {
        return;
    }

    if (BossBGMComponent)
    {
        return;
    }

    BossBGMComponent = UGameplayStatics::SpawnSound2D(
        this,
        InBossBGM,
        Volume
    );

    if (BossBGMComponent)
    {
        BossBGMComponent->FadeIn(FadeInTime, Volume);
    }
}

void AFLPlayerController::StopBossBGM(float FadeOutTime)
{
    if (!BossBGMComponent)
    {
        return;
    }

    BossBGMComponent->FadeOut(FadeOutTime, 0.f);
    BossBGMComponent = nullptr;
}

void AFLPlayerController::ShowGameEndWidget()
{
    FTimerHandle GameEndTimerHandle;

    GetWorld()->GetTimerManager().SetTimer(
        GameEndTimerHandle,
        this,
        &AFLPlayerController::DelayShowGameEndWidget,
        3.0f,
        false
    );
}

void AFLPlayerController::DelayShowGameEndWidget()
{
    if (!GameEndWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GameEndWidgetClass is null"));
        return;
    }

    if (!GameEndWidget)
    {
        GameEndWidget = CreateWidget<UUserWidget>(this, GameEndWidgetClass);
    }

    if (GameEndWidget && !GameEndWidget->IsInViewport())
    {
        GameEndWidget->AddToViewport(100);
    }

    SetPause(true);

    FInputModeUIOnly InputMode;
    InputMode.SetWidgetToFocus(GameEndWidget->TakeWidget());
    SetInputMode(InputMode);

    bShowMouseCursor = true;
}
