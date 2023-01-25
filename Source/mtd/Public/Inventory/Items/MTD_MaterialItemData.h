#pragma once

#include "MTD_BaseInventoryItemData.h"

#include "MTD_MaterialItemData.generated.h"

USTRUCT(BlueprintType)
struct FMTD_MaterialItemDataRow
    : public FTableRowBase
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Materials")
    int32 MaxAmount = 250;
};

UCLASS()
class MTD_API UMTD_MaterialItemData
    :  public UMTD_BaseInventoryItemData
{
   GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    UPROPERTY(BlueprintReadOnly, Category="Materials")
    int32 CurrentAmount = 0;

    UPROPERTY(BlueprintReadOnly, Category="Materials")
    int32 MaxAmount = 0;
};
