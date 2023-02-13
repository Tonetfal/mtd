#pragma once

#include "mtd.h"

#include "MTD_LevelDefinition.generated.h"

class ALevelInstance;
class AMTD_ManaToken;
class UMTD_FoeRateDefinition;
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
 * Data asset defining a single level. It represents a playable level.
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

    /** Map this level takes place on. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FSoftObjectPath Map;

    /**
     * Time in seconds first wave will be force started after. May not be used
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 FirstWaveForceStartTime = 60.f;
    
    /**
     * Time in seconds a wave, except the first one, will be force started after. May not be used
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 RegularWaveForceStartTime = 30.f;

    /** Maximum health a single core has. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 CoreHealth = 1000;

    /** Mana tokens a chest drops on opening. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<TSubclassOf<AMTD_ManaToken>, int32> ChestManaTokens;

    /** All difficulties a level can be played on. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<EMTD_LevelDifficulty, TObjectPtr<UMTD_LevelDifficultyDefinition>> Difficulties;
};
