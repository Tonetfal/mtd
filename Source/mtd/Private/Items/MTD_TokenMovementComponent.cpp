#include "Items/MTD_TokenMovementComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "EngineDefines.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/WorldSettings.h"
#include "ProfilingDebugging/CsvProfiler.h"

UMTD_TokenMovementComponent::UMTD_TokenMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    bUpdateOnlyIfRendered = false;

    bWantsInitializeComponent = true;
    bComponentShouldUpdatePhysicsVolume = false;
}

void UMTD_TokenMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();

    if (Velocity.SizeSquared() > 0.f)
    {
        const bool Valid = IsValid(UpdatedComponent);
        
        if (Valid)
        {
            const FRotator DesiredRotation = Velocity.Rotation();
            UpdatedComponent->SetWorldRotation(DesiredRotation);
        }

        UpdateComponentVelocity();

        if ((Valid) && (UpdatedPrimitive->IsSimulatingPhysics()))
        {
            UpdatedPrimitive->SetPhysicsLinearVelocity(Velocity);
        }
    }
}

void UMTD_TokenMovementComponent::UpdateTickRegistration()
{
    if (bAutoUpdateTickRegistration)
    {
        Super::UpdateTickRegistration();
    }
}

void UMTD_TokenMovementComponent::TickComponent(float DeltaSeconds, enum ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction)
{
    QUICK_SCOPE_CYCLE_COUNTER(STAT_TokenMovementComponent_TickComponent);
    CSV_SCOPED_TIMING_STAT_EXCLUSIVE(TokenMovement);

    // Consume PendingForce and reset to zero.
    // At this point, any calls to AddForce() will apply to the next frame.
    PendingForceThisUpdate = PendingForce;
    ClearPendingForce();

    // Skip if don't want component updated when not rendered or updated component can't move
    if ((HasStoppedSimulation()) || (ShouldSkipUpdate(DeltaSeconds)))
    {
        return;
    }

    Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

    if ((!IsValid(UpdatedComponent)) || (!bSimulationEnabled))
    {
        return;
    }

    AActor *ActorOwner = UpdatedComponent->GetOwner();
    if ((!ActorOwner) || (!CheckStillInWorld()))
    {
        return;
    }

    if (UpdatedComponent->IsSimulatingPhysics())
    {
        return;
    }

    FHitResult Hit(1.f);
    if ((bSimulationEnabled) && (IsValid(ActorOwner)) && (!HasStoppedSimulation()))
    {
        // Initial move state
        Hit.Time = 1.f;
        const FVector OldVelocity = Velocity;
        const FVector MoveDelta = ComputeMoveDelta(OldVelocity, DeltaSeconds);
        const FQuat NewRotation = MoveDelta.ToOrientationQuat();

        // Move the component
        if (bShouldBounce)
        {
            // If it's allowed to bounce, we are allowed to move out of penetrations, so use SafeMoveUpdatedComponent
            // which does that automatically
            SafeMoveUpdatedComponent(MoveDelta, NewRotation, bSweepCollision, Hit);
        }
        else
        {
            // If it's forbidden bounce, then don't adjust if initially penetrating, because that should be a blocking
            // hit that causes a hit event and stop simulation
            TGuardValue<EMoveComponentFlags> ScopedFlagRestore(MoveComponentFlags,
                MoveComponentFlags | MOVECOMP_NeverIgnoreBlockingOverlaps);
            MoveUpdatedComponent(MoveDelta, NewRotation, bSweepCollision, &Hit);
        }

        // If something has destroyed the component -- abort
        if (!IsValid(ActorOwner) || HasStoppedSimulation())
        {
            return;
        }

        // Handle hit result after movement
        if (!Hit.bBlockingHit)
        {
            PreviousHitTime = 1.f;
            bIsSliding = false;

            // Calculate new velocity if events didn't change it during the movement update
            if (Velocity == OldVelocity)
            {
                Velocity = ComputeVelocity(Velocity, DeltaSeconds);
            }
        }
        else
        {
            // Calculate new velocity if events didn't change it during the movement update
            if (Velocity == OldVelocity)
            {
                // Re-calculate end velocity for partial time
                Velocity = (Hit.Time > KINDA_SMALL_NUMBER) ?
                    (ComputeVelocity(OldVelocity, DeltaSeconds * Hit.Time)) : (OldVelocity);
            }

            // Handle blocking hit
            float SubTickTimeRemaining = DeltaSeconds * (1.f - Hit.Time);
            const EHandleBlockingHitResult HandleBlockingResult =
                HandleBlockingHit(Hit, DeltaSeconds, MoveDelta, SubTickTimeRemaining);
            
            if ((HandleBlockingResult == EHandleBlockingHitResult::Abort) || (HasStoppedSimulation()))
            {
                // Ignore
            }
            else if (HandleBlockingResult == EHandleBlockingHitResult::Deflect)
            {
                HandleDeflection(Hit, OldVelocity, SubTickTimeRemaining);
                PreviousHitTime = Hit.Time;
                PreviousHitNormal = ConstrainNormalToPlane(Hit.Normal);
            }
            else if (HandleBlockingResult == EHandleBlockingHitResult::AdvanceNextSubstep)
            {
                // Reset deflection logic to ignore this hit
                PreviousHitTime = 1.f;
            }
            else
            {
                // Unhandled EHandleBlockingHitResult
                checkNoEntry();
            }
        }
    }

    UpdateComponentVelocity();
    Direction = Velocity.GetSafeNormal();
}

