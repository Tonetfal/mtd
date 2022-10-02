#include "Projectiles/MTD_ProjectileMovementComponent.h"

void UMTD_ProjectileMovementComponent::TickComponent(
	float DeltaSeconds,
	ELevelTick TickType, 
	FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	if (!IsValid(UpdatedComponent))
		return;
	
	const AActor* ActorOwner = UpdatedComponent->GetOwner();
	if (!ActorOwner || !CheckStillInWorld())
		return;

	if (UpdatedComponent->IsSimulatingPhysics())
		return;

	if (Direction.IsZero() && !IsValid(MovementParameters.HomingTarget.Get()))
		return;

	const float AccelDelta = MovementParameters.Acceleration * DeltaSeconds;
	CurrentSpeed = FMath::Clamp(CurrentSpeed + AccelDelta,
		MovementParameters.InitialSpeed, MovementParameters.MaxSpeed);

	// TODO Rotate to the target direction depending on some offset
	if (!IsValid(MovementParameters.HomingTarget.Get()))
	{
		Velocity = ComputeVelocity(Direction);
	}
	else
	{
		Velocity = ComputeVelocity(ComputeHomingDirection());
	}
	
	const FVector MoveDelta = Velocity * DeltaSeconds;

	if (MovementParameters.bRotationFollowsVelocity)
	{
		// TODO Rotation
	}

	UpdatedComponent->SetWorldLocation(
		UpdatedComponent->GetComponentLocation() + MoveDelta);
}

bool UMTD_ProjectileMovementComponent::CheckStillInWorld()
{
	if (!IsValid(UpdatedComponent))
		return false;

	const UWorld *MyWorld = GetWorld();
	if (!IsValid(MyWorld))
		return false;

	// Check the variations of KillZ
	const AWorldSettings *WorldSettings = MyWorld->GetWorldSettings(true);
	if (!WorldSettings->AreWorldBoundsChecksEnabled())
		return true;
	
	AActor *ActorOwner = UpdatedComponent->GetOwner();
	if (!IsValid(ActorOwner))
		return false;
	
	if (ActorOwner->GetActorLocation().Z < WorldSettings->KillZ)
	{
		const UDamageType *DmgType = WorldSettings->KillZDamageType ? 
			WorldSettings->KillZDamageType->GetDefaultObject<UDamageType>() :
			GetDefault<UDamageType>();
		ActorOwner->FellOutOfWorld(*DmgType);
		return false;
	}
	// Check if box has poked outside the world
	else if( UpdatedComponent && UpdatedComponent->IsRegistered() )
	{
		const FBox&	Box = UpdatedComponent->Bounds.GetBox();
		if (Box.Min.X < -HALF_WORLD_MAX || Box.Max.X > HALF_WORLD_MAX ||
			Box.Min.Y < -HALF_WORLD_MAX || Box.Max.Y > HALF_WORLD_MAX ||
			Box.Min.Z < -HALF_WORLD_MAX || Box.Max.Z > HALF_WORLD_MAX)
		{
			MTD_WARN("%s is outside the world bounds!", *ActorOwner->GetName());
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
	const FVector TargetLoc =
		MovementParameters.HomingTarget->GetComponentLocation();
	const FVector OurLoc = UpdatedComponent->GetComponentLocation();
	const FVector Dist = TargetLoc - OurLoc;
	
	return Dist / Dist.Length();
}

FVector UMTD_ProjectileMovementComponent::ComputeVelocity(
	FVector MovementDirection) const
{
	return MovementDirection * CurrentSpeed;
}

void UMTD_ProjectileMovementComponent::SetInitialSpeed(float Value)
{
	MovementParameters.InitialSpeed = Value;
}

void UMTD_ProjectileMovementComponent::SetMaxSpeed(float Value)
{
	MovementParameters.MaxSpeed = Value;
}

void UMTD_ProjectileMovementComponent::SetAcceleration(float Value)
{
	MovementParameters.Acceleration = Value;
}

void UMTD_ProjectileMovementComponent::SetHomingTarget(
	TWeakObjectPtr<USceneComponent> Value)
{
	MovementParameters.HomingTarget = Value;
}

void UMTD_ProjectileMovementComponent::SetDirection(FVector Value)
{
	Direction = Value;
}

void UMTD_ProjectileMovementComponent::SetRotationFollowsVelocity(bool Value)
{
	MovementParameters.bRotationFollowsVelocity = Value;
}

void UMTD_ProjectileMovementComponent::SetMovementParameters(
	FMTD_ProjectileParameters Parms)
{
	MovementParameters = Parms;
}

float UMTD_ProjectileMovementComponent::GetInitialSpeed() const
{
	return MovementParameters.InitialSpeed;
}

float UMTD_ProjectileMovementComponent::GetMaxSpeed() const
{
	return MovementParameters.MaxSpeed;
}

float UMTD_ProjectileMovementComponent::GetAcceleration() const
{
	return MovementParameters.Acceleration;
}

TWeakObjectPtr<USceneComponent>
UMTD_ProjectileMovementComponent::GetHomingTarget() const
{
	return MovementParameters.HomingTarget;
}

FVector UMTD_ProjectileMovementComponent::GetDirection() const
{
	return Direction;
}

bool UMTD_ProjectileMovementComponent::GetRotationFollowsVelocity() const
{
	return MovementParameters.bRotationFollowsVelocity;
}

FMTD_ProjectileParameters
UMTD_ProjectileMovementComponent::GetMovementParameters() const
{
	return MovementParameters;
}
