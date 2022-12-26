#pragma once

#include "Engine/DataAsset.h"
#include "mtd.h"

#include "MTD_ItemCoreTypes.generated.h"

class AMTD_ManaToken;

UCLASS()
class MTD_API UMTD_ManaTokensData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId("ManaTokens", GetFName());
    }

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        meta=(ShortTooltip="Tokens must be sorted from the smallest to the greatest."))
    TMap<int32, TSubclassOf<AMTD_ManaToken>> Data;
};
