#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "mtd.h"

#include "MTD_AbilityBarWidget.generated.h"

class AMTD_BaseCharacter;
class UMTD_AbilitiesUiData;

/**
 * A set of Abilities UI Data with its respective Hero Player.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_HeroesAbilityUiData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TMap<TSubclassOf<AMTD_BaseCharacter>, TObjectPtr<UMTD_AbilitiesUiData>> Data;
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
