#pragma once

#include "mtd.h"
#include "ScalableFloat.h"
#include "Engine/DataAsset.h"

#include "MTD_PawnData.generated.h"

class UMTD_AbilitySet;
class UMTD_InputConfig;

USTRUCT(BlueprintType)
struct FPawnAttributes
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FScalableFloat Health;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FScalableFloat Mana;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FScalableFloat Skillpoints;
};

UCLASS(BlueprintType, Const,
    meta=(ShortTooltip = "Data asset used to define a Pawn."))
class UMTD_PawnData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /// Ability sets to grant to this pawn's ability system.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MTD|Abilities")
    TArray<UMTD_AbilitySet *> AbilitySets;

    /// Values to initialize the pawn attributes with.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MTD|Attributes")
    TObjectPtr<UCurveTable> PlayerDefaultAttributes = nullptr;

    /// Input configuration used players to create input mappings and bind input actions.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MTD|Input")
    TObjectPtr<UMTD_InputConfig> InputConfig = nullptr;
};