UMTD_TokenMovementComponent::EHandleBlockingHitResult UMTD_TokenMovementComponent::HandleBlockingHit(
    const FHitResult &Hit, float TimeTick, const FVector &MoveDelta, float &SubTickTimeRemaining)
{
    const AActor *ActorOwner = (IsValid(UpdatedComponent) ? (UpdatedComponent->GetOwner()) : (nullptr));
    if ((!CheckStillInWorld()) || (!IsValid(ActorOwner)))
    {
        return EHandleBlockingHitResult::Abort;
    }

    HandleImpact(Hit, TimeTick, MoveDelta);

    if ((!IsValid(ActorOwner)) || (HasStoppedSimulation()))
    {
        return EHandleBlockingHitResult::Abort;
    }

    if (Hit.bStartPenetrating)
    {
        return EHandleBlockingHitResult::Abort;
    }

    SubTickTimeRemaining = TimeTick * (1.f - Hit.Time);
    return EHandleBlockingHitResult::Deflect;
}

void UMTD_TokenMovementComponent::HandleImpact(const FHitResult &Hit, float TimeSlice, const FVector &MoveDelta)
{
    bool bStopSimulating = false;

    if (bShouldBounce)
    {
        const FVector OldVelocity = Velocity;
        Velocity = ComputeBounceResult(Hit, TimeSlice, MoveDelta);

        // Broadcast the event
        OnTokenBounceDelegate.Broadcast(Hit, OldVelocity);

        // Event may modify velocity or threshold, so check velocity threshold now
        Velocity = LimitVelocity(Velocity);
        if (IsVelocityUnderSimulationThreshold())
        {
            bStopSimulating = true;
        }
    }
    else
    {
        bStopSimulating = true;
    }

    if (bStopSimulating)
    {
        StopSimulating(Hit);
    }
}

bool UMTD_TokenMovementComponent::HandleDeflection(FHitResult &Hit, const FVector &OldVelocity,
    float &SubTickTimeRemaining)
{
    const FVector Normal = ConstrainNormalToPlane(Hit.Normal);

    // Multiple hits within very short time period?
    const bool bMultiHit = (PreviousHitTime < 1.f) && (Hit.Time <= KINDA_SMALL_NUMBER);

    // If velocity still into wall (after HandleBlockingHit() had a chance to adjust), slide along wall
    constexpr float DotTolerance = 0.01f;
    bIsSliding = ((bMultiHit) && (FVector::Coincident(PreviousHitNormal, Normal))) ||
                 ((Velocity.GetSafeNormal() | Normal) <= DotTolerance);

    if (bIsSliding)
    {
        if ((bMultiHit) && ((PreviousHitNormal | Normal) <= 0.f))
        {
            // 90 degree or less corner, so use cross product for direction
            FVector NewDir = (Normal ^ PreviousHitNormal);
            NewDir = NewDir.GetSafeNormal();
            Velocity = Velocity.ProjectOnToNormal(NewDir);
            if ((OldVelocity | Velocity) < 0.f)
            {
                Velocity *= -1.f;
            }
            Velocity = ConstrainDirectionToPlane(Velocity);
        }
        else
        {
            // Adjust to move along new wall
            Velocity = ComputeSlideVector(Velocity, 1.f, Normal, Hit);
        }

        // Check min velocity
        if (IsVelocityUnderSimulationThreshold())
        {
            StopSimulating(Hit);
            return false;
        }

        // Velocity is now parallel to the impact surface
        if (SubTickTimeRemaining > KINDA_SMALL_NUMBER)
        {
            if (!HandleSliding(Hit, SubTickTimeRemaining))
            {
                return false;
            }
        }
    }

    return true;
}

bool UMTD_TokenMovementComponent::HandleSliding(FHitResult &Hit, float &SubTickTimeRemaining)
{
    FHitResult InitialHit(Hit);
    const FVector OldHitNormal = ConstrainDirectionToPlane(Hit.Normal);

    // Velocity is now parallel to the impact surface;
    // Perform the move now, before adding gravity/accel again, so we don't just keep hitting the surface
    SafeMoveUpdatedComponent(
        Velocity * SubTickTimeRemaining, UpdatedComponent->GetComponentQuat(), bSweepCollision, Hit);

    if (HasStoppedSimulation())
    {
        return false;
    }

    // A second hit can deflect the velocity (through the normal bounce code), for the next iteration
    if (Hit.bBlockingHit)
    {
        const float TimeTick = SubTickTimeRemaining;
        SubTickTimeRemaining = TimeTick * (1.f - Hit.Time);

        const EHandleBlockingHitResult HitResult =
            HandleBlockingHit(Hit, TimeTick, Velocity * TimeTick, SubTickTimeRemaining);
        
        if ((HitResult == EHandleBlockingHitResult::Abort) || (HasStoppedSimulation()))
        {
            return false;
        }
    }
    else
    {
        // Find velocity after elapsed time
        const FVector PostTickVelocity = ComputeVelocity(Velocity, SubTickTimeRemaining);

        // If pointing back into surface, apply friction and acceleration
        const FVector Force = PostTickVelocity - Velocity;
        const float ForceDotN = Force | OldHitNormal;
        if (ForceDotN < 0.f)
        {
            const FVector ProjectedForce = FVector::VectorPlaneProject(Force, OldHitNormal);
            const FVector NewVelocity = Velocity + ProjectedForce;

            const FVector FrictionForce =
                -NewVelocity.GetSafeNormal() * FMath::Min(-ForceDotN * Friction, NewVelocity.Size());
            Velocity = ConstrainDirectionToPlane(NewVelocity + FrictionForce);
        }
        else
        {
            Velocity = PostTickVelocity;
        }

        // Check min velocity
        if (IsVelocityUnderSimulationThreshold())
        {
            StopSimulating(InitialHit);
            return false;
        }

        SubTickTimeRemaining = 0.f;
    }

    return true;
}

