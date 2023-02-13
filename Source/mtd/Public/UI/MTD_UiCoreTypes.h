#pragma once

#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_UiCoreTypes.generated.h"

/**
 * Asset containing UI data for an ability.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_AbilityUiData
    : public UDataAsset
{
    GENERATED_BODY()

private:
    /** Ability icon. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<UTexture2D> Icon = nullptr;
};

/**
 * Asset containing UI data for an ability and other front-end related data.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_AbilityUiDataWrapper
    : public UDataAsset
{
    GENERATED_BODY()

private:
    /** Ability UI data. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilityUiData> AbilityData = nullptr;

    /** Index the ability has to be on. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="9.0"))
    int32 SlotIndex = 0;
};

/**
 * Container of ability UI data with associated gameplay tag.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_AbilitiesUiData
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Container of ability UI data with associated gameplay tag. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TMap<FGameplayTag, TObjectPtr<UMTD_AbilityUiDataWrapper>> Data;
};
