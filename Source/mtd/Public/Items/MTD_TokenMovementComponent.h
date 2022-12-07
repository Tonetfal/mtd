#pragma once

#include "GameFramework/MovementComponent.h"
#include "mtd.h"
#include "UObject/ObjectMacros.h"

#include "MTD_TokenMovementComponent.generated.h"

/**
 * TokenMovementComponent updates the position of another component during its tick.
 *
 * Behavior such as bouncing after impacts and homing toward a target are supported.
 *
 * Normally the root component of the owning actor is moved, however another component may be selected (see
 * SetUpdatedComponent()).
 * If the updated component is simulating physics, only the initial launch parameters (when initial velocity is
 * non-zero) will affect the projectile, and the physics sim will take over from there.
 *
 * Note: The class is very similar to ProjectileMovementComponent, but different.
 */
UCLASS(ClassGroup="Movement", meta=(BlueprintSpawnableComponent), ShowCategories=(Velocity))
class MTD_API UMTD_TokenMovementComponent : public UMovementComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnTokenBounceSignature,
        const FHitResult&, ImpactResult,
        const FVector&, ImpactVelocity);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnTokenStopSignature,
        const FHitResult&, ImpactResult);

protected:
    /** Enum indicating how simulation should proceed after HandleBlockingHit() is called. */
    enum class EHandleBlockingHitResult
    {
        /**
         * Assume velocity has been deflected, and trigger HandleDeflection(). This is the default return value of
         * HandleBlockingHit().
         */
        Deflect,

        /**
         * Advance to the next simulation update. Typically used when additional slide/multi-bounce logic can be
         * ignored, such as when an object that blocked the token is destroyed and movement should continue.
         */
        AdvanceNextSubstep,

        /**
         * Abort all further simulation. Typically used when components have been invalidated or simulation should
         * stop.
         */
        Abort,
    };

public:
    UMTD_TokenMovementComponent();
    
    //~UActorComponent Interface
    virtual void TickComponent(float DeltaSeconds, enum ELevelTick TickType,
        FActorComponentTickFunction *ThisTickFunction) override;
    //~End of UActorComponent Interface

    //~UMovementComponent Interface
    virtual void InitializeComponent() override;
    virtual void UpdateTickRegistration() override;
    
    virtual float GetMaxSpeed() const override;
    //~End of UMovementComponent Interface

protected:
    /**
     * Handle blocking hit during simulation update. Checks that simulation remains valid after collision.
     * If simulating then calls HandleImpact(), and returns EHandleHitWallResult::Deflect by default to enable
     * multi-bounce and sliding support through HandleDeflection().
     * If no longer simulating then returns EHandleHitWallResult::Abort, which aborts attempts at further simulation.
     *
     * @param  Hit					Blocking hit that occurred.
     * @param  TimeTick				Time delta of last move that resulted in the blocking hit.
     * @param  MoveDelta			Movement delta for the current sub-step.
     * @param  SubTickTimeRemaining	How much time to continue simulating in the current sub-step, which may change as a result of this function.
     *									Initial default value is: TimeTick * (1.f - Hit.Time)
     * @return Result indicating how simulation should proceed.
     * @see EHandleHitWallResult, HandleImpact()
     */
    virtual EHandleBlockingHitResult HandleBlockingHit(const FHitResult &Hit, float TimeTick, const FVector &MoveDelta,
        float &SubTickTimeRemaining);

    /**
     * Applies bounce logic if enabled to affect velocity upon impact (using ComputeBounceResult()),
     * or stops the projectile if bounces are not enabled or velocity is below BounceVelocityStopSimulatingThreshold.
     * Triggers applicable events (OnTokenBounceDelegate).
     */
    virtual void HandleImpact(const FHitResult &Hit, float TimeSlice = 0.f,
        const FVector &MoveDelta = FVector::ZeroVector) override;

    /**
     * Handle a blocking hit after HandleBlockingHit() returns a result indicating that deflection occured.
     * Default implementation increments NumBounces, checks conditions that could indicate a slide, and calls
     * HandleSliding() if necessary.
     * 
     * @param  Hit					Blocking hit that occurred. May be changed to indicate the last hit result of further movement.
     * @param  OldVelocity			Velocity at the start of the simulation update sub-step. Current Velocity may differ (as a result of a bounce).
     * @param  SubTickTimeRemaining	Time remaining in the simulation sub-step. May be changed to indicate change to remaining time.
     * @return True if simulation of the projectile should continue, false otherwise.
     * @see HandleSliding()
     */
    virtual bool HandleDeflection(FHitResult &Hit, const FVector &OldVelocity, float &SubTickTimeRemaining);

    /**
     * Handle case where the token is sliding along a surface.
     * Velocity will be parallel to the impact surface upon entry to this method.
     * 
     * @param  Hit				    Hit result of impact causing slide. May be modified by this function to reflect any subsequent movement.
     * @param  SubTickTimeRemaining	Time remaining in the tick. This function may update this time with any reduction to the simulation time requested.
     * @return True if simulation of the projectile should continue, false otherwise.
     */
    virtual bool HandleSliding(FHitResult &Hit, float &SubTickTimeRemaining);

    /** Computes result of a bounce and returns the new velocity. */
    virtual FVector ComputeBounceResult(const FHitResult &Hit, float TimeSlice, const FVector &MoveDelta);

