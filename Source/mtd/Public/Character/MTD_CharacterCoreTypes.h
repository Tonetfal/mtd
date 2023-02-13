#pragma once

#include "GameplayTagContainer.h"
#include "mtd.h"
#include "Projectile/MTD_ProjectileCoreTypes.h"

#include "MTD_CharacterCoreTypes.generated.h"

class AMTD_ManaToken;
class UInputMappingContext;
class UMTD_AbilitySet;
class UMTD_InputConfig;
class UMTD_ProjectileData;

/** Shorthand for table evaluations. Return if an attribute couldn't be found, as well as warning about the fail. */
#define EVALUTE_ATTRIBUTE(ATTRIBUTE_TABLE, ROW_NAME, IN_XY, OUT_XY) \
    do \
    { \
        TEnumAsByte<EEvaluateCurveTableResult::Type> Result; \
        const FString ContextString; \
        \
        UDataTableFunctionLibrary::EvaluateCurveTableRow( \
            ATTRIBUTE_TABLE, ROW_NAME, IN_XY, Result, OUT_XY, ContextString); \
        \
        if (Result != EEvaluateCurveTableResult::RowFound) \
        { \
            MTDS_WARN("Attribute [%s] on Owner's [%s] Attribute Table could not be found.", \
                *ROW_NAME.ToString(), *GetName()); \
            return; \
        } \
    } while(0)

/** Attribute names used to retrieve data from curve tables. */
const FName HealthAttributeName = "TotalHealth";
const FName HealthDeltaAttributeName = "DeltaHealth";
const FName ManaAttributeName = "TotalMana";
const FName ManaDeltaAttributeName = "DeltaMana";
const FName BalanceDamageAttributeName = "BalanceDamage";
const FName BalanceResistAttributeName = "BalanceResist";

const FName DamageAttributeName = "Damage";
const FName RangeAttributeName = "Range";
const FName VisionDegreesAttributeName = "VisionDegrees";
const FName FirerateAttributeName = "Firerate";
const FName ProjectileSpeedAttributeName = "ProjectileSpeed";

const FName HealthScaleAttributeName = "HealthScale";
const FName DamageScaleScaleAttributeName = "DamageScale";
const FName SpeedScaleScaleAttributeName = "SpeedScale";
const FName ExpAttributeName = "Exp";

/**
 * Data asset used to define a pawn.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_PawnData
    : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** Ability sets to grant to this pawn's ability system. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Abilities")
    TArray<UMTD_AbilitySet *> AbilitySets;
};

/**
 * Data asset used to define a player.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_PlayerData
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Values to initialize player base attributes with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UCurveTable> AttributeTable = nullptr;

    /** Input configuration used by players to create input mappings, and bind input actions. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UMTD_InputConfig> InputConfig = nullptr;

    /** Input mapping context to bind input actions with actual keys. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TObjectPtr<const UInputMappingContext>> InputContexts;

    /** Hero classes a character is associated with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTagContainer HeroClasses;
};

/**
 * Data asset used to define a tower.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_TowerData
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Values to initialize tower base attributes with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UCurveTable> AttributeTable = nullptr;

    /** Projectile related data to describe projectiles tower will fire. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UMTD_ProjectileData> ProjectileData = nullptr;
};

/**
 * Data asset used to define a foe.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_FoeData
    : public UDataAsset
{
    GENERATED_BODY()
    
public:
    /** Amount of base health a foe has. The value is scaled depending on difficulty. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Health = 0.f;
    
    /** Amount of base damage a foe has. The value is scaled depending on difficulty. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Damage = 0.f;
    
    /** Amount of base speed a foe has. The value is scaled depending on difficulty. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Speed = 0.f;
    
    /** Experience a foe gives when killed. The value is scaled depending on difficulty. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Experience = 0.f;
    
    /** Mana a foe drops on death. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<TSubclassOf<AMTD_ManaToken>, int32> ManaTokens;
    
    /** Amount of base balance damage a foe has. Does not scale with difficulty. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BalanceDamage = 0.f;
    
    /** Amount of minimal balance damage to knockback. Does not scale with difficulty. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BalanceThreshold = 0.f;
    
    /** Ignored balance damage percentage. The values should be in range of [0, 100]. Does not scale with difficulty. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BalanceResist = 0.f;

    /** Time in seconds a foe is knockback for when loses balance. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float KnockbackTime = 0.f;
};
