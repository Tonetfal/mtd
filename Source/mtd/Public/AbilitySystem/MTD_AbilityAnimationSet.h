#pragma once

#include "Containers/Map.h"
#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_AbilityAnimationSet.generated.h"

USTRUCT(BlueprintType)
struct FMTD_AbilityAnimations
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TArray<TObjectPtr<UAnimMontage>> Animations;
};

UCLASS(BlueprintType)
class UMTD_AbilityAnimationSet : public UDataAsset
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="MTD|Ability System")
    FMTD_AbilityAnimations GetAbilityAnimMontages(FGameplayTag AbilityTag) const;

private:
    UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
    TMap<FGameplayTag,FMTD_AbilityAnimations> AbilityAnimations;
};
