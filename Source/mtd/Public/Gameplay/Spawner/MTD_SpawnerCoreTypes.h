#pragma once

#include "mtd.h"

#include "MTD_SpawnerCoreTypes.generated.h"

class AMTD_BaseCharacter;

UCLASS(Const)
class MTD_API UMTD_CharacterQuantityDefinition
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Key: Character the data is associated with.
     * 
     * Value: Parameter defining how much of Quantity should a spawner have for that particular CharacterClass in order
     * to spawn one. Note that it will be multiplied by QuantityRate.
     */
    UPROPERTY(EditDefaultsOnly)
    TMap<TSubclassOf<AMTD_BaseCharacter>, float> QuantityMap;
};

UCLASS()
class MTD_API UMTD_CharacterRateDefinition
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Key: Character the data is associated with.
     * 
     * Value: QuantityStep multiplier. Example: if an enemy is spawned each 2 Quantity values, the QuantityRate of 2
     * would make it spawn each 4 instead. Set the rate to 0 to make the character not spawn on a wave at all.
     */
    UPROPERTY(EditDefaultsOnly)
    TMap<TSubclassOf<AMTD_BaseCharacter>, float> RateMap;
};
