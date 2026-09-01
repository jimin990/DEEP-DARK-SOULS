// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FLBossHealthBarWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class PROJECTFL_API UFLBossHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetBossName(const FText& InName);
    void SetHealthPercent(float Percent);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> ProgressBar_BossHealth;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> Text_BossName;
};