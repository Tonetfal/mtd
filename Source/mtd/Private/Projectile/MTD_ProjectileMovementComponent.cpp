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
    const bool bHome = ((MovementParameters.bIsHoming) && (!IsValid(MovementParameters.HomingTarget.Get())));

    // Exit if our state won't change if we will do the rest of the function
    if ((Direction.IsZero()) && (bHome))
    {
        return;
    }

    // Accelerate if didn't hit the max speed
    const float AccelDelta = MovementParameters.Acceleration * DeltaSeconds;
    CurrentSpeed =
        FMath::Clamp(CurrentSpeed + AccelDelta, MovementParameters.InitialSpeed, MovementParameters.MaxSpeed);

    // TODO Rotate to the target direction depending on some offset
    if (bHome)
    {
        Velocity = ComputeVelocity(ComputeHomingDirection());
    }
    else
    {
        Velocity = ComputeVelocity(Direction);
    }

    const FVector MoveDelta = Velocity * DeltaSeconds;

    // TODO Make rotation follow the velocity

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

    return Dist / Dist.Length();
}

FVector UMTD_ProjectileMovementComponent::ComputeVelocity(FVector MovementDirection) const
{
    return MovementDirection * CurrentSpeed;
}

void UMTD_ProjectileMovementComponent::SetMovementParameters(FMTD_ProjectileMovementParameters Parms)
{
    MovementParameters = Parms;
    Direction = Parms.Direction;
}
