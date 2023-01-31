#pragma once

#include "Equipment/MTD_EquipmentCoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "mtd.h"

#include "MTD_InventoryBlueprintFunctionLibrary.generated.h"

class AMTD_InventoryItemInstance;
class UMTD_BaseInventoryItemData;

UCLASS()
class MTD_API UMTD_InventoryBlueprintFunctionLibrary
    : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static UMTD_BaseInventoryItemData *GenerateInventoryItemData(
        UObject *WorldContextObject, int32 ItemID, float Difficulty);
    
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static UMTD_BaseInventoryItemData *CreateBaseInventoryItemData(UObject* WorldContextObject, int32 ItemID);
    
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static AMTD_InventoryItemInstance *CreateItemInstance(AActor* WorldContextObject,
        UMTD_BaseInventoryItemData *ItemData);
    
	UFUNCTION(BlueprintCallable, BlueprintPure)
    static EMTD_EquipmentType GetEquipmentType(UMTD_BaseInventoryItemData *ItemData);
    
	UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEquippable(UMTD_BaseInventoryItemData *ItemData);

	UFUNCTION(BlueprintCallable, BlueprintPure)
    static int32 GetItemAmount(UMTD_BaseInventoryItemData *ItemData);

	UFUNCTION(BlueprintCallable, BlueprintPure)
    static int32 GetMaxItemAmount(UMTD_BaseInventoryItemData *ItemData);
};
