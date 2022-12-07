#pragma once

#include "GameplayEffect.h"
#include "mtd.h"

#include "MTD_ProjectileCoreTypes.generated.h"

USTRUCT(BlueprintType)
struct FMTD_ProjectileMovementParameters
{
    GENERATED_BODY()

public:
    /// The speed the projectile will start at.
    UPROPERTY(BlueprintReadWrite)
    float InitialSpeed = 0.f;

    /// The max speed the projectile will be travelling at.
    UPROPERTY(BlueprintReadWrite)
    float MaxSpeed = 0.f;

    /// The speed the projectile may accelerate itself each second by.
    UPROPERTY(BlueprintReadWrite)
    float Acceleration = 0.f;

    /// If set, HomingTarget must be specified. Rotation rate should be specified as well. Direction field will not be
    /// taken into account.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bIsHoming = false;

    /// Degrees the homing projectile will be able to rotate by per second.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.0"))
    float RotationRate = 0.f;

    /// If bIsHoming is set, the projectile will move towards the target. If the target dies - the projectile will
    /// follow the direction it was following just before the death.
    UPROPERTY(BlueprintReadWrite)
    TWeakObjectPtr<AActor> HomingTarget = nullptr;

    /// If bIsHoming is not set, this will be the direction the projectile will follow until destructuion.
    UPROPERTY(BlueprintReadWrite)
    FVector Direction = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FMTD_ProjectileParameters
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    FCollisionProfileName CollisionProfileName{FName(TEXT("NoCollision"))};

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool bIsRadial = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        meta=(EditCondition="bIsRadial"))
    float RadialDamageRadius = 100.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TArray<FGameplayEffectSpecHandle> GameplayEffectsToGrantOnHit;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FMTD_ProjectileMovementParameters MovementParameters;
};
