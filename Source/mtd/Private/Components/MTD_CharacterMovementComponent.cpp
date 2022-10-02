#include "Components/MTD_CharacterMovementComponent.h"

#include "GameFramework/Character.h"

void UMTD_CharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	MaxWalkSpeed = MaxWalkSpeedBase;
}

void UMTD_CharacterMovementComponent::TickComponent(
	float DeltaSeconds,
	ELevelTick TickType,
	FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	const float DirAngle = GetMovementDirectionAngleImpl(); // 0.0 - 3.14
	const float DirAngleNorm = DirAngle * FMath::Sign(DirAngle); // 0.0 - 1.0
	const float Ratio = DirAngleNorm / DOUBLE_PI;
	const float MaxSpeedDiff = MaxWalkSpeedBase - MaxWalkSpeedBackwards;
	const float MaxSpeedEdit = MaxSpeedDiff * Ratio;
	MaxWalkSpeed = MaxWalkSpeedBase - MaxSpeedEdit;

	// UE_LOG(LogTemp, Warning, TEXT("Speed %f %f"), Velocity.Length(), MaxWalkSpeed);
}

float UMTD_CharacterMovementComponent::GetMovementDirectionAngle() const
{
	return FMath::RadiansToDegrees(DirectionRadians);
}

float UMTD_CharacterMovementComponent::GetMovementDirectionAngleImpl()
{
	if (Velocity.IsZero() || !IsValid(CharacterOwner.Get()))
		return 0.f;

	const FVector ActorForward = CharacterOwner->GetActorForwardVector();
	const FVector MovVectorNorm = Velocity.GetSafeNormal();

	const float AngleBetween =
		FMath::Acos(FVector::DotProduct(ActorForward, MovVectorNorm));
	const FVector CrossProduct =
		FVector::CrossProduct(ActorForward, MovVectorNorm);

	DirectionRadians = CrossProduct.IsZero() ?
		AngleBetween : AngleBetween * FMath::Sign(CrossProduct.Z);
	return DirectionRadians;
}
