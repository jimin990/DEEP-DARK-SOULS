// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemComponent.h"
#include "FLPlayerController.generated.h"

class UFLPlayerHUDWidget;
class UFLBossHealthBarWidget;
class UAbilitySystemComponent;
class UFLAttributeSet;
class UFLInventoryWidget;
class UInputAction;

UCLASS()
class PROJECTFL_API AFLPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UFLPlayerHUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UFLPlayerHUDWidget> HUDWidget;

    // º¸½º UI
protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UFLBossHealthBarWidget> BossHealthBarWidgetClass;

    UPROPERTY()
    TObjectPtr<UFLBossHealthBarWidget> BossHealthBarWidget;

    UPROPERTY()
    TObjectPtr<UAbilitySystemComponent> CurrentBossASC;

    FDelegateHandle BossHealthChangedHandle;
    FDelegateHandle BossMaxHealthChangedHandle;

public:
    void ShowBossHealthBar(AActor* BossActor);
    void HideBossHealthBar();

private:
    void BindBossAttributes(UAbilitySystemComponent* BossASC);
    void UnbindBossAttributes();
    void UpdateBossHealthBar();

    void OnBossHealthChanged(const FOnAttributeChangeData& Data);
    void OnBossMaxHealthChanged(const FOnAttributeChangeData& Data);
	
public:
    void ToggleInventory();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UFLInventoryWidget> InventoryWidgetClass;

    UPROPERTY()
    TObjectPtr<UFLInventoryWidget> InventoryWidget;
};
