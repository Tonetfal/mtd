#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "mtd.h"

#include "MTD_FloatingToken.generated.h"

class UBoxComponent;
class UMTD_TokenMovementComponent;
class USphereComponent;

/**
 * Token actor that floats around in the world.
 *
 * Is intented to follow a target, adn do something when overlapped with it.
 */
UCLASS(Abstract)
class MTD_API AMTD_FloatingToken
    : public AActor
{
    GENERATED_BODY()

public:
    AMTD_FloatingToken();
    
    //~AActor Interface
    virtual void PostInitializeComponents() override;
    //~End of AActor Interface

    /**
     * Don't fly towards any trigger for a given time, however, be still activatable by them if overlapped.
     * @param   IgnoreTime: time in seconds target have to be ignored for.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Floating Token")
    void StopHomingFor(float IgnoreTime);

    /**
     * Check whether the token can be activated by a given pawn.
     * @param   Trigger: pawn that's trying to activate us.
     * @return  If true, given pawn can activate us, false otherwise.
     */
    virtual bool CanBeActivatedBy(APawn *Trigger) const;

protected:
    /**
     * Event to fire when a pawn is activating us.
     * @param   Trigger: pawn that's activating us.
     */
    UFUNCTION(BlueprintNativeEvent, Category="MTD|Floating Token")
    void OnActivate(APawn *Trigger);
    virtual void OnActivate_Implementation(APawn *Trigger);

    /**
     * Get a pawn that can be followed.
     * @return  Pawn actor that can be followed. May be nullptr.
     */
    virtual APawn *FindNewTarget() const;

    /**
     * Event to fire when a pawn was added to target list.
     * @param   Trigger: added pawn.
     */
    virtual void OnTriggerAdded(APawn *Trigger);
    
    /**
     * Event to fire when a pawn was removed from target list.
     * @param   Trigger: removed pawn.
     */
    virtual void OnTriggerRemoved(APawn *Trigger);

    /**
     * Set a new target to follow.
     * @param   Target: new target pawn. May be nullptr.
     */
    virtual void SetNewTarget(APawn *Target);

    /**
     * Get impulse towards a given pawn.
     * @param   Trigger: pawn to gain speed towards.
     * @return  Impulse towards the given pawn.
     */
    FVector GetImpulseTowards(const APawn *Trigger) const;

    /**
     * Activate scan mode.
     *
     * Add all detected triggers to listening.
     *
     * @see bScanMode
     */
    virtual void EnableScan();
    
    /**
     * Deactivate scan mode.
     * 
     * Remove all detected triggers from listening.
     *
     * @see bScanMode
     */
    virtual void DisableScan();

    /**
     * Add a new actor to listening.
     *
     * Is intended to be called when scan mode is enabled, and when a new actor cannot be targetted at the moment.
     * @param   Trigger: actor to add to listening.
     */
    virtual void AddToListening(AActor *Trigger);

    /**
     * Remove an actor from listening.
     * @param   Trigger: actor to remove from listening.
     */
    virtual void RemoveFromListening(AActor *Trigger);

    /**
     * Remove actor that we are targetting at the moment from listening.
     */
    void RemoveCurrentTargetFromListening();

private:
    /**
     * Event to fire when a triger actor enters activation trigger area.
     */
    UFUNCTION()
    void OnActivationTriggerBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);
    
    /**
     * Event to fire when a triger actor enters detection trigger area.
     */
    UFUNCTION()
    void OnDetectTriggerBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    /**
     * Event to fire when a trigger actor leaves detection trigger area.
     */
    UFUNCTION()
    void OnDetectTriggerEndOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex);

    /**
     * Event to fire when trigger actors have to be not ignored anymore.
     */
    UFUNCTION()
    void OnStopIgnoreTriggers();

protected:
    /** Token movement component. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components")
    TObjectPtr<UMTD_TokenMovementComponent> MovementComponent = nullptr;

    /** List of currently detected triggers. */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Trigger|Runtime")
    TArray<TObjectPtr<APawn>> DetectedTriggers;

    /** Don't fly specifically towards anything? */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Trigger|Runtime")
    bool bDontHome = false;

    /** Is scan mode enabled? */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Trigger|Runtime")
    bool bScanMode = false;

private:
    /** World collision component. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> CollisionComponent = nullptr;

    /** Area where a trigger actor can try to activate us. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> ActivationTriggerComponent = nullptr;

    /** Area where a trigger actor is detected in. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> DetectTriggerComponent = nullptr;

    /** Speed that the token will gain when there is a new homing target. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Trigger", meta=(AllowPrivateAccess="true"))
    float ImpulseStrength = 50.f;

    /** Timer handle to manage ignore triggers event. */
    FTimerHandle IgnoreTimerHandle;
};
