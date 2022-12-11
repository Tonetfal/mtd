#include "SBS_BuildComponent.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "SBS_BuildGhostActor.h"
#include "Kismet/GameplayStatics.h"

DECLARE_LOG_CATEGORY_CLASS(LogSimpleBuildSystem, All, All);

bool FSBS_BuildingData::IsValid() const
{
    return ((ObjectClass) && (::IsValid(StaticMesh)) && (::IsValid(AllowMaterial)) && (::IsValid(ForbidMaterial)));
}

void FSBS_BuildingData::Invalidate()
{
    ObjectClass = nullptr;
    StaticMesh = nullptr;
    AllowMaterial = nullptr;
    ForbidMaterial = nullptr;
}

USBS_BuildComponent::USBS_BuildComponent(const FObjectInitializer &ObjectInitializer)
: UPawnComponent(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USBS_BuildComponent::BeginPlay()
{
    Super::BeginPlay();

    Owner = GetPawn<ACharacter>();
    if (!IsValid(Owner))
    {
        UE_LOG(LogSimpleBuildSystem, Warning, TEXT("Owner is invalid."));
        return;
    }

    ensure(BuildModeMappingContext);

    OwnerCamera = Owner->FindComponentByClass<UCameraComponent>();
    check(IsValid(OwnerCamera));

    OwnerController = Owner->GetLocalViewingPlayerController();
    check(IsValid(OwnerController));
}

void USBS_BuildComponent::BuildStart_Implementation()
{
    ensure(!bIsBuildModeActive);

    CreateGhostBuildActor();
    SetComponentTickEnabled(true);

    SetBuildState(ESBS_BuildState::Moving);
}

void USBS_BuildComponent::BuildAbort_Implementation()
{
    if (IsValid(BuildGhostActor))
    {
        BuildGhostActor->Destroy();
    }

    SetBuildState(ESBS_BuildState::Invalid);
}

void USBS_BuildComponent::BuildStop_Implementation()
{
    BuildModeDisable();
}

void USBS_BuildComponent::MoveBuildGhostActor_Implementation()
{
    ensure(BuildState == ESBS_BuildState::Moving);

    FHitResult Hit;
    const FVector ObservedPoint = FindObservedPoint(MoveTraceLength, &Hit);

    // Fix some issues with ground placing by adding an offset
    const FVector GroundOffset = FVector::UpVector + Hit.ImpactNormal;
    
    const FVector SurfaceLocation = ObservedPoint + GroundOffset;
    const FVector GroundLocation = FindGround(SurfaceLocation);

    BuildGhostActor->SetActorLocation(GroundLocation);
}

void USBS_BuildComponent::RotateBuildGhostActor_Implementation()
{
    ensure(BuildState == ESBS_BuildState::Rotating);

    const FVector ObservedPoint = FindObservedPoint(MoveTraceLength);
    const FVector ObjectLocation = BuildGhostActor->GetActorLocation();

    const FVector Distance = ObservedPoint - ObjectLocation;
    const FVector Cross = ObservedPoint.Cross(ObjectLocation);

    // Avoid dividing by 0
    if (Distance.X == 0.f)
    {
        return;
    }

    // Compute Z angle
    const float AngleRad = FMath::Atan(Distance.Y / Distance.X);
    const float AngleMod = (FMath::Sign(Cross.Y) == 1) ? (180.f) : (0.f);
    const float AngleDeg = FMath::RadiansToDegrees(AngleRad) + AngleMod;

    // Rotate by Y
    BuildGhostActor->SetActorRotation(FRotator(0.f, AngleDeg, 0.f), ETeleportType::TeleportPhysics);
}

void USBS_BuildComponent::ProgressBuilding_Implementation(float DeltaSeconds)
{
    ensure(BuildProgressRatio < 1.f);

    const float DeltaProgress = GetBuildProgressRatioPerTick(DeltaSeconds);
    AddBuildProgressRatio(DeltaProgress);

    if (BuildProgressRatio >= 1.f)
    {
        SetBuildState(ESBS_BuildState::Finished);
    }
}

float USBS_BuildComponent::GetBuildProgressRatioPerTick_Implementation(float DeltaSeconds) const
{
    if (ActiveBuildingData.Duration <= 0.f)
    {
        return 1.f;
    }

    return DeltaSeconds / ActiveBuildingData.Duration;
}

void USBS_BuildComponent::AddBuildProgressRatio(float ProgressRatio)
{
    ensure((ProgressRatio >= 0.f) && (ProgressRatio <= 1.f));
    ensure((BuildProgressRatio >= 0.f) && (BuildProgressRatio <= 1.f));

    const float OldValue = BuildProgressRatio;
    BuildProgressRatio = FMath::Min(1.f, BuildProgressRatio + ProgressRatio);

    OnBuildProgressChangedDelegate.Broadcast(BuildProgressRatio, OldValue);
}

void USBS_BuildComponent::SetBuildState(ESBS_BuildState NewState)
{
    ensure(BuildState != NewState);

    const ESBS_BuildState OldState = BuildState;
    BuildState = NewState;

    // Don't notify about invalidating
    if (NewState == ESBS_BuildState::Invalid)
    {
        return;
    }

    if (NewState == ESBS_BuildState::Building)
    {
        OnStartBuildingDelegate.Broadcast();
    }

    OnBuildStateChangedDelegate.Broadcast(NewState, OldState);
}

void USBS_BuildComponent::BindBuildDelegates()
{
    check(IsValid(BuildGhostActor));

    BuildGhostActor->OnBuildAllowedDelegate.AddUObject(this, &ThisClass::OnBuildAllowed);
    BuildGhostActor->OnBuildForbidDelegate.AddUObject(this, &ThisClass::OnBuildForbid);
}

void USBS_BuildComponent::CreateGhostBuildActor()
{
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    UWorld *World = GetWorld();
    AActor *Actor = World->SpawnActor(BuildGhostActorClass, nullptr, nullptr, SpawnParameters);
    
    BuildGhostActor = Cast<ASBS_BuildGhostActor>(Actor);

    // Bind as soon as possible
    BindBuildDelegates();
    
    BuildGhostActor->SetStaticMesh(ActiveBuildingData.StaticMesh);
}

AActor *USBS_BuildComponent::SpawnBuilding() const
{
    const FVector Offset(0.f, 0.f, BuildGhostActor->GetOffsetZ());
    
    FTransform Transform = BuildGhostActor->GetTransform();
    Transform.AddToTranslation(Offset);

    const auto HdlMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AActor *Actor = GetWorld()->SpawnActorDeferred<AActor>(
        ActiveBuildingData.ObjectClass, Transform, Owner, Owner, HdlMethod);

    if (bTryCreateDefaultControllersOnSpawn)
    {
        if (ActiveBuildingData.ObjectClass->IsChildOf(APawn::StaticClass()))
        {
            auto Pawn = Cast<APawn>(Actor);
            Pawn->SpawnDefaultController();
        }
    }

    UGameplayStatics::FinishSpawningActor(Actor, Transform);

    return Actor;
}

void USBS_BuildComponent::OnBuildAllowed()
{
    check(BuildGhostActor);

    bCanPlaceBuilding = true;
    BuildGhostActor->SetMaterial(ActiveBuildingData.AllowMaterial);
}

void USBS_BuildComponent::OnBuildForbid()
{
    check(BuildGhostActor);
    ensure(bCanPlaceBuilding);

    bCanPlaceBuilding = false;
    BuildGhostActor->SetMaterial(ActiveBuildingData.ForbidMaterial);
}

void USBS_BuildComponent::AddInputContext(const UInputMappingContext *InputMappingContext)
{
    check(OwnerController);

    auto EiSubsystem = OwnerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    FModifyContextOptions ContextOptions;
    ContextOptions.bIgnoreAllPressedKeysUntilRelease = false;

    EiSubsystem->AddMappingContext(InputMappingContext, MappingContextPriority, ContextOptions);
}

void USBS_BuildComponent::RemoveInputContext(const UInputMappingContext *InputMappingContext)
{
    check(OwnerController);

    auto EiSubsystem = OwnerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    FModifyContextOptions ContextOptions;
    ContextOptions.bIgnoreAllPressedKeysUntilRelease = false;

    EiSubsystem->RemoveMappingContext(InputMappingContext, ContextOptions);
}

FVector USBS_BuildComponent::FindObservedPoint(float TraceLength, FHitResult *OutHit) const
{
    const FVector CameraLocation = OwnerCamera->GetComponentLocation();
    const FVector CameraForward = OwnerCamera->GetForwardVector();

    const FVector LineStart = CameraLocation;
    const FVector LineEnd = LineStart + CameraForward * TraceLength;

    FCollisionObjectQueryParams ObjectQueryParams;
    for (ECollisionChannel CollisionChannel : ObjectTypesToQueryOnTraces)
    {
        ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
    }

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(BuildGhostActor);
    QueryParams.AddIgnoredActor(Owner);

    FHitResult Hit;
    GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, LineEnd, ObjectQueryParams, QueryParams);

    if (OutHit)
    {
        *OutHit = Hit;
    }

    return Hit.bBlockingHit ? Hit.ImpactPoint : LineEnd;
}

