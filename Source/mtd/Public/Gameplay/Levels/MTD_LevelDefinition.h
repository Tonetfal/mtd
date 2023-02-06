#pragma once

#include "mtd.h"

#include "MTD_LevelDefinition.generated.h"

class ALevelInstance;
class UMTD_CharacterRateDefinition;
class UMTD_LevelDifficultyDefinition;

/**
 * Enum representing a difficulty level on a game level.
 */
UENUM(BlueprintType)
enum class EMTD_LevelDifficulty : uint8
{
    Invalid,
    Easy,
    Medium,
    Hard,
    Insane
};

/**
 * Data Asset defining a single level. It represents a playable level.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_LevelDefinition
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Level name. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName Name = "None";

    /** The map this level takes placee on. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FSoftObjectPath Map;

    /**
     * If difficulty level is supposed to force start a wave after some time, this value will be used at the very first
     * one.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 FirstWaveForceStartSeconds = 60.f;
    
    /**
     * If difficulty level is supposed to force start a wave after some time, this value will be used at any wave except
     * the very first one.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 RegularWaveForceStartSeconds = 30.f;

    /** Amount of maximum health a single core will have. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 CoreHealth = 1000;

    /** Amount of mana a chest has. */
    // @todo split this into a set of tokens instead of tokenizing the value which may lead to a one big token
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 ManaChest = 50.f;

    /** All the difficulties the level can be played on. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<EMTD_LevelDifficulty, TObjectPtr<UMTD_LevelDifficultyDefinition>> Difficulties;
};
