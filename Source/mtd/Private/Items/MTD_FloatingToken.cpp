#include "Items/MTD_FloatingToken.h"

#include "Components/SphereComponent.h"
#include "Items/MTD_TokenMovementComponent.h"

AMTD_FloatingToken::AMTD_FloatingToken()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;
    PrimaryActorTick.TickInterval = 0.1f; // Don't run it too often

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

void AMTD_FloatingToken::BeginPlay()
{
    Super::BeginPlay();

    ActivationTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnActivationTriggerBeginOverlap);

    DetectTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnDetectTriggerBeginOverlap);
    DetectTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnDetectTriggerEndOverlap);
}

void AMTD_FloatingToken::IgnoreTriggersFor(float Seconds)
{
    if (IsValid(MovementComponent->HomingTargetComponent.Get()))
    {
        SetNewTarget(nullptr);
    }
    
    bIgnoreTriggers = true;
    GetWorldTimerManager().SetTimer(IgnoreTimerHandle, this, &ThisClass::OnStopIgnoreTriggers, Seconds, false);
}

bool AMTD_FloatingToken::CanBeActivatedOn(APawn *Pawn) const
{
    return true;
}

void AMTD_FloatingToken::OnActivate_Implementation(APawn *Pawn)
{
    if (IgnoreTimerHandle.IsValid())
    {
        GetWorldTimerManager().ClearTimer(IgnoreTimerHandle);
    }
    
    Destroy();
}

APawn *AMTD_FloatingToken::FindNewTarget() const
{
    return nullptr;
}

void AMTD_FloatingToken::OnPawnAdded(APawn *Pawn)
{
    const USceneComponent *CurrentTarget = MovementComponent->HomingTargetComponent.Get();
    if (((!IsValid(CurrentTarget)) && (CanBeActivatedOn(Pawn))))
    {
        SetNewTarget(Pawn);
    }
    else
    {
        if (bScanMode)
        {
            AddToListening(Pawn);
        }
        else
        {
            EnableScan();
        }
    }
}

void AMTD_FloatingToken::OnPawnRemoved(APawn *Pawn)
{
    if (bScanMode)
    {
        RemoveFromListening(Pawn);
        if (DetectedPawns.IsEmpty())
        {
            DisableScan();
        }
    }
    else
    {
        // If the actor was the homing target, find a new one
        const USceneComponent *Target = MovementComponent->HomingTargetComponent.Get();
        if (((IsValid(Target)) && (Pawn == Target->GetOwner())))
        {
            APawn *NewTarget = FindNewTarget();
            SetNewTarget(NewTarget);
        }
    }
}

void AMTD_FloatingToken::SetNewTarget(APawn *Target)
{
    if (bIgnoreTriggers)
    {
        return;
    }

    if (IsValid(Target))
    {
        if (MinimalImpulseTowardsTarget != 0.f)
        {
            const FVector P0 = GetActorLocation();
            const FVector P1 = Target->GetActorLocation();
            const FVector Displacement = (P1 - P0);
            const FVector Direction = Displacement.GetSafeNormal();
            const FVector ImpulseVelocity = (Direction * MinimalImpulseTowardsTarget);

            // Add some impulse towards the target
            MovementComponent->AddForce(ImpulseVelocity);
        }
        
        MovementComponent->SetUpdatedComponent(GetRootComponent());
        MovementComponent->HomingTargetComponent = Target->GetRootComponent();
    }
    else
    {
        MovementComponent->HomingTargetComponent = nullptr;
    }
}

void AMTD_FloatingToken::EnableScan()
{
    ensure(!bScanMode);
    bScanMode = true;
}

void AMTD_FloatingToken::DisableScan()
{
    ensure(bScanMode);
    bScanMode = false;
}

void AMTD_FloatingToken::AddToListening(AActor *Actor)
{
    // Empty
}

void AMTD_FloatingToken::RemoveFromListening(AActor *Actor)
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
        if (bScanMode)
        {
            DisableScan();
        }
        
        SetNewTarget(NewTarget);
    }
}
