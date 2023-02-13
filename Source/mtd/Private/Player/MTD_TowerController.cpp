#include "Player/MTD_TowerController.h"

#include "Character/MTD_Tower.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISightTargetInterface.h"

AMTD_TowerController::AMTD_TowerController()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    TeamComponent = CreateDefaultSubobject<UMTD_TeamComponent>("MTD Team Component");
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");
    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception Component");

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

    // Follow tower so others can see our team
    bAttachToPawn = true;

    // Tower makes use of gameplay ability system, so PS is required since it contains it
    bWantsPlayerState = true;
}

void AMTD_TowerController::OnPossess(APawn *InPawn)
{
    check(IsValid(SightConfig));
    check(IsValid(PerceptionComponent));

    Super::OnPossess(InPawn);
    
    // Cache before initing
    CacheTowerAttributes();
    InitSightConfig();

    // Re-configure the perception component with new values, and update the stimuli
    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->RequestStimuliListenerUpdate();

    auto Tower = CastChecked<AMTD_Tower>(InPawn);
    Tower->OnRangeAttributeChangedDelegate.AddDynamic(this, &ThisClass::UpdateSightAttributes);
}

void AMTD_TowerController::InitSightConfig()
{
    check(IsValid(SightConfig));

    SightConfig->Implementation = UAISense_Sight::StaticClass();
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
}

void AMTD_TowerController::CacheTowerAttributes()
{
    auto Tower = CastChecked<AMTD_Tower>(GetPawn());

    SightRadius = Tower->GetScaledVisionRange();
    PeripheralVisionHalfAngleDegrees = Tower->GetScaledVisionHalfDegrees();
}

AActor *AMTD_TowerController::GetFireTarget()
{
    if (!IsFireTargetVisible())
    {
        FireTarget = SearchForFireTarget();
    }
    return FireTarget;
}

void AMTD_TowerController::SetVisionRange(float Range)
{
    if (SightConfig->SightRadius == Range)
    {
        return;
    }
    
    SightConfig->SightRadius = Range;
    SightConfig->LoseSightRadius = Range;

    // Re-configure the perception component with new values, and update the stimuli
    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->RequestStimuliListenerUpdate();
}

void AMTD_TowerController::SetPeripheralVisionHalfAngleDegrees(float HalfDegrees)
{
    if (SightConfig->PeripheralVisionAngleDegrees == HalfDegrees)
    {
        return;
    }
    
    SightConfig->PeripheralVisionAngleDegrees = HalfDegrees;
    
    // Re-configure the perception component with new values, and update the stimuli
    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->RequestStimuliListenerUpdate();
}

void AMTD_TowerController::UpdateSightAttributes()
{
    CacheTowerAttributes();
    SetVisionRange(SightRadius);
    SetPeripheralVisionHalfAngleDegrees(PeripheralVisionHalfAngleDegrees);
}

bool AMTD_TowerController::IsFireTargetVisible() const
{
    if (!((IsValid(FireTarget)) && (IsValid(GetPawn()))))
    {
        return false;
    }

    const auto SightTarget = Cast<IAISightTargetInterface>(FireTarget);
    if (!SightTarget)
    {
        return false;
    }

    FVector SeenLocation;
    int32 NumberOfLoSChecksPerformed = 0;
    float SightStrength;

    const bool bCanBeSeen = SightTarget->CanBeSeenFrom(
        GetPawn()->GetPawnViewLocation(),
        SeenLocation,
        NumberOfLoSChecksPerformed,
        SightStrength,
        GetPawn(), nullptr, nullptr);

    if (!bCanBeSeen)
    {
        MTDS_VVERBOSE("[%s] is not seen anymore", *FireTarget->GetName());
    }

    return bCanBeSeen;
}

AActor *AMTD_TowerController::SearchForFireTarget() const
{
    TArray<AActor *> PerceivedActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

    AActor *ClosestActor = FindClosestActor(PerceivedActors);
    return ClosestActor;
}

AActor *AMTD_TowerController::FindClosestActor(const TArray<AActor *> &Actors) const
{
    if (Actors.IsEmpty())
    {
        return nullptr;
    }

    const AActor *OurPawn = GetPawn();
    if (!IsValid(OurPawn))
    {
        return nullptr;
    }

    const FVector OurPosition = OurPawn->GetActorLocation();
    AActor *ClosestActor = nullptr;
    float ClosestDistanceSq = 0.f;

    // Iterate through all actors, and check distance between our tower and current actor 
    for (AActor *Actor : Actors)
    {
        const FVector OtherPosition = Actor->GetActorLocation();
        const FVector DistanceVector = (OurPosition - OtherPosition);
        const float DistanceSq = DistanceVector.SquaredLength();

        if (((!ClosestActor) || (ClosestDistanceSq > DistanceSq)))
        {
            ClosestDistanceSq = DistanceSq;
            ClosestActor = Actor;
        }
    }

    return ClosestActor;
}