protected:
    /** Compute the distance the token should move in the given time, at a given a velocity. */
    virtual FVector ComputeMoveDelta(FVector InVelocity, float DeltaSeconds) const;

    /**
     * Given an initial velocity and a time step, compute a new velocity.
     * Default implementation applies the result of ComputeAcceleration() to velocity.
     */
    virtual FVector ComputeVelocity(FVector InVelocity, float DeltaSeconds) const;

    /** Compute the acceleration that will be applied */
    virtual FVector ComputeAcceleration() const;

    /** Allow the projectile to track towards its homing target. */
    virtual FVector ComputeHomingAcceleration() const;
    
    virtual FVector ComputeDistanceVectorTowards(const USceneComponent *Target) const;
    virtual FVector ComputeDirectionTowards(const USceneComponent *Target) const;
    virtual FVector RotateTowards(FVector InVelocity, const USceneComponent *Target, float DeltaSeconds) const;
    
    /** Don't allow velocity magnitude to exceed MaxSpeed, if MaxSpeed is non-zero. */
    FVector LimitVelocity(FVector NewVelocity) const;

protected:
    /** Returns true if velocity magnitude is less than BounceVelocityStopSimulatingThreshold. */
    bool IsVelocityUnderSimulationThreshold() const;

    /** Compute gravity effect given current physics volume, projectile gravity scale, etc. */
    virtual float GetGravityZ() const override;
    bool ShouldApplyGravity() const;

    /**
     * Clears the reference to UpdatedComponent, fires stop event (OnTokenStopDelegate), and stops ticking
     * (if bAutoUpdateTickRegistration is true).
     */
    virtual void StopSimulating(const FHitResult &HitResult);
    bool HasStoppedSimulation() const;

    /** Check whether the token is still in the world. Check KillZ, world bounds, and handle the situation. */
    virtual bool CheckStillInWorld();
    
public:
    /** Adds a force which is accumulated until next tick, used by ComputeAcceleration() to affect Velocity. */
    UFUNCTION(BlueprintCallable, Category="MTD|Token Movement Component")
    void AddForce(FVector Force);

    /** Returns the sum of pending forces from AddForce(). */
    FVector GetPendingForce() const;

    /** Clears any pending forces from AddForce(). */
    UFUNCTION(BlueprintCallable, Category="MTD|Token Movement Component")
    void ClearPendingForce();

public:
    /** Called when the token impacts something, and bounces are enabled. */
    UPROPERTY(BlueprintAssignable)
    FOnTokenBounceSignature OnTokenBounceDelegate;

    /**
     * Called when the token stops (velocity is below simulation threshold, bounces are disabled, or it is forcibly
     * stopped).
     */
    UPROPERTY(BlueprintAssignable)
    FOnTokenStopSignature OnTokenStopDelegate;

protected:
    /**
     * Double-buffer of pending force so that updates can use the accumulated value and reset the data so other
     * AddForce() calls work correctly.
     */
    FVector PendingForceThisUpdate = FVector::ZeroVector;
    
private:
    /** Pending force for next tick. */
    UPROPERTY(BlueprintReadOnly, Category="MTD|Token Movement Component",
        meta=(AllowPrivateAccess="true"))
    FVector PendingForce = FVector::ZeroVector;

