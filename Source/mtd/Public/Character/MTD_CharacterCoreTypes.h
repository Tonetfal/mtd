#pragma once

#include "mtd.h"
#include "Projectile/MTD_ProjectileCoreTypes.h"

#include "MTD_CharacterCoreTypes.generated.h"

class UMTD_GameplayEffect;
class AMTD_Projectile;
class UMTD_InputConfig;

UCLASS(BlueprintType, meta=(ShortTooltip = "Data asset used to define a Player."))
class MTD_API UMTD_PlayerData : public UDataAsset
{
    GENERATED_BODY()

public:
    /// Values to initialize the player attributes with.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UCurveTable> AttributeTable = nullptr;

    /// Input configuration used players to create input mappings and bind input actions.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UMTD_InputConfig> InputConfig = nullptr;
};

UCLASS(BlueprintType, meta=(ShortTooltip = "Data asset used to define a Tower."))
class MTD_API UMTD_TowerData : public UDataAsset
{
    GENERATED_BODY()

public:
    /// Values to initialize the tower attributes with.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UCurveTable> AttributeTable = nullptr;

    /// Projectile class the tower will be spawning on fire.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AMTD_Projectile> ProjectileClass = nullptr;
    
    /// Projectile parameters the spawned projectiles will be granted.
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FMTD_ProjectileParameters ProjectileParameters;

    /// Projectile damage gameplay effect to apply on hit.
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TSubclassOf<UMTD_GameplayEffect> DamageGameplayEffectClass = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TArray<TSubclassOf<UMTD_GameplayEffect>> GameplayEffectsToGrantClasses;
};
