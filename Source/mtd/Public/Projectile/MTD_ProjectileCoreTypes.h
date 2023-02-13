#pragma once

#include "mtd.h"

#include "MTD_ProjectileCoreTypes.generated.h"

class AMTD_Projectile;
class UMTD_GameplayEffect;

/**
 * Data asset used to define a projectile.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_ProjectileData
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Projectile class to spawn on fire. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AMTD_Projectile> ProjectileClass = nullptr;

    /** Gameplay effect classes to grant on projectile hit. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TArray<TSubclassOf<UMTD_GameplayEffect>> GameplayEffectsToGrantClasses;
};
