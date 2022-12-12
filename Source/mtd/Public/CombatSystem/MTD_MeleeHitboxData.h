#pragma once

#include "Engine/DataAsset.h"
#include "mtd.h"

#include "MTD_MeleeHitboxData.generated.h"

USTRUCT(BlueprintType)
struct FMTD_MeleeHitSphereDefinition
{
    GENERATED_BODY()

public:
    bool operator==(const FMTD_MeleeHitSphereDefinition &Rhs) const
    {
        return Nickname == Rhs.Nickname;
    }

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName Nickname;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector Offset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Radius;
};

UCLASS(BlueprintType)
class MTD_API UMTD_MeleeHitboxData : public UDataAsset
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure)
    TArray<FMTD_MeleeHitSphereDefinition> GetMeleeHitSpheres(TArray<int32> Indices);
    
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FMTD_MeleeHitSphereDefinition> MeleeHitSpheres;
};

USTRUCT()
struct FMTD_ActiveHitboxEntry
{
    GENERATED_BODY()

public:
    FMTD_MeleeHitSphereDefinition HitboxInfo;
};
