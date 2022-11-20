#include "Projectile/MTD_ProjectileMovementComponent.h"

void UMTD_ProjectileMovementComponent::TickComponent(
    float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

    if (!IsValid(UpdatedComponent))
    {
        return;
    }

    const AActor *ActorOwner = UpdatedComponent->GetOwner();
    if (!ActorOwner || !CheckStillInWorld())
    {
        return;
    }

    if (UpdatedComponent->IsSimulatingPhysics())
    {
        return;
    }

    // Should be homing
    const bool bHome = ((MovementParameters.bIsHoming) && (IsValid(MovementParameters.HomingTarget.Get())));

    // Exit if our state won't change if we will do the rest of the function
    if ((Direction.IsZero()) && (bHome))
    {
        return;
    }

    // Accelerate if didn't hit the max speed
    const float AccelDelta = MovementParameters.Acceleration * DeltaSeconds;
    CurrentSpeed =
        FMath::Clamp(CurrentSpeed + AccelDelta, MovementParameters.InitialSpeed, MovementParameters.MaxSpeed);

    if (bHome)
    {
        // Perform the smooth rotation
        if (MovementParameters.RotationRate != 0.f)
        {
            const FVector DirectionToTarget = ComputeHomingDirection();

            // Old
            // const FRotator Rotator = ComputeRotator(DirectionToTarget);
            // const FVector RotatedDirectionToTarget = Rotator.RotateVector(DirectionToTarget);

            // New
            const FVector RotatedDirectionToTarget = GetRotatedDirection(DirectionToTarget, DeltaSeconds);
            
            Direction = RotatedDirectionToTarget;
        }
    }
    
    Velocity = ComputeVelocity(Direction);
    const FVector MoveDelta = Velocity * DeltaSeconds;

    UpdatedComponent->SetWorldLocation(UpdatedComponent->GetComponentLocation() + MoveDelta);
}

bool UMTD_ProjectileMovementComponent::CheckStillInWorld()
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
        const UDamageType *DmgType = (WorldSettings->KillZDamageType)
                                         ? (WorldSettings->KillZDamageType->GetDefaultObject<UDamageType>())
                                         : (GetDefault<UDamageType>());
        ActorOwner->FellOutOfWorld(*DmgType);
        return false;
    }
    // Check if box has poked outside the world
    if ((UpdatedComponent) && (UpdatedComponent->IsRegistered()))
    {
        const FBox &Box = UpdatedComponent->Bounds.GetBox();
        if (Box.Min.X < -HALF_WORLD_MAX || Box.Max.X > HALF_WORLD_MAX ||
            Box.Min.Y < -HALF_WORLD_MAX || Box.Max.Y > HALF_WORLD_MAX ||
            Box.Min.Z < -HALF_WORLD_MAX || Box.Max.Z > HALF_WORLD_MAX)
        {
            MTDS_WARN("[%s] is outside the world bounds!", *ActorOwner->GetName());
            ActorOwner->OutsideWorldBounds();

            // It is not safe to use physics or collision at this point
            ActorOwner->SetActorEnableCollision(false);
            return false;
        }
    }
    return true;
}

FVector UMTD_ProjectileMovementComponent::ComputeHomingDirection() const
{
    const FVector TargetLoc = MovementParameters.HomingTarget->GetActorLocation();
    const FVector OurLoc = UpdatedComponent->GetComponentLocation();
    const FVector Dist = TargetLoc - OurLoc;

    return Dist.GetSafeNormal();
}

FVector UMTD_ProjectileMovementComponent::ComputeVelocity(FVector MovementDirection) const
{
    return MovementDirection * CurrentSpeed;
}

FRotator UMTD_ProjectileMovementComponent::ComputeRotator(FVector DirectionToTarget) const
{
    // Shorthand the expressions
    const FVector P0 = Direction;
    const FVector P1 = DirectionToTarget;
    const float RotAngle = MovementParameters.RotationRate;

    const FVector2D P0XY(P0.X, P0.Y);
    const FVector2D P0XZ(P0.X, P0.Z);
    const FVector2D P0YZ(P0.Y, P0.Z);

    const FVector2D P1XY(P1.X, P1.Y);
    const FVector2D P1XZ(P1.X, P1.Z);
    const FVector2D P1YZ(P1.Y, P1.Z);

    const float DotXY = FVector2D::DotProduct(P0XY, P1XY);
    const float DotXZ = FVector2D::DotProduct(P0XZ, P1XZ);
    const float DotYZ = FVector2D::DotProduct(P0YZ, P1YZ);

    // Compute angles between each invidual axis
    const float AngleXY = FMath::Acos(DotXY);
    const float AngleXZ = FMath::Acos(DotXZ);
    const float AngleYZ = FMath::Acos(DotYZ);

    // Cache signs to apply them to make angles abs, and then apply the sign on the gotten value
    const float SignAngleXY = FMath::Sign(AngleXY);
    const float SignAngleXZ = FMath::Sign(AngleXZ);
    const float SignAngleYZ = FMath::Sign(AngleYZ);
    
    // Make sure to don't over rotate to the other side
    const float RotAngleXY = SignAngleXY * FMath::Min(AngleXY * SignAngleXY, RotAngle);
    const float RotAngleXZ = SignAngleXZ * FMath::Min(AngleXZ * SignAngleXZ, RotAngle);
    const float RotAngleYZ = SignAngleYZ * FMath::Min(AngleYZ * SignAngleYZ, RotAngle);

    return FRotator(RotAngleXY, RotAngleXZ, RotAngleYZ);
}

FVector UMTD_ProjectileMovementComponent::GetRotatedDirection(FVector DirectionToTarget, float DeltaSeconds) const
{
    // Shorthand the expressions
    const FVector P0 = Direction;
    const FVector P1 = DirectionToTarget;
    const float RotAngle = MovementParameters.RotationRate * DeltaSeconds;

    const FVector Dist = Direction - DirectionToTarget;
    const FVector DistOppSigns = Dist.GetSignVector();
    
    const FVector RotTowards(DistOppSigns * RotAngle);
    const FRotator Rotator = RotTowards.Rotation();

    const FVector P2 = Rotator.RotateVector(P0);
    const FVector Result
    (
        (DistOppSigns.X == 1) ? (FMath::Min(P1.X, P2.X)) : (FMath::Max(P1.X, P2.X)),
        (DistOppSigns.Y == 1) ? (FMath::Min(P1.Y, P2.Y)) : (FMath::Max(P1.Y, P2.Y)),
        (DistOppSigns.Z == 1) ? (FMath::Min(P1.Z, P2.Z)) : (FMath::Max(P1.Z, P2.Z))
    );

    return Result;
}

void UMTD_ProjectileMovementComponent::SetMovementParameters(FMTD_ProjectileMovementParameters Parms)
{
    MovementParameters = Parms;
    Direction = Parms.Direction;
}
