#pragma once

#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "mtd.h"

#include "MTD_AbilityBarWidget.generated.h"

class AMTD_BaseCharacter;
class UMTD_AbilitiesUiData;

/**
 * Container of ability UI data with its associated characters.
 */
UCLASS(BlueprintType)
class MTD_API UMTD_HeroesAbilityUiData
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /** Container of ability UI data associated with characters. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
    TMap<TSubclassOf<AMTD_BaseCharacter>, TObjectPtr<UMTD_AbilitiesUiData>> Data;
};

/**
 * 
 */
UCLASS()
class MTD_API UMTD_AbilityBarWidget
    : public UUserWidget
{
    GENERATED_BODY()

public:
    //~UUserWidget Interface
    virtual bool Initialize() override;
    //~End of UUserWidget Interface

private:
    /**
     * Select heroes ability UI data for owner character.
     */
    void SelectHeroUiData();

private:
    /** Container of ability UI data with its associated character. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Ability Bar Widget", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroesAbilityUiData> HeroesAbilityUiData = nullptr;

    /** Hero UI data used for owner character. */
    UPROPERTY(BlueprintReadOnly, Category="MTD|Ability Bar Widget", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilitiesUiData> ActiveHeroUiData = nullptr;
};
