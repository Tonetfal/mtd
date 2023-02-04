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
    if (((!ActorOwner) || (!CheckStillInWorld())))
    {
        return;
    }

    if (UpdatedComponent->IsSimulatingPhysics())
    {
        return;
    }

    PendingAccelerationThisUpdate = PendingAcceleration;
    ClearAcceleration();
    
    const FVector MoveDelta = ComputeMoveDelta(Velocity, DeltaSeconds);
    const FQuat DesiredRotation = Direction.ToOrientationQuat();
    
    FHitResult HitResult;
    SafeMoveUpdatedComponent(MoveDelta, DesiredRotation, bSweep, HitResult);
    UpdateComponentVelocity();
}

void UMTD_ProjectileMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();

    Velocity = (Direction * InitialSpeed);
}

FVector UMTD_ProjectileMovementComponent::ComputeMoveDelta(FVector InVelocity, float DeltaSeconds) const
{
    const FVector NewVelocity = ComputeVelocity(InVelocity, DeltaSeconds);
    const FVector MoveDelta = NewVelocity * DeltaSeconds;

    return MoveDelta;
}

FVector UMTD_ProjectileMovementComponent::ComputeVelocity(FVector InVelocity, float DeltaSeconds) const
{
    const FVector NewVelocity = (Velocity + PendingAccelerationThisUpdate);
    const FVector LimitedVelocity = LimitVelocity(NewVelocity);

    FVector FinalVelocity = LimitedVelocity;
    if (((bIsHoming) && (HomingTargetComponent.IsValid())))
    {
        const FVector RotatedVelocity = RotateTowards(FinalVelocity, HomingTargetComponent.Get(), DeltaSeconds);
        FinalVelocity = RotatedVelocity;
    }

    return FinalVelocity;
}

FVector UMTD_ProjectileMovementComponent::ComputeDistanceVectorTowards(const USceneComponent *Target) const
{
    const FVector OurPosition = UpdatedComponent->GetComponentLocation();
    const FVector TargetPosition = Target->GetComponentLocation();
    const FVector Distance = (TargetPosition - OurPosition);

    return Distance;
}

FVector UMTD_ProjectileMovementComponent::ComputeDirectionTowards(const USceneComponent *Target) const
{
    const FVector Distance = ComputeDistanceVectorTowards(Target);
    const FVector Dir = Distance.GetSafeNormal();

    return Dir;
}

FVector UMTD_ProjectileMovementComponent::RotateTowards(FVector InVelocity, const USceneComponent *Target,
    float DeltaSeconds) const
{
    const FVector V0 = InVelocity;
    const FVector V1 = ComputeDistanceVectorTowards(Target);

    const float DR = (RotationRate * DeltaSeconds);
    const FRotator R0 = V0.Rotation().Clamp();
    const FRotator R1 = V1.Rotation().Clamp();

    const float Len = V0.Length();

    FRotator R = (R1 - R0);
    
    if (FMath::Abs(R.Yaw) > 180.f)
    {
        const float Sign = FMath::Sign(R.Yaw);
        R.Yaw -= (Sign * 180.f);
        R.Yaw *= -1.f;
    }
    
    if (FMath::Abs(R.Pitch) > 180.f)
    {
        const float Sign = FMath::Sign(R.Pitch);
        R.Pitch -= (Sign * 180.f);
        R.Pitch *= -1.f;
    }
    
    R.Yaw = (FMath::Sign(R.Yaw) * FMath::Min(DR, FMath::Abs(R.Yaw)));
    R.Pitch = (FMath::Sign(R.Pitch) * FMath::Min(DR, FMath::Abs(R.Pitch)));

    const FRotator R2 = (R0 + R);
    const FVector RotatedVector = (Len * R2.Vector());
    
    return RotatedVector;
}

FVector UMTD_ProjectileMovementComponent::LimitVelocity(FVector NewVelocity) const
{
    const float CurrentMaxSpeed = GetMaxSpeed();
    if (CurrentMaxSpeed > 0.f)
    {
        NewVelocity = NewVelocity.GetClampedToMaxSize(CurrentMaxSpeed);
    }

    return ConstrainDirectionToPlane(NewVelocity);
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
