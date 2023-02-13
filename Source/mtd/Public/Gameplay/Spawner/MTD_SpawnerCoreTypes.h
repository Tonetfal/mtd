#pragma once

#include "mtd.h"

#include "MTD_SpawnerCoreTypes.generated.h"

class AMTD_BaseFoeCharacter;

/**
 * Data asset defining how much of a foe has to spawn.
 *
 * The Quantity does not directly represent the final amount of foes.
 */
UCLASS(Const)
class MTD_API UMTD_FoeQuantityDefinition
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Key: Foe class the data is associated with.
     * 
     * Value: Parameter defining how much of Quantity should a spawner have for that particular FoeClass in order
     * to spawn one. Note that it will be multiplied by QuantityRate.
     */
    UPROPERTY(EditDefaultsOnly)
    TMap<TSubclassOf<AMTD_BaseFoeCharacter>, float> QuantityMap;
};

/**
 * Data asset defining scale for foes amount.
 *
 * The scale directly multiplies final Quantity value. If there has to spawn 5 enemies, rate of 2 will make them 10.
 */
UCLASS()
class MTD_API UMTD_FoeRateDefinition
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Key: Foe class the data is associated with.
     * 
     * Value: QuantityStep multiplier.
     *
     * Example: if a foe is spawned each 2 Quantity values, the QuantityRate of 2 would make it spawn each 4 instead.
     * Set the rate to 0 to make a character to not spawn at all.
     */
    UPROPERTY(EditDefaultsOnly)
    TMap<TSubclassOf<AMTD_BaseFoeCharacter>, float> RateMap;
};
