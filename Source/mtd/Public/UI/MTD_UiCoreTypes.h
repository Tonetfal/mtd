#pragma once

#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_UiCoreTypes.generated.h"

/**
 * Asset describing a single ability UI properties.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_AbilityUiData : public UDataAsset
{
    GENERATED_BODY()

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<UTexture2D> Icon = nullptr;
};

/**
 * Asset containing ability UI data and other front-end related data.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_AbilityUiDataWrapper : public UDataAsset
{
    GENERATED_BODY()

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilityUiData> AbilityData = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="9.0"))
    int32 SlotIndex = 0;
};

/**
 * Set of Ability UI Data with associated with its respective Gameplay Tag.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_AbilitiesUiData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TMap<FGameplayTag, TObjectPtr<UMTD_AbilityUiDataWrapper>> Data;
};
