#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "mtd.h"

#include "MTD_AbilityBarWidget.generated.h"

class AMTD_BaseCharacter;

/**
 * An asset describing a single ability UI properties.
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
 * An asset containing the ability UI data and the appropriate slot that the ability will be placed on HUD.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_AbilitySlotUiData : public UDataAsset
{
    GENERATED_BODY()

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilityUiData> AbilityData = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true", ClampMin="0.0"))
    int32 SlotIndex = 0;
};

/**
 * A set of Ability UI Data with associated with its respective Gameplay Tag.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_AbilitiesUiData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TMap<FGameplayTag,TObjectPtr<UMTD_AbilitySlotUiData>> Data;
};

/**
 * A set of Abilities UI Data with its respective Hero Player.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_HeroesAbilityUiData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TMap<TSubclassOf<AMTD_BaseCharacter>,TObjectPtr<UMTD_AbilitiesUiData>> Data;
};

UCLASS()
class MTD_API UMTD_AbilityBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

private:
    void PickHeroUiData();

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Ability Bar Widget", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroesAbilityUiData> HeroesAbilityUiData = nullptr;

    UPROPERTY(BlueprintReadOnly, Category="MTD|Ability Bar Widget", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilitiesUiData> ActiveHeroUiData = nullptr;
};
