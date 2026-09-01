// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "FLEQSContext_Target.generated.h"

UCLASS()
class PROJECTFL_API UFLEQSContext_Target : public UEnvQueryContext
{
    GENERATED_BODY()

public:
    virtual void ProvideContext(
        FEnvQueryInstance& QueryInstance,
        FEnvQueryContextData& ContextData
    ) const override;
};