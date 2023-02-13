#pragma once

#include "MTD_BaseInventoryItemData.h"

#include "MTD_MaterialItemData.generated.h"

/**
 * Material item data row containing generic material information.
 */
USTRUCT(BlueprintType)
struct FMTD_MaterialItemDataRow
    : public FTableRowBase
{
    GENERATED_BODY()

public:
    /**
     * Main config.
     */

    /** Maximum amount of item copies that can be in single slot. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Materials")
    int32 MaxAmount = 250;
};

/**
 * Material item data containing specific material information.
 */
UCLASS()
class MTD_API UMTD_MaterialItemData
    :  public UMTD_BaseInventoryItemData
{
   GENERATED_BODY()

public:
    /**
     * Main config.
     */
    
    /** Maximum amount of item copies that can be in single slot. */
    UPROPERTY(BlueprintReadOnly, Category="Materials")
    int32 MaxAmount = 0;

    /** Current amount of item copies in single slot. */
    UPROPERTY(BlueprintReadOnly, Category="Materials")
    int32 CurrentAmount = 0;
};
