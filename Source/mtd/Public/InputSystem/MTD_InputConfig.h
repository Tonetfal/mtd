#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_InputConfig.generated.h"

class UInputAction;

/**
 * Wrapper containing input action with its associated input tag.
 */
USTRUCT(BlueprintType)
struct FMTD_InputAction
{
    GENERATED_BODY()

public:
    /** Input action defining an input. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UInputAction> InputAction = nullptr;

    /** Gameplay tag associated with an input action. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag InputTag;
};

/**
 * Input config containing all native and ability input actions with their associated input tags.
 */
UCLASS(BlueprintType, Const)
class MTD_API UMTD_InputConfig
    : public UDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Find an input action for a native ability given a tag.
     * @param   InputTag: input tag to use for searching.
     * @return  Found input action.
     */
    const UInputAction *FindNativeInputActionForTag(const FGameplayTag &InputTag) const;
    
    /**
     * Find an input action for a custom ability given a tag.
     * @param   InputTag: input tag to use for searching.
     * @return  Found input action.
     */
    const UInputAction *FindAbilityInputActionForTag(const FGameplayTag &InputTag) const;

public:
    /**
     * Input actions used for native abilities.
     *
     * They are intended to be manually bound.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FMTD_InputAction> NativeInputActions;

    /**
     * Input actions used for abilities.
     *
     * They are intended to be automatically bound.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FMTD_InputAction> AbilityInputActions;
};
