#pragma once

#include "mtd.h"

#include "MTD_LevelDifficultyDefinition.generated.h"

class UMTD_FoeRateDefinition;

/**
 * Data asset defining a single difficulty for a level.
 *
 * It represents difficulties such as Easy, Medium, Hard, Insane etc.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_LevelDifficultyDefinition
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Get intensivity curve for a given wave.
     * @param   Wave: wave to get the curve for.
     * @return  Intensivity curve for a given wave.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UCurveFloat *GetIntensivityCurve(int32 Wave) const;

    /**
     * Get characters rate definition for a given wave.
     * @param   Wave: wave to get the characters rate definition for.
     * @return  Characters rate definition for a given wave.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UMTD_FoeRateDefinition *GetCharacterRateDefinition(int32 Wave) const;

public:
    /** Mob difficulty value on wave 1. Will be scaled up with wave progression. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseDifficulty = 0.f;

    // @todo ItemQuality
    
    /** Mob quantity value on wave 1. Will be scaled up with wave progression. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseQuantity = 0.f;
    
    /** Time in seconds mobs spawn for on wave 1. Will be scaled up with wave progression. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseTime = 0.f;

    /**
     * Default foe rate data. May be overriden by specific foe rate data table.
     *
     * @see SpecificFoeRateData
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UMTD_FoeRateDefinition> DefaultFoeRateData = nullptr;
    
    /**
     * Default intensivity float curve. May be overriden by specific foe intensivity curve.
     *
     * @see SpecificFoeIntensivityCurve
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UCurveFloat> DefaultIntensivityCurve = nullptr;

    /** Foe rate data for specific waves. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<int32 /* WaveIndex */, TObjectPtr<const UMTD_FoeRateDefinition>> SpecificFoeRateData;
    
    /** Intensivity float curve for specific waves. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<int32 /* WaveIndex */, TObjectPtr<const UCurveFloat>> SpecificIntensivityCurve;
};
