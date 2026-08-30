// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "FLGameplayCueNotify_Static.generated.h"

class UNiagaraSystem;
class USoundBase;

UCLASS()
class PROJECTFL_API UFLGameplayCueNotify_Static : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
    virtual bool OnExecute_Implementation(
        AActor* MyTarget,
        const FGameplayCueParameters& Parameters
    ) const override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword Hit")
    TObjectPtr<UNiagaraSystem> HitEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sword Hit")
    TObjectPtr<USoundBase> HitSound;
};
