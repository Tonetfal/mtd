#pragma once

#include "AbilitySystemComponent.h"
#include "mtd.h"

#include "MTD_AbilitySystemComponent.generated.h"

class UMTD_GameplayAbility;

/**
 * Default ability system component used in this project.
 */
UCLASS()
class MTD_API UMTD_AbilitySystemComponent
    : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    /**
     * Process all the gathered inputs since last process. Should be called each frame.
     * @param   DeltaSeconds: time since last process.
     * @param   bGamePaused: if true, the game is paused, false otherwise.
     */
    void ProcessAbilityInput(float DeltaSeconds, bool bGamePaused);

    /**
     * Clear all ability inputs, such as press, hold, and release.
     */
    void ClearAbilityInput();

    /**
     * Set a new duration to an active gameplay effect handle.
     * @param   Handle: active gameplay effect handle to set the new duration to.
     * @param   NewDuration: new duration time in seconds.
     * @return  If true, the duration has been successfully changed, false otherwise.
     */
    bool SetGameplayEffectDurationHandle(FActiveGameplayEffectHandle Handle, float NewDuration);
    
    /**
     * Change level of an active gameplay effect handle.
     * @param   Handle: active gameplay effect handle to change the level of.
     * @param   DeltaLevel: delta to add to the gameplay effect level.
     * @return  If true, the level has been successfully changed, false otherwise.
     */
    bool ChangeGameplayEffectLevelHandle(FActiveGameplayEffectHandle Handle, int32 DeltaLevel) const;

    /**
     * Event to fire when a listened input has been pressed.
     * @param   InputTag: input tag associated with the pressed input button.
     */
    void OnAbilityInputTagPressed(const FGameplayTag &InputTag);
    
    /**
     * Event to fire when a listened input has been releaseed.
     * @param   InputTag: input tag associated with the releaseed input button.
     */
    void OnAbilityInputTagReleased(const FGameplayTag &InputTag);

    /**
     * Give an input tag to a given a gameplay ability. The ability must be present in the ability system component.
     * @param   InputTag: gameplay tag to add to the abliity.
     * @param   Ability: ability to assosiate the input tag with.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Ability System Component")
    void GiveTagToAbility(const FGameplayTag &InputTag, UMTD_GameplayAbility *Ability);

private:
    /** Container of gameplay ability spec handles which assosiated input has been pressed. */
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

    /** Container of gameplay ability spec handles which assosiated input is being held. */
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

    /** Container of gameplay ability spec handles which assosiated input has been released. */
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
};
