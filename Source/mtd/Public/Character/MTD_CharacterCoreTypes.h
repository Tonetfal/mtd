#pragma once

#include "mtd.h"
#include "Projectile/MTD_ProjectileCoreTypes.h"

#include "MTD_CharacterCoreTypes.generated.h"

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
const FName HealthAttributeName = "Health";
const FName ManaAttributeName = "Mana";
const FName BalanceResistAttributeName = "BalanceResist";
const FName BalanceDamageAttributeName = "BalanceDamage";

const FName DamageAttributeName = "Damage";
const FName RangeAttributeName = "Range";
const FName VisionDegreesAttributeName = "VisionDegrees";
const FName FirerateAttributeName = "Firerate";
const FName ProjectileSpeedAttributeName = "ProjectileSpeed";

const FName HealthScaleAttributeName = "HealthScale";
const FName DamageScaleScaleAttributeName = "DamageScale";
const FName SpeedScaleScaleAttributeName = "SpeedScale";
const FName ExpAttributeName = "Exp";

UCLASS(BlueprintType, Const, meta=(ShortTooltip="Data asset used to define a Player."))
class MTD_API UMTD_PlayerData : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Values to initialize the player attributes with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UCurveTable> AttributeTable = nullptr;

    /** Input configuration used by players to create input mappings and bind input actions. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UMTD_InputConfig> InputConfig = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TObjectPtr<const UInputMappingContext>> InputContexts;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTagContainer HeroClasses;
};

UCLASS(BlueprintType, Const, meta=(ShortTooltip="Data asset used to define a Tower."))
class MTD_API UMTD_TowerData : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Values to initialize the tower attributes with. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UCurveTable> AttributeTable = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UMTD_ProjectileData> ProjectileData = nullptr;
};

UCLASS(BlueprintType, Const, meta=(ShortTooltip="Data asset used to define a Pawn."))
class MTD_API UMTD_PawnData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /** Ability sets to grant to this pawn's ability system. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MTD|Abilities")
    TArray<UMTD_AbilitySet *> AbilitySets;
};

UCLASS(BlueprintType, Const, meta=(ShortTooltip="Data asset used to define an Enemy."))
class MTD_API UMTD_EnemyData : public UDataAsset
{
    GENERATED_BODY()
    
public:
    /** Amount of base health an enemy will be granted. The value will be scaled depending on level. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Health = 0.f;
    
    /** Amount of base damage an enemy will be granted. The value will be scaled depending on level. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Damage = 0.f;
    
    /** Amount of base speed an enemy will be granted. The value will be scaled depending on level. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Speed = 0.f;
    
    /** Experience an enemy will grant when killed. The value will be scaled depending on difficulty. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Experience = 0.f;
    
    /** Amount of mana an enemy will drop on death. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Mana = 0.f;
    
    /** Amount of base balance damage an enemy will be granted. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BalanceDamage = 0.f;
    
    /** Amount of minimal balance damage to knockback. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BalanceThreshold = 0.f;
    
    /** Percentage of balance damage to ignore. The values should be in range of [0, 100]. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BalanceResist = 0.f;

    /** Seconds an enemy will be knockback for if lost balance. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float KnockbackTime = 0.f;
};