FVector UMTD_TokenMovementComponent::ComputeBounceResult(const FHitResult &Hit, float TimeSlice,
    const FVector &MoveDelta)
{
    FVector TempVelocity = Velocity;
    const FVector Normal = ConstrainNormalToPlane(Hit.Normal);
    const float VDotNormal = (TempVelocity | Normal);

    // Only if velocity is opposed by normal or parallel
    if (VDotNormal <= 0.f)
    {
        // Project velocity onto normal in reflected direction.
        const FVector ProjectedNormal = Normal * -VDotNormal;

        // Point velocity in direction parallel to surface
        TempVelocity += ProjectedNormal;

        // Only tangential velocity should be affected by friction.
        const float ScaledFriction = (bBounceAngleAffectsFriction || bIsSliding) ?
            (FMath::Clamp(-VDotNormal / TempVelocity.Size(), MinFrictionFraction, 1.f) * Friction) : (Friction);
        TempVelocity *= FMath::Clamp(1.f - ScaledFriction, 0.f, 1.f);

        // Coefficient of restitution only applies perpendicular to impact.
        TempVelocity += (ProjectedNormal * FMath::Max(Bounciness, 0.f));

        // Bounciness could cause us to exceed max speed.
        TempVelocity = LimitVelocity(TempVelocity);
    }

    return TempVelocity;
}

FVector UMTD_TokenMovementComponent::ComputeMoveDelta(FVector InVelocity, float DeltaSeconds) const
{
    const FVector NewVelocity = ComputeVelocity(InVelocity, DeltaSeconds);
    const FVector MoveDelta = NewVelocity * DeltaSeconds;

    return MoveDelta;
}

FVector UMTD_TokenMovementComponent::ComputeVelocity(FVector InVelocity, float DeltaSeconds) const
{
    const FVector Acceleration = ComputeAcceleration() * DeltaSeconds;
    const FVector NewVelocity = InVelocity + Acceleration;
    const FVector LimitedVelocity = LimitVelocity(NewVelocity);

    FVector FinalVelocity = LimitedVelocity;
    if (HomingTargetComponent.IsValid())
    {
        const FVector RotatedVelocity = RotateTowards(FinalVelocity, HomingTargetComponent.Get(), DeltaSeconds);
        FinalVelocity = RotatedVelocity;
    }

    return FinalVelocity;
}

FVector UMTD_TokenMovementComponent::ComputeAcceleration() const
{
    FVector Acceleration = FVector::ZeroVector;
    Acceleration.Z += GetGravityZ() * GravityScale;
    Acceleration += PendingForceThisUpdate;

    if ((bIsHomingToken) && (HomingTargetComponent.IsValid()))
    {
        Acceleration += ComputeHomingAcceleration();
    }

    return Acceleration;
}

FVector UMTD_TokenMovementComponent::ComputeHomingAcceleration() const
{
    const FVector Acceleration = Direction * HomingAccelerationMagnitude;
    return Acceleration;
}

FVector UMTD_TokenMovementComponent::ComputeDistanceVectorTowards(const USceneComponent *Target) const
{
    const FVector OurPosition = UpdatedComponent->GetComponentLocation();
    const FVector TargetPosition = Target->GetComponentLocation();
    const FVector Distance = TargetPosition - OurPosition;

    return Distance;
}

FVector UMTD_TokenMovementComponent::ComputeDirectionTowards(const USceneComponent *Target) const
{
    const FVector Distance = ComputeDistanceVectorTowards(Target);
    const FVector Dir = Distance.GetSafeNormal();

    return Dir;
}

void DrawGizmo(const UWorld *World, FVector Base, FVector Vector, float Thickness)
{
    const FVector Forward = Vector.GetSafeNormal();
    const FVector Up = FVector::CrossProduct(Forward, FVector::RightVector).GetSafeNormal();
    const FVector Right = FVector::CrossProduct(Forward, Up).GetSafeNormal();

    constexpr float Length = 100.f;
    DrawDebugLine(World, Base, Base + Forward * Length, FColor::Red, false, -1.f, 0, Thickness);
    DrawDebugLine(World, Base, Base + Up * Length, FColor::Blue, false, -1.f, 0, Thickness);
    DrawDebugLine(World, Base, Base + Right * Length, FColor::Green, false, -1.f, 0, Thickness);
}

