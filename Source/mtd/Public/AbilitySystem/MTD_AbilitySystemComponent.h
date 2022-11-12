#pragma once

#include "AbilitySystemComponent.h"
#include "mtd.h"

#include "MTD_AbilitySystemComponent.generated.h"

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

private:
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
};