FVector USBS_BuildComponent::FindGround(FVector LineStart) const
{
    const FVector LineEnd =
        LineStart - FVector::ZAxisVector * MoveSurfaceTraceLength;

    FCollisionObjectQueryParams ObjectQueryParams;
    for (ECollisionChannel CollisionChannel : ObjectTypesToQueryOnTraces)
    {
        ObjectQueryParams.AddObjectTypesToQuery(CollisionChannel);
    }

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(BuildGhostActor);
    QueryParams.AddIgnoredActor(Owner);
    QueryParams.bTraceComplex = true;

    FHitResult Hit;
    GetWorld()->LineTraceSingleByObjectType(Hit, LineStart, LineEnd, ObjectQueryParams, QueryParams);

    return Hit.bBlockingHit ? Hit.ImpactPoint : LineEnd;
}

bool USBS_BuildComponent::BuildModeEnable(FSBS_BuildingData Data)
{
    // ChangeBuildingData must be used if Build Mode is already enabled
    if (bIsBuildModeActive)
    {
        return false;
    }

    if (!Data.IsValid())
    {
        UE_LOG(LogSimpleBuildSystem, Warning, TEXT("Data is invalid"));
        return false;
    }

    ActiveBuildingData = Data;

    BuildStart();
    AddInputContext(BuildModeMappingContext.Get());
    bIsBuildModeActive = true;

    OnBuildEnabledDelegate.Broadcast();
    return true;
}