FVector UMTD_TokenMovementComponent::RotateTowards(FVector InVelocity, const USceneComponent *Target,
    float DeltaSeconds) const
{
    const FVector V0 = InVelocity;
    const FVector V1 = ComputeDistanceVectorTowards(Target);

    const float DR = RotationRate * DeltaSeconds;
    const FRotator R0 = V0.Rotation();
    const FRotator R1 = V1.Rotation();

    // MTD_WARN("V0 [%s] V1 [%s]", *V0.ToString(), *V1.ToString());

    FRotator R = R1 - R0;
    // MTD_WARN("R0 [%s] R1 [%s]", *R0.ToString(), *R1.ToString());
    // MTD_WARN("R [%s]", *R.ToString());

    R.Yaw = FMath::Sign(R.Yaw) * FMath::Min(DR, FMath::Abs(R.Yaw));
    R.Pitch = FMath::Sign(R.Pitch) * FMath::Min(DR, FMath::Abs(R.Pitch));

    // MTD_WARN("R' [%s]", *R.ToString());

    const FVector RotatedVector = R.RotateVector(V0);

    // MTD_WARN("Rotated vector [%s]", *RotatedVector.ToString());

    const UWorld *World = GetWorld();
    const FVector Base = UpdatedComponent->GetOwner()->GetActorLocation();
    DrawGizmo(World, Base, RotatedVector, 4.f);
    DrawGizmo(World, Base, V1, 2.f);

    return RotatedVector;
}

FVector UMTD_TokenMovementComponent::LimitVelocity(FVector NewVelocity) const
{
    const float CurrentMaxSpeed = GetMaxSpeed();
    if (CurrentMaxSpeed > 0.f)
    {
        NewVelocity = NewVelocity.GetClampedToMaxSize(CurrentMaxSpeed);
    }

    return ConstrainDirectionToPlane(NewVelocity);
}

void UMTD_TokenMovementComponent::StopSimulating(const FHitResult &HitResult)
{
    Velocity = PendingForce = PendingForceThisUpdate = FVector::ZeroVector;
    
    UpdateComponentVelocity();
    SetUpdatedComponent(nullptr);
    
    OnTokenStopDelegate.Broadcast(HitResult);
}

bool UMTD_TokenMovementComponent::CheckStillInWorld()
{
    if (!IsValid(UpdatedComponent))
    {
        return false;
    }

    const UWorld *MyWorld = GetWorld();
    if (!IsValid(MyWorld))
    {
        return false;
    }

    // Check the variations of KillZ
    const AWorldSettings *WorldSettings = MyWorld->GetWorldSettings(true);
    if (!WorldSettings->AreWorldBoundsChecksEnabled())
    {
        return true;
    }

    AActor *ActorOwner = UpdatedComponent->GetOwner();
    if (!IsValid(ActorOwner))
    {
        return false;
    }

    if (ActorOwner->GetActorLocation().Z < WorldSettings->KillZ)
    {
        const UDamageType *DmgType = (WorldSettings->KillZDamageType) ?
            (WorldSettings->KillZDamageType->GetDefaultObject<UDamageType>()) : (GetDefault<UDamageType>());

        ActorOwner->FellOutOfWorld(*DmgType);
        return false;
    }

    // Check if box has poked outside the world
    else if (UpdatedComponent->IsRegistered())
    {
        const FBox &Box = UpdatedComponent->Bounds.GetBox();
        if ((Box.Min.X < -HALF_WORLD_MAX) || (Box.Max.X > HALF_WORLD_MAX) ||
            (Box.Min.Y < -HALF_WORLD_MAX) || (Box.Max.Y > HALF_WORLD_MAX) ||
            (Box.Min.Z < -HALF_WORLD_MAX) || (Box.Max.Z > HALF_WORLD_MAX))
        {
            MTDS_WARN("[%s] is outside the world bounds.", *ActorOwner->GetName());
            ActorOwner->OutsideWorldBounds();

            // It's unsafe to use physics or collision at this point
            ActorOwner->SetActorEnableCollision(false);
            const FHitResult Hit(1.f);
            StopSimulating(Hit);

            return false;
        }
    }
    return true;
}
