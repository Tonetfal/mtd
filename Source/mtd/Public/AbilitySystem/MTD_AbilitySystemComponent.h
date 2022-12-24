#pragma once

#include "AbilitySystemComponent.h"
#include "mtd.h"

#include "MTD_AbilitySystemComponent.generated.h"

class UMTD_GameplayAbility;
UCLASS()
class MTD_API UMTD_AbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    void ProcessAbilityInput(float DeltaSeconds, bool bGamePaused);
    bool SetGameplayEffectDurationHandle(FActiveGameplayEffectHandle Handle, float NewDuration);
    bool IncreaseGameplayEffectLevelHandle(FActiveGameplayEffectHandle Handle, float IncreaseBy) const;

    void OnAbilityInputTagPressed(const FGameplayTag &InputTag);
    void OnAbilityInputTagReleased(const FGameplayTag &InputTag);

    UFUNCTION(BlueprintCallable, Category="MTD|Ability System Component")
    void GiveTagToAbility(const FGameplayTag &Tag, UMTD_GameplayAbility *Ability);

private:
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
};
