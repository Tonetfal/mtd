#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "mtd.h"

#include "MTD_ItemDropsBlueprintFunctionLibrary.generated.h"

class AMTD_BaseEnemyCharacter;
class AMTD_InventoryItemInstance;
class UMTD_BaseInventoryItemData;

UCLASS()
class MTD_API UMTD_ItemDropsBlueprintFunctionLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static UMTD_BaseInventoryItemData *CreateRandomDropItem(AMTD_BaseEnemyCharacter *WorldContextObject);
    
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AMTD_InventoryItemInstance *CreateRandomDropItemInstance(AMTD_BaseEnemyCharacter *WorldContextObject,
        bool bWarnIfFailed = true);
};
