#pragma once

#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "mtd.h"

#include "MTD_InputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FMTD_InputAction
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UInputAction> InputAction = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag InputTag;
};

UCLASS(BlueprintType, Const)
class MTD_API UMTD_InputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    const UInputAction *FindNativeInputActionForTag(FGameplayTag InputTag) const;
    const UInputAction *FindAbilityInputActionForTag(FGameplayTag InputTag) const;

public:
    /// List of input actions used by the owner. These input actions are mapped to a gameplay tag and must be manually
    /// bound.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FMTD_InputAction> NativeInputActions;

    /// List of input actions used by the owner. These input actions are mapped to a gameplay tag and are automatically
    /// bound to abilities with matching input tags.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FMTD_InputAction> AbilityInputActions;
};