public:
    /** Limit on speed of the token (0 means no limit). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Token Movement Component")
    float MaxSpeed = 0.f;

    /** Custom gravity scale for the token. Set to 0 for no gravity. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Token Movement Component")
    float GravityScale = 1.f;
    
    /** Way the movement is performed at. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MTD|Token Movement Component")
    FVector Direction = FVector::ZeroVector;
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

    /**
     * If true, does normal simulation ticking and update. If false, simulation is halted, but component will still
     * tick (allowing interpolation to run).
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Movement Simulation")
    bool bSimulationEnabled = true;

    /**
     * If true, movement uses swept collision checks.
     * If false, collision effectively teleports to the destination. Note that when this is disabled, movement will
     * never generate blocking collision hits (though overlaps will be updated).
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Movement Simulation")
    bool bSweepCollision = true;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

    /** If true, simple bounces will be simulated. Set this to false to stop simulating on contact. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Token Bounces")
    bool bShouldBounce = false;

    /**
     * Controls the effects of friction on velocity parallel to the impact surface when bouncing.
     * If true, friction will be modified based on the angle of impact, making friction higher for perpendicular
     * impacts and lower for glancing impacts.
     * If false, a bounce will retain a proportion of tangential velocity equal to (1.0 - Friction), acting as a
     * "horizontal restitution".
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Token Bounces")
    bool bBounceAngleAffectsFriction = false;

    /**
     * Percentage of velocity maintained after the bounce in the direction of the normal of impact (coefficient of
     * restitution).
     * 1.0 = no velocity lost, 0.0 = no bounce. Ignored if bShouldBounce is false.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Token Bounces", meta=(ClampMin="0.0"))
    float Bounciness = 0.6f;

    /**
     * Coefficient of friction, affecting the resistance to sliding along a surface.
     * Normal range is [0,1] : 0.0 = no friction, 1.0+ = very high friction.
     * Also affects the percentage of velocity maintained after the bounce in the direction tangent to the normal of
     * impact.
     * Ignored if bShouldBounce is false.
     * @see bBounceAngleAffectsFriction
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Token Bounces", meta=(ClampMin="0.0"))
    float Friction = 0.2f;

    /**
     * If velocity is below this threshold after a bounce, stops simulating and triggers the OnTokenStopDelegate event.
     * Ignored if bShouldBounce is false, in which case the toke nstops simulating on the first impact.
     * @see StopSimulating(), OnTokenStopDelegate
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Token Bounces")
    float BounceVelocityStopSimulatingThreshold = 5.f;

    /**
     * When bounce angle affects friction, apply at least this fraction of normal friction.
     * Helps consistently slow objects sliding or rolling along surfaces or in valleys when the usual friction amount
     * would take a very long time to settle.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Token Bounces", AdvancedDisplay,
        meta=(ClampMin="0.0", ClampMax="1.0"))
    float MinFrictionFraction = 0.f;

    /** If true, the token will slide / roll along a surface. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Token Bounces")
    bool bIsSliding = false;

protected:
    /** Saved HitResult Time (0 to 1) from previous simulation step. Equal to 1.0 when there was no impact. */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Projectile Bounces")
    float PreviousHitTime = 1.f;

    /**
     * Saved HitResult Normal from previous simulation step that resulted in an impact. If PreviousHitTime is 1.0,
     * then the hit was not in the last step.
     */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="MTD|Projectile Bounces")
    FVector PreviousHitNormal = FVector::UpVector;
    
public:
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

    /**
     * If true, token will accelerate toward the homing target. HomingTargetComponent must be set after the token is
     * spawned.
     * @see HomingTargetComponent, HomingAccelerationMagnitude
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Homing")
    bool bIsHomingToken = false;

    /**
     * The magnitude of our acceleration towards the homing target. Overall velocity magnitude will still be limited
     * by MaxSpeed.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Homing", meta=(ClampMin="0.0"))
    float HomingAccelerationMagnitude = 0.f;

    /** Degrees the token will be able to turn per second. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Homing", meta=(ClampMin="0.0"))
    float RotationRate = 0.f;

    /**
     * The current target we are homing towards. Can only be set at runtime (when projectile is spawned or updating).
     * @see bIsHomingProjectile
     */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="MTD|Homing")
    TWeakObjectPtr<USceneComponent> HomingTargetComponent = nullptr;
};


inline bool UMTD_TokenMovementComponent::IsVelocityUnderSimulationThreshold() const
{
    return (Velocity.SizeSquared() < FMath::Square(BounceVelocityStopSimulatingThreshold));
}

inline float UMTD_TokenMovementComponent::GetMaxSpeed() const
{
    return MaxSpeed;
}

inline float UMTD_TokenMovementComponent::GetGravityZ() const
{
    return (ShouldApplyGravity()) ? (Super::GetGravityZ() * GravityScale) : (0.f);
}

inline bool UMTD_TokenMovementComponent::ShouldApplyGravity() const
{
    return (GravityScale != 0.f);
}

inline bool UMTD_TokenMovementComponent::HasStoppedSimulation() const
{
    return ((!IsValid(UpdatedComponent)) || (!IsActive()));
}

inline void UMTD_TokenMovementComponent::AddForce(FVector Force)
{
    PendingForce += Force;
}

inline FVector UMTD_TokenMovementComponent::GetPendingForce() const
{
    return PendingForce;
}

inline void UMTD_TokenMovementComponent::ClearPendingForce()
{
    PendingForce = FVector::ZeroVector;
}
