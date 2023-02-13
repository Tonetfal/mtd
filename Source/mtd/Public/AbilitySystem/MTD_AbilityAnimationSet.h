#pragma once

#include "Containers/Map.h"
#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_AbilityAnimationSet.generated.h"

/**
 * Container of animation montages that can be played when using an ability.
 */
USTRUCT(BlueprintType)
struct FMTD_AbilityAnimations
{
    GENERATED_BODY()

public:
    /** All animations that can be played when using an ability. */
    UPROPERTY(EditDefaultsOnly)
    TArray<TObjectPtr<UAnimMontage>> Animations;
};

/**
 * Container of animation montages that can be played for specific abilities.
 */
UCLASS(BlueprintType)
class UMTD_AbilityAnimationSet
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Get all animation montages that can be played for an ability with the given gameplay tag.
     * @param   AbilityTag: gameplay tag representing MTD gameplay ability's main tag.
     * @param   OutAbilityAnimations: output parameter. All animation montages that can be played for an ability with
     * the given gameplay tag.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Ability System")
    void GetAbilityAnimMontages(const FGameplayTag &AbilityTag, FMTD_AbilityAnimations &OutAbilityAnimations) const;

private:
    /** MTD gameplay ability's main tags binding to all animation montages that can be played for them. */
    UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
    TMap<FGameplayTag, FMTD_AbilityAnimations> AbilityAnimations;
};
