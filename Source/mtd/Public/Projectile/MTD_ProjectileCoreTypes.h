#pragma once

#include "mtd.h"

#include "MTD_ProjectileCoreTypes.generated.h"

class AMTD_Projectile;
class UMTD_GameplayEffect;

USTRUCT(BlueprintType)
struct FMTD_ProjectileParameters
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    FCollisionProfileName CollisionProfileName = FName("NoCollision");

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    bool bIsRadial = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,
        meta=(EditCondition="bIsRadial"))
    float RadialDamageRadius = 100.f;
};

/**
 * Data asset used to define a Projectile.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_ProjectileData
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Projectile class the tower will be spawning on fire. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AMTD_Projectile> ProjectileClass = nullptr;

    /** Parameters the spawned projectiles will be granted. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FMTD_ProjectileParameters ProjectileParameters;

    /** Gameplay effect classes to grant on projectile hit. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TArray<TSubclassOf<UMTD_GameplayEffect>> GameplayEffectsToGrantClasses;
};
