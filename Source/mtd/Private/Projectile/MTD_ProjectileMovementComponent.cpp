#include "Projectile/MTD_ProjectileMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

UMTD_ProjectileMovementComponent::UMTD_ProjectileMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    bUpdateOnlyIfRendered = false;

    bWantsInitializeComponent = true;
    bComponentShouldUpdatePhysicsVolume = false;
}

void UMTD_ProjectileMovementComponent::TickComponent(float DeltaSeconds, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

    if (!IsValid(UpdatedComponent))
    {
        return;
    }

    const AActor *ActorOwner = UpdatedComponent->GetOwner();
    if ((!ActorOwner) || (!CheckStillInWorld()))
    {
        return;
    }

    if (UpdatedComponent->IsSimulatingPhysics())
    {
        return;
    }

    PendingAccelerationThisUpdate = PendingAcceleration;
    ClearAcceleration();

    const FVector MoveDelta = ComputeMoveDelta(DeltaSeconds);
    const FQuat DesiredRotation = Direction.ToOrientationQuat();

    FHitResult Hit;
    SafeMoveUpdatedComponent(MoveDelta, DesiredRotation, false, Hit);
    UpdateComponentVelocity();
}

void UMTD_ProjectileMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();

    CurrentSpeed = InitialSpeed;
}

FVector UMTD_ProjectileMovementComponent::ComputeMoveDelta(float DeltaSeconds)
{
    const bool bHome = ((bIsHoming) && (HomingTarget.IsValid()));

    // Exit if our state won't change if we will do the rest of the function
    if ((Direction.IsZero()) && (!bHome))
    {
        return FVector::ZeroVector;
    }

    Accelerate(DeltaSeconds);

    if (bHome)
    {
        RotateTowardsHoming(DeltaSeconds);
    }
    
    ComputeVelocity();
    const FVector MoveDelta = Velocity * DeltaSeconds;

    return MoveDelta;
}

void UMTD_ProjectileMovementComponent::Accelerate(float DeltaSeconds)
{
    const float AccelDelta = Acceleration * DeltaSeconds;
    const float NewSpeed = CurrentSpeed + AccelDelta + PendingAccelerationThisUpdate;
    
    SetSpeed(NewSpeed);
}

void UMTD_ProjectileMovementComponent::RotateTowardsHoming(float DeltaSeconds)
{
    if (RotationRate == 0.f)
    {
        return;
    }
    
    const FVector DirectionToTarget = GetHomingDirection();
    
    // TODO: Tmp
    Direction = DirectionToTarget;

    return;

    // If the projectile is not moving, then make it face the target
    if (Direction.IsZero())
    {
        Direction = DirectionToTarget;
    }
    else
    {
        const FVector V0 = Direction;
        const FVector V1 = GetHomingDirection();

        const float DR = RotationRate * DeltaSeconds;
        const FRotator R0 = V0.Rotation();
        const FRotator R1 = V1.Rotation();

        FRotator R = R1 - R0;
        R.Yaw = FMath::Sign(R.Yaw) * FMath::Min(DR, FMath::Abs(R.Yaw));
        R.Pitch = FMath::Sign(R.Pitch) * FMath::Min(DR, FMath::Abs(R.Pitch));

        Direction = R.RotateVector(V0);
    }
}

FVector UMTD_ProjectileMovementComponent::GetHomingDistanceVector() const
{
    const FVector TargetPos = HomingTarget->GetActorLocation();
    const FVector OurPos = UpdatedComponent->GetComponentLocation();
    const FVector DistanceVector = TargetPos - OurPos;

    return DistanceVector;
}

FVector UMTD_ProjectileMovementComponent::GetHomingDirection() const
{
    const FVector DistanceVector = GetHomingDistanceVector();
    const FVector HomingDirection = DistanceVector.GetSafeNormal();

    return HomingDirection;
}

bool UMTD_ProjectileMovementComponent::CheckStillInWorld() const
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
            return false;
        }
    }
    return true;
}