void USBS_BuildComponent::ChangeBuildingData(FSBS_BuildingData Data)
{
    // BuildModeEnable must be used if it is not enabled
    if (!bIsBuildModeActive)
    {
        UE_LOG(LogSimpleBuildSystem, Warning, TEXT("Build Mode is not active"));
        return;
    }

    if (!Data.IsValid())
    {
        UE_LOG(LogSimpleBuildSystem, Warning, TEXT("Data is invalid"));
        return;
    }

    ensure(BuildState != ESBS_BuildState::Building);
    ensure(BuildProgressRatio == 0.f);
    check(BuildGhostActor);

    ActiveBuildingData = Data;
    if (BuildState != ESBS_BuildState::Moving)
    {
        SetBuildState(ESBS_BuildState::Moving);
    }

    // New building may have new mesh and different height, hence re-create it
    BuildGhostActor->Destroy();
    bCanPlaceBuilding = true;
    CreateGhostBuildActor();

    OnBuildingDataChangedDelegate.Broadcast();
}

void USBS_BuildComponent::BuildModeDisable()
{
    if (!bIsBuildModeActive)
    {
        return SetComponentTickEnabled(false);
    }
    RemoveInputContext(BuildModeMappingContext.Get());

    BuildAbort();

    bIsBuildModeActive = false;
    bCanPlaceBuilding = false;
    BuildProgressRatio = 0.f;

    OnBuildDisabledDelegate.Broadcast();
}

void USBS_BuildComponent::BuildConfirm_Implementation()
{
    if (!bCanPlaceBuilding)
    {
        return;
    }

    switch (BuildState)
    {
    case ESBS_BuildState::Moving:
        SetBuildState(ESBS_BuildState::Rotating);
        break;

    case ESBS_BuildState::Rotating:
        SetBuildState(ESBS_BuildState::Building);
        break;

    default:
        break;
    }

    OnBuildConfirmDelegate.Broadcast();
}

void USBS_BuildComponent::BuildFinish_Implementation()
{
    AActor *Actor = SpawnBuilding();

    OnBuildFinishedDelegate.Broadcast(Actor);
}

void USBS_BuildComponent::TickComponent(
    float DeltaSeconds,
    ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

    switch (BuildState)
    {
    case ESBS_BuildState::Moving:
        MoveBuildGhostActor();
        break;
    case ESBS_BuildState::Rotating:
        RotateBuildGhostActor();
        break;
    case ESBS_BuildState::Building:
        ProgressBuilding(DeltaSeconds);
        break;
    case ESBS_BuildState::Finished:
        BuildFinish();
        BuildStop();
        break;
    default:
        break;
    }
}
