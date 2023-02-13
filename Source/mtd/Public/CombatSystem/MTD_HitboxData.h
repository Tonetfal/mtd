#pragma once

#include "Engine/DataAsset.h"
#include "mtd.h"

#include "MTD_HitboxData.generated.h"

/**
 * Definition of a single hitbox.
 */
USTRUCT(BlueprintType)
struct FMTD_HitboxDefinition
{
    GENERATED_BODY()

public:
    /**
     * Check for nicknames equality.
     * @param   Rhs: hitbox definition to compare with.
     * @return  If true, two hitboxes' nicknames are equal, false otherwise.
     */
    bool operator==(const FMTD_HitboxDefinition &Rhs) const
    {
        return Nickname == Rhs.Nickname;
    }

public:
    /** Unique nickname associated with this hitbox. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName Nickname = NAME_None;

    /** Offset hitbox has to be at relatively to owner. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector Offset = FVector::ZeroVector;

    /** Hitbox sphere radius. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Radius = 0.f;
};

/**
 * Set of hitboxes.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_HitboxData
    : public UDataAsset
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure)
    TArray<FMTD_HitboxDefinition> GetMeleeHitSpheres(const TArray<int32> &Indices);
    
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FMTD_HitboxDefinition> Hitboxes;
};
