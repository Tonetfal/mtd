#pragma once

#include "Engine/DataAsset.h"
#include "mtd.h"
#include "ScalableFloat.h"

#include "MTD_PawnData.generated.h"

class UMTD_AbilitySet;

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
};
