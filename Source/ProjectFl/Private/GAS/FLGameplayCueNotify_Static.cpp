// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/FLGameplayCueNotify_Static.h"
#include "NiagaraFunctionLibrary.h"

bool UFLGameplayCueNotify_Static::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
    /*if (!MyTarget)
    {
        return false;
    }

    const FVector SpawnLocation =
        Parameters.Location.IsNearlyZero()
        ? MyTarget->GetActorLocation()
        : Parameters.Location;

    const FRotator SpawnRotation =
        Parameters.Normal.IsNearlyZero()
        ? FRotator::ZeroRotator
        : Parameters.Normal.Rotation();

    if (HitEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            MyTarget,
            HitEffect,
            SpawnLocation,
            SpawnRotation
        );
    }

    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            MyTarget,
            HitSound,
            SpawnLocation
        );
    }*/

    return true;
}
