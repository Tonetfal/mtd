#include "Items/MTD_FloatingToken.h"

#include "Components/SphereComponent.h"
#include "Items/MTD_TokenMovementComponent.h"

AMTD_FloatingToken::AMTD_FloatingToken()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
    PrimaryActorTick.TickInterval = 0.1f; // Don't run it too often

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
    SetRootComponent(CollisionComponent);
    CollisionComponent->SetSphereRadius(25.f);

    CollisionComponent->SetCollisionProfileName(FloatingTokenCollisionProfileName);
    CollisionComponent->SetGenerateOverlapEvents(false);
    CollisionComponent->SetCanEverAffectNavigation(false);

    ActivationTriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Activation Trigger Component"));
    ActivationTriggerComponent->SetupAttachment(GetRootComponent());
    ActivationTriggerComponent->SetSphereRadius(50.f);

    ActivationTriggerComponent->SetGenerateOverlapEvents(true);
    ActivationTriggerComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    ActivationTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ActivationTriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    ActivationTriggerComponent->SetCollisionResponseToChannel(PlayerCollisionChannel, ECR_Overlap);
    ActivationTriggerComponent->SetCanEverAffectNavigation(false);

    DetectTriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Detect Trigger Component"));
    DetectTriggerComponent->SetupAttachment(GetRootComponent());
    DetectTriggerComponent->SetSphereRadius(500.f);

    DetectTriggerComponent->SetGenerateOverlapEvents(true);
    DetectTriggerComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    DetectTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectTriggerComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    DetectTriggerComponent->SetCollisionResponseToChannel(PlayerCollisionChannel, ECollisionResponse::ECR_Overlap);
    DetectTriggerComponent->SetCanEverAffectNavigation(false);

    MovementComponent = CreateDefaultSubobject<UMTD_TokenMovementComponent>(TEXT("Movement Component"));
}

void AMTD_FloatingToken::BeginPlay()
{
    Super::BeginPlay();

    ActivationTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnActivationTriggerBeginOverlap);

    DetectTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnDetectTriggerBeginOverlap);
    DetectTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnDetectTriggerEndOverlap);
}

bool AMTD_FloatingToken::CanBeActivatedOn(APawn *Pawn) const
{
    return true;
}

void AMTD_FloatingToken::OnActivate_Implementation(APawn *Pawn)
{
    Destroy();
}

APawn *AMTD_FloatingToken::FindNewTarget() const
{
    return (DetectedPawns.IsEmpty()) ? (nullptr) : (DetectedPawns[0]);
}

void AMTD_FloatingToken::OnPawnAdded(APawn *Pawn)
{
    // Home towards the actor if there's no target and the actor isn't full
    const USceneComponent *Target = MovementComponent->HomingTargetComponent.Get();
    if ((!IsValid(Target)) && (CanBeActivatedOn(Pawn)))
    {
        const FVector P0 = GetActorLocation();
        const FVector P1 = Pawn->GetActorLocation();
        const FVector Displacement = P1 - P0;
        
        FVector Direction;
        float Distance;
        
        Displacement.ToDirectionAndLength(Direction, Distance);
        
        MovementComponent->SetUpdatedComponent(GetRootComponent());
        MovementComponent->AddForce(Direction * MinimalForceTowardsTarget);
        SetNewTarget(Pawn);
    }
}

void AMTD_FloatingToken::OnPawnRemoved(APawn *Pawn)
{
    // If the actor was the homing target, find a new one
    const USceneComponent *Target = MovementComponent->HomingTargetComponent.Get();
    if ((IsValid(Target)) && (Pawn == Target->GetOwner()))
    {
        APawn *NewTarget = FindNewTarget();
        SetNewTarget(NewTarget);
    }
}

void AMTD_FloatingToken::SetNewTarget(APawn *Pawn)
{
    if (bIgnoreTriggers)
    {
        return;
    }
    
    MovementComponent->HomingTargetComponent = (IsValid(Pawn)) ? (Pawn->GetRootComponent()) : (nullptr);
}

void AMTD_FloatingToken::IgnoreTriggersFor(float Seconds)
{
    if (IsValid(MovementComponent->HomingTargetComponent.Get()))
    {
        SetNewTarget(nullptr);
    }
    
    bIgnoreTriggers = true;
    
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnStopIgnoreTriggers, Seconds, false);
}

void AMTD_FloatingToken::OnActivationTriggerBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    auto Pawn = Cast<APawn>(OtherActor);
    if (CanBeActivatedOn(Pawn))
    {
        OnActivate(Pawn);
    }
}

void AMTD_FloatingToken::OnDetectTriggerBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    auto Pawn = Cast<APawn>(OtherActor);
    if (!ensureMsgf(Pawn, TEXT("Actor [%s] is not a Pawn."), *OtherActor->GetName()))
    {
        return;
    }
    
    DetectedPawns.Add(Pawn);
    OnPawnAdded(Pawn);
}

void AMTD_FloatingToken::OnDetectTriggerEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    auto Pawn = Cast<APawn>(OtherActor);
    if (!ensureMsgf(Pawn, TEXT("Actor [%s] is not a Pawn."), *OtherActor->GetName()))
    {
        return;
    }
    
    DetectedPawns.Remove(Pawn);
    OnPawnRemoved(Pawn);
}

void AMTD_FloatingToken::OnStopIgnoreTriggers()
{
    bIgnoreTriggers = false;
    
    APawn *NewTarget = FindNewTarget();
    if (IsValid(NewTarget))
    {
        SetNewTarget(NewTarget);
    }
}
