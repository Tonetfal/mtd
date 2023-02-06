#include "Player/MTD_TowerController.h"

#include "Character/MTD_Tower.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISightTargetInterface.h"

AMTD_TowerController::AMTD_TowerController()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    TeamComponent = CreateDefaultSubobject<UMTD_TeamComponent>(TEXT("MTD Team Component"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

    PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());

    bAttachToPawn = true;
    bWantsPlayerState = true;
}

void AMTD_TowerController::BeginPlay()
{
    Super::BeginPlay();
}

void AMTD_TowerController::OnPossess(APawn *InPawn)
{
    check(SightConfig);
    check(PerceptionComponent);

    Super::OnPossess(InPawn);
    
    // Cache before initing
    CacheTowerAttributes();
    InitConfig();

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->RequestStimuliListenerUpdate();

    auto Tower = CastChecked<AMTD_Tower>(InPawn);
    Tower->OnRangeAttributeChangedDelegate.AddDynamic(this, &ThisClass::UpdateSightAttributes);
}

AActor *AMTD_TowerController::GetFireTarget()
{
    if (!IsFireTargetStillVisible())
    {
        FireTarget = SearchForFireTarget();
    }
    return FireTarget;
}

void AMTD_TowerController::SetVisionRange(float Range)
{
    SightConfig->SightRadius = Range;
    SightConfig->LoseSightRadius = Range;

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->RequestStimuliListenerUpdate();
}

void AMTD_TowerController::SetPeripheralVisionHalfAngleDegrees(float Degrees)
{
    SightConfig->PeripheralVisionAngleDegrees = Degrees;

    PerceptionComponent->ConfigureSense(*SightConfig);
    PerceptionComponent->RequestStimuliListenerUpdate();
}

void AMTD_TowerController::UpdateSightAttributes()
{
    CacheTowerAttributes();
    SetVisionRange(SightRadius);
    SetPeripheralVisionHalfAngleDegrees(PeripheralVisionHalfAngleDegrees);
}

bool AMTD_TowerController::IsFireTargetStillVisible() const
{
    if ((!IsValid(FireTarget)) || (!IsValid(GetPawn())))
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

AActor *AMTD_TowerController::SearchForFireTarget()
{
    TArray<AActor *> PerceivedActors;
    PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

    return FindClosestActor(PerceivedActors);
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
        MTDS_WARN("We are dangling");
        return nullptr;
    }

    const FVector OutLoc = OurPawn->GetActorLocation();
    const float OurLen = OutLoc.Length();

    AActor *ClosestActor = Actors[0];
    float ClosestDistance = FMath::Abs(OurLen - ClosestActor->GetActorLocation().Length());

    for (AActor *Actor :Actors)
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

void AMTD_TowerController::InitConfig()
{
    check(SightConfig);

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
