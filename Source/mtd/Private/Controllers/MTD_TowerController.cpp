#include "Controllers/MTD_TowerController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISightTargetInterface.h"

AMTD_TowerController::AMTD_TowerController()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Team =
		CreateDefaultSubobject<UMTD_TeamComponent>(TEXT("MTD Team Component"));
	
	SightConfig = 
    	CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));		
    	
	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees =
		PeripheralVisionHalfAngleDegrees;
	SightConfig->DetectionByAffiliation = DetectionByAffiliation;
	SightConfig->AutoSuccessRangeFromLastSeenLocation =
		AutoSuccessRangeFromLastSeenLocation;
	SightConfig->PointOfViewBackwardOffset = PointOfViewBackwardOffset;
	SightConfig->NearClippingRadius = NearClippingRadius;
	SightConfig->SetMaxAge(MaxAge);
	
	PerceptionComponent =
		CreateDefaultSubobject<UAIPerceptionComponent>(
			TEXT("Perception Component"));

	PerceptionComponent->ConfigureSense(*SightConfig);
}

void AMTD_TowerController::BeginPlay()
{
	Super::BeginPlay();

	check(SightConfig);
	check(PerceptionComponent);
	
	PerceptionComponent->SetDominantSense(
		*SightConfig->GetSenseImplementation());
	PerceptionComponent->ConfigureSense(*SightConfig);
}

AActor *AMTD_TowerController::GetFireTarget()
{
	if (!IsFireTargetStillVisible())
	{
		MTDS_WARN("Fire target is not seen anymore, searching for a new one");
		FireTarget = SearchForFireTarget();
	}
	return FireTarget;
}

void AMTD_TowerController::SetVisionRange(float Range)
{
	SightConfig->SightRadius = Range;
	SightConfig->LoseSightRadius = Range;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = Range;
	
	PerceptionComponent->ConfigureSense(*SightConfig);
}

void AMTD_TowerController::SetPeripheralVisionHalfAngleDegrees(float Degrees)
{
	SightConfig->PeripheralVisionAngleDegrees = Degrees;
	
	PerceptionComponent->ConfigureSense(*SightConfig);
}

bool AMTD_TowerController::IsFireTargetStillVisible() const
{
	if (!IsValid(FireTarget) || !IsValid(GetPawn()))
		return false;

	const auto *FirePawn = Cast<APawn>(FireTarget);
	if (!FirePawn)
		return false;

	const AController *TargetController = FirePawn->GetController();
	if (!IsValid(TargetController))
		return false;

	const auto SightTarget = Cast<IAISightTargetInterface>(TargetController);
	if (!SightTarget)
		return false;

	FVector SeenLocation;
	int32 NumberOfLoSChecksPerformed = 0;
	float SightStrength;
	
	const bool bCanBeSeen = SightTarget->CanBeSeenFrom(
		GetPawn()->GetPawnViewLocation(),
		SeenLocation,
		NumberOfLoSChecksPerformed,
		SightStrength, 
		GetPawn(), nullptr, nullptr);
		
	return bCanBeSeen;
}

AActor *AMTD_TowerController::SearchForFireTarget()
{
	TArray<AActor*> PerceivedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(
		UAISense_Sight::StaticClass(), PerceivedActors);

	PerceptionComponent->RequestStimuliListenerUpdate();

	MTDS_WARN("Get currently perceived actors num %d", PerceivedActors.Num());

	const AActor *ClosestActor = FindClosestActor(PerceivedActors);
	if (!ClosestActor)
		return nullptr;

	const AController *ActorController = Cast<AController>(ClosestActor);
	if (!IsValid(ActorController))
	{
		MTD_WARN("%s is not a controller", *ClosestActor->GetName());
		return nullptr;
	}
	
	return ActorController->GetPawn();
}

AActor *AMTD_TowerController::FindClosestActor(
	const TArray<AActor*> &Actors) const
{
	if (Actors.IsEmpty())
		return nullptr;

	const AActor *OurPawn = GetPawn();
	if (!IsValid(OurPawn))
	{
		MTD_WARN("%s is dangling", *GetName());
		return nullptr;
	}
	
	const FVector OutLoc = OurPawn->GetActorLocation();
	const float OurLen = OutLoc.Length();
	
	AActor *ClosestActor = Actors[0];
	float ClosestDistance =
		FMath::Abs(OurLen - ClosestActor->GetActorLocation().Length());
	
	for (AActor *Actor : Actors)
	{
		const FVector OtherLoc = Actor->GetActorLocation();
		const float OtherLen = OtherLoc.Length();
		const float Distance = FMath::Abs(OurLen - OtherLen);
		
		if (ClosestDistance > Distance)
		{
			ClosestDistance = Distance;
			ClosestActor = Actor;
		}
	}
	
	return ClosestActor;
}
