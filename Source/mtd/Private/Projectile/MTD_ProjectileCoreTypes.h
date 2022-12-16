#pragma once

#include "mtd.h"

#include "MTD_ProjectileCoreTypes.generated.h"

class UMTD_GameplayEffect;
class AMTD_Projectile;

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
};

UCLASS(BlueprintType, Const, meta=(ShortTooltip="Data asset used to define a Projectile."))
class MTD_API UMTD_ProjectileData : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Projectile class the tower will be spawning on fire. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AMTD_Projectile> ProjectileClass = nullptr;
    
    /** Parameters the spawned projectiles will be granted. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FMTD_ProjectileParameters ProjectileParameters;

    /** Damage gameplay effect to apply on projectile hit. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TSubclassOf<UMTD_GameplayEffect> DamageGameplayEffectClass = nullptr;

    /** Gameplay effect classes to grant on projectile hit. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TArray<TSubclassOf<UMTD_GameplayEffect>> GameplayEffectsToGrantClasses;
};
