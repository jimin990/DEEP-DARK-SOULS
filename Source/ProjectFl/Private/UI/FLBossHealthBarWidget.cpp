// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FLBossHealthBarWidget.h"

#include "UI/FLBossHealthBarWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UFLBossHealthBarWidget::SetBossName(const FText& InName)
{
    if (Text_BossName)
    {
        Text_BossName->SetText(InName);
    }
}

void UFLBossHealthBarWidget::SetHealthPercent(float Percent)
{
    if (ProgressBar_BossHealth)
    {
        ProgressBar_BossHealth->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
    }
}