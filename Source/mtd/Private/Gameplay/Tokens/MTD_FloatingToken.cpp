#include "Gameplay/Tokens/MTD_FloatingToken.h"

#include "Components/SphereComponent.h"
#include "Gameplay/Tokens/MTD_TokenMovementComponent.h"

AMTD_FloatingToken::AMTD_FloatingToken()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>("Collision Component");
    SetRootComponent(CollisionComponent);
    CollisionComponent->SetSphereRadius(25.f);
    CollisionComponent->SetCollisionProfileName(FloatingTokenCollisionProfileName);
    CollisionComponent->SetGenerateOverlapEvents(false);
    CollisionComponent->SetCanEverAffectNavigation(false);

    ActivationTriggerComponent = CreateDefaultSubobject<USphereComponent>("Activation Trigger Component");
    ActivationTriggerComponent->SetupAttachment(GetRootComponent());
    ActivationTriggerComponent->SetSphereRadius(50.f);
    ActivationTriggerComponent->SetGenerateOverlapEvents(true);
    ActivationTriggerComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    ActivationTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ActivationTriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    ActivationTriggerComponent->SetCollisionResponseToChannel(PlayerCollisionChannel, ECR_Overlap);
    ActivationTriggerComponent->SetCanEverAffectNavigation(false);

    DetectTriggerComponent = CreateDefaultSubobject<USphereComponent>("Detect Trigger Component");
    DetectTriggerComponent->SetupAttachment(GetRootComponent());
    DetectTriggerComponent->SetSphereRadius(500.f);
    DetectTriggerComponent->SetGenerateOverlapEvents(true);
    DetectTriggerComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    DetectTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectTriggerComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    DetectTriggerComponent->SetCollisionResponseToChannel(PlayerCollisionChannel, ECollisionResponse::ECR_Overlap);
    DetectTriggerComponent->SetCanEverAffectNavigation(false);

    MovementComponent = CreateDefaultSubobject<UMTD_TokenMovementComponent>("Movement Component");
}

void AMTD_FloatingToken::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Listen for overlap events
    ActivationTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnActivationTriggerBeginOverlap);
    DetectTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnDetectTriggerBeginOverlap);
    DetectTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnDetectTriggerEndOverlap);
}

void AMTD_FloatingToken::StopHomingFor(float IgnoreTime)
{
    if (IsValid(MovementComponent->HomingTargetComponent.Get()))
    {
        // Remove target if asked to ignore
        SetNewTarget(nullptr);
    }

    // Switch state
    bDontHome = true;

    // Fire stop ignore event after IgnoreTime expires
    GetWorldTimerManager().SetTimer(IgnoreTimerHandle, this, &ThisClass::OnStopIgnoreTriggers, IgnoreTime, false);
}

bool AMTD_FloatingToken::CanBeActivatedBy(APawn *Trigger) const
{
    return true;
}

void AMTD_FloatingToken::OnActivate_Implementation(APawn *Trigger)
{
    if (IgnoreTimerHandle.IsValid())
    {
        // Avoid firing anything because we're already done
        GetWorldTimerManager().ClearTimer(IgnoreTimerHandle);
    }

    // Destroy on activation
    Destroy();
}

APawn *AMTD_FloatingToken::FindNewTarget() const
{
    return nullptr;
}

void AMTD_FloatingToken::OnTriggerAdded(APawn *Trigger)
{
    const USceneComponent *CurrentTarget = MovementComponent->HomingTargetComponent.Get();
    if (((!IsValid(CurrentTarget)) && (CanBeActivatedBy(Trigger))))
    {
        // New target to follow
        SetNewTarget(Trigger);
    }
    else
    {
        if (bScanMode)
        {
            // Add a new trigger that cannot be activate us at the moment to listening
            AddToListening(Trigger);
        }
        else
        {
            // Enable scan, hence start listening for all triggers
            EnableScan();
        }
    }
}

void AMTD_FloatingToken::OnTriggerRemoved(APawn *Trigger)
{
    if (bScanMode)
    {
        // We're not interested in him since he left
        RemoveFromListening(Trigger);

        // Disable scan if no one left
        if (DetectedTriggers.IsEmpty())
        {
            DisableScan();
        }
    }
    else
    {
        // If the trigger was the target, try to find a new one
        const USceneComponent *Target = MovementComponent->HomingTargetComponent.Get();
        if (((IsValid(Target)) && (Trigger == Target->GetOwner())))
        {
            APawn *NewTarget = FindNewTarget();
            SetNewTarget(NewTarget);
        }
    }
}

void AMTD_FloatingToken::SetNewTarget(APawn *Target)
{
    // Avoid setting a new target when we don't home
    if (bDontHome)
    {
        return;
    }

    if (IsValid(Target))
    {
        // Add some impulse towards the target
        const FVector ImpulseVelocity = GetImpulseTowards(Target);
        MovementComponent->AddForce(ImpulseVelocity);

        // Set target as the new homing target on movement component
        MovementComponent->SetUpdatedComponent(GetRootComponent());
        MovementComponent->HomingTargetComponent = Target->GetRootComponent();
    }
    else
    {
        MovementComponent->HomingTargetComponent = nullptr;
    }
}

FVector AMTD_FloatingToken::GetImpulseTowards(const APawn *Trigger) const
{
    if (!IsValid(Trigger))
    {
        return FVector::ZeroVector;
    }
    
    if (ImpulseStrength == 0.f)
    {
        return FVector::ZeroVector;
    }

    // Find direction towards the pawn, and scale it by the impulse strength
    const FVector P0 = GetActorLocation();
    const FVector P1 = Trigger->GetActorLocation();
    const FVector Displacement = (P1 - P0);
    const FVector Direction = Displacement.GetSafeNormal();
    const FVector ImpulseVelocity = (Direction * ImpulseStrength);

    return ImpulseVelocity;
}

void AMTD_FloatingToken::EnableScan()
{
    ensure(!bScanMode);
    bScanMode = true;

    // Add all triggers to listening
    ensure(!DetectedTriggers.IsEmpty());
    for (APawn *Pawn : DetectedTriggers)
    {
        AddToListening(Pawn);
    }
}

void AMTD_FloatingToken::DisableScan()
{
    ensure(bScanMode);
    bScanMode = false;
    
    // Remove all triggers from listening
    for (APawn *Pawn : DetectedTriggers)
    {
        RemoveFromListening(Pawn);
    }
}

void AMTD_FloatingToken::AddToListening(AActor *Trigger)
{
    // Empty
}

void AMTD_FloatingToken::RemoveFromListening(AActor *Trigger)
{
    // Empty
}

void AMTD_FloatingToken::RemoveCurrentTargetFromListening()
{
    if (MovementComponent->HomingTargetComponent.IsValid())
    {
        AActor *OldTarget = MovementComponent->HomingTargetComponent->GetOwner();
        RemoveFromListening(OldTarget);
    }
}

void AMTD_FloatingToken::OnActivationTriggerBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    auto Trigger = Cast<APawn>(OtherActor);
    if (CanBeActivatedBy(Trigger))
    {
        OnActivate(Trigger);
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
    auto Trigger = Cast<APawn>(OtherActor);
    if (!ensureMsgf(Trigger, TEXT("Actor [%s] is not a pawn."), *OtherActor->GetName()))
    {
        return;
    }
    
    DetectedTriggers.Add(Trigger);
    OnTriggerAdded(Trigger);
}

void AMTD_FloatingToken::OnDetectTriggerEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    auto Trigger = Cast<APawn>(OtherActor);
    if (!ensureMsgf(Trigger, TEXT("Actor [%s] is not a pawn."), *OtherActor->GetName()))
    {
        return;
    }
    
    DetectedTriggers.Remove(Trigger);
    OnTriggerRemoved(Trigger);
}

void AMTD_FloatingToken::OnStopIgnoreTriggers()
{
    // Switch state
    bDontHome = false;

    // Try to search a new target when we got this possiblity
    APawn *NewTarget = FindNewTarget();
    if (IsValid(NewTarget))
    {
        // Disable scan if we can have a target
        if (bScanMode)
        {
            DisableScan();
        }

        // Set the found target
        SetNewTarget(NewTarget);
    }
}
