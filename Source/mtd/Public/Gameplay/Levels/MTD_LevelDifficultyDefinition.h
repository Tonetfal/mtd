#pragma once

#include "mtd.h"

#include "MTD_LevelDifficultyDefinition.generated.h"

class UMTD_CharacterRateDefinition;

/**
 * Data Asset defining a single difficulty for a level.
 *
 * It represents difficulties such as Easy, Medium, Hard, Insane etc.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_LevelDifficultyDefinition
    : public UDataAsset
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UCurveFloat *GetIntensivityCurve(int32 Wave) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    const UMTD_CharacterRateDefinition *GetCharacterRateDefinition(int32 Wave) const;

public:
    /** Mob Difficulty value on wave 1. Will be scaled up with wave progression. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseDifficulty = 0.f;

    // @todo ItemQuality
    
    /** Mob Quantity value on wave 1. Will be scaled up with wave progression. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseQuantity = 0.f;
    
    /** Wave Time value (in seconds) on wave 1. Will be scaled up with wave progression. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BaseTime = 0.f;

    /** Generally used Enemy Rate Data if there is no specific one in SpecificEnemyRateData. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UMTD_CharacterRateDefinition> DefaultEnemyRateData = nullptr;
    
    /** Generally used Intensivity Float Curve if there is no specific one in SpecificEnemyIntensivityCurve. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UCurveFloat> DefaultIntensivityCurve = nullptr;

    /** Enemy Rate Data for specific waves. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<int32 /* WaveIndex */, TObjectPtr<const UMTD_CharacterRateDefinition>> SpecificEnemyRateData;
    
    /** Intensivity Float Curve for specific waves. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<int32 /* WaveIndex */, TObjectPtr<const UCurveFloat>> SpecificIntensivityCurve;
};
