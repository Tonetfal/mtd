#pragma once

#include "Gameplay/Tokens/MTD_FloatingToken.h"
#include "mtd.h"

#include "MTD_ManaToken.generated.h"

class UMTD_GameplayEffect;
class UMTD_ManaComponent;

/**
 * Mana token that floats around in the world.
 */
UCLASS()
class MTD_API AMTD_ManaToken
    : public AMTD_FloatingToken
{
    GENERATED_BODY()

protected:
    //~AMTD_FloatingToken Interface
    virtual bool CanBeActivatedBy(APawn *Trigger) const override;
    virtual void OnActivate_Implementation(APawn *Trigger) override;
    virtual APawn *FindNewTarget() const override;

    virtual void OnTriggerAdded(APawn *Trigger) override;
    virtual void SetNewTarget(APawn *Target) override;

    virtual void AddToListening(AActor *Trigger) override;
    virtual void RemoveFromListening(AActor *Trigger) override;
    //~End of AMTD_FloatingToken Interface

    /**
     * Spawn mana tokens.
     * @param   Target: context actor the function is called from. Will be the tokens owner.
     * @param   Transform: transform to spawn tokens with.
     * @param   ManaTokens: mana tokens to spawn.
     * @param   TriggersIgnoreTime: time in seconds newly spawned mana tokens should ignore triggers for.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Mana Token")
    static TArray<AMTD_ManaToken *> SpawnMana(
        AActor *Target,
        const FTransform &Transform,
        const TMap<TSubclassOf<AMTD_ManaToken>, int32> &ManaTokens,
        float TriggersIgnoreTime = 0.f);

    /**
     * Push token in a random direction which is supposed to be upwards with a given random speed limits.
     * @param   MinSpeed: minimal speed token will be pushed with.
     * @param   MaxSpeed: maximum speed token will be pushed with.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Mana Token")
    void AddRandomDirectionForce(const float MinSpeed, const float MaxSpeed);

    /**
     * Push all tokens in a random direction which is supposed to be upwards with a given random speed limits.
     * @param   ManaTokens: mana tokens to randomly push.
     * @param   MinSpeed: minimal speed tokens will be pushed with.
     * @param   MaxSpeed: maximum speed tokens will be pushed with.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Mana Token")
    static void AddRandomDirectionForceToTokens(const TArray<AMTD_ManaToken *> &ManaTokens, const float MinSpeed,
        const float MaxSpeed);

private:
    /**
     * Event to fire when mana value is changed while in scan mode.
     * @param   ManaComponent: mana component the mana value has changed on.
     */
    void OnManaChangeHandleScanCase(UMTD_ManaComponent *ManaComponent);
    
    /**
     * Event to fire when mana value is changed.
     * @param   ManaComponent: mana component the mana value has changed on.
     * @param   OldValue: old mana value.
     * @param   NewValue: new mana value.
     * @param   InInstigator: logical actor caused the whole chain.
     */
    UFUNCTION()
    void OnTargetManaAttributeChanged(UMTD_ManaComponent *ManaComponent, float OldValue, float NewValue,
        AActor* InInstigator);
    
    /**
     * Event to fire when max mana value is changed.
     * @param   ManaComponent: mana component the max mana value has changed on.
     * @param   OldValue: old max mana value.
     * @param   NewValue: new max mana value.
     * @param   InInstigator: logical actor caused the whole chain.
     */
    UFUNCTION()
    void OnTargetMaxManaAttributeChanged(UMTD_ManaComponent *ManaComponent, float OldValue, float NewValue,
        AActor* InInstigator);

private:
    /** Amount of mana a trigger will be granted with on activation. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Mana Token", meta=(ClampMin="0.0"))
    int32 ManaAmount = 1;
};
