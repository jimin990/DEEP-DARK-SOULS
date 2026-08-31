#pragma once

#include "CoreMinimal.h"
#include "FLAIState.generated.h"

UENUM(BlueprintType)
enum class EFLAIState : uint8
{
    None UMETA(DisplayName = "None"),
    Patrol UMETA(DisplayName = "Patrol"),
    Chase UMETA(DisplayName = "Chase"),
    Combat UMETA(DisplayName = "Combat"),
    Dead UMETA(DisplayName = "Dead"),
    HitReact UMETA(DisplayName = "HiReact")
};