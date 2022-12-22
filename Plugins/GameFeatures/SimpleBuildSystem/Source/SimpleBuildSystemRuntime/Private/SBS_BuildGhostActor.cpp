#include "SBS_BuildGhostActor.h"

#include "CollisionDebugDrawingPublic.h"
#include "Components/ArrowComponent.h"

ASBS_BuildGhostActor::ASBS_BuildGhostActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
    SetRootComponent(SceneRoot);

    Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
    Arrow->SetupAttachment(GetRootComponent());

    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    StaticMesh->SetupAttachment(GetRootComponent());

    StaticMesh->SetCollisionProfileName("OverlapAll");
    StaticMesh->SetGenerateOverlapEvents(true);
}

void ASBS_BuildGhostActor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    DrawVision();
}

void ASBS_BuildGhostActor::SetStaticMesh(UStaticMesh *Mesh)
{
    if (!IsValid(Mesh))
    {
        return;
    }

    // Fix floating point error, otherwise the object will likely overlap with the ground it's standing on
    StaticMesh->AddLocalOffset(FVector(0.f, 0.f, BaseOffsetZ));

    const UStaticMesh *OldMesh = StaticMesh->GetStaticMesh();
    StaticMesh->SetStaticMesh(Mesh);

    // After a mesh has been set up it may call begin/end overlap events depending if there was a mesh and if the new
    // one is overlapping with something. In case of absence of any mesh previous mesh and no overlapping actor at the
    // frame the mesh has been set, broadcast the possibility to build
    if ((!OldMesh) && (OverlappingActors.IsEmpty()))
    {
        OnBuildAllowedDelegate.Broadcast();
    }
}

void ASBS_BuildGhostActor::SetMaterial(UMaterialInterface *MaterialInterface)
{
    if (!IsValid(MaterialInterface))
    {
        return;
    }

    StaticMesh->SetMaterial(0, MaterialInterface);
}

float ASBS_BuildGhostActor::GetOffsetZ() const
{
    if (!IsValid(StaticMesh))
    {
        return 0.f;
    }

    const float Height = GetMeshHeight();
    const float HalfHeight = Height / 2.f;

    return HalfHeight + BaseOffsetZ;
}

void ASBS_BuildGhostActor::BeginPlay()
{
    Super::BeginPlay();

    StaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnMeshBeginOverlap);
    StaticMesh->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnMeshEndOverlap);
}

void ASBS_BuildGhostActor::OnMeshBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    OverlappingActors.Add(OtherActor);

    if (OverlappingActors.Num() == 1)
    {
        OnBuildForbidDelegate.Broadcast();
    }
}

void ASBS_BuildGhostActor::OnMeshEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    OverlappingActors.Remove(OtherActor);

    if (OverlappingActors.IsEmpty())
    {
        OnBuildAllowedDelegate.Broadcast();
    }
}

#define DRAW_LINE(P1, P2) DrawDebugLine(World, P1, P2, Color, bPersistentLines, LifeTime, DepthPriority, Thickness)
#define COMPUTE_DIRECTION(RAD) FVector(FMath::Cos(RAD), FMath::Sin(RAD), 0.f)
#define COMPUTE_VERTEX(RAD, OFFSET_VECTOR) \
    Transform.TransformPosition((COMPUTE_DIRECTION(CurrentAngle) * VisionRange) + OFFSET_VECTOR)
#define DRAW_ARC(OFFSET_VECTOR) \
    do { \
        CurrentAngle = MinAngle; \
        FVector LastVertex = COMPUTE_VERTEX(CurrentAngle, OFFSET_VECTOR); \
        CurrentAngle += AngleStep; \
        for (int32 i = 0; i < Sections; i++) \
        { \
            const FVector ThisVertex = COMPUTE_VERTEX(CurrentAngle, OFFSET_VECTOR); \
            DRAW_LINE(LastVertex, ThisVertex); \
            LastVertex = ThisVertex; \
            CurrentAngle += AngleStep; \
        } \
    } while(0)

void ASBS_BuildGhostActor::DrawVision()
{
    UWorld *World = GetWorld();

    const FTransform Transform = StaticMesh->GetComponentTransform();
    const float VisionHeight = GetMeshHeight();
    const FVector HeightVector = FVector::UpVector * VisionHeight;
    const FVector RotatedHeightVector = Transform.GetUnitAxis(EAxis::Z) * VisionHeight;

    const FVector BottomRootPosition = Transform.GetLocation();
    const FVector TopRootPosition = BottomRootPosition + RotatedHeightVector;

    const float HalfVisionRad = FMath::DegreesToRadians(VisionDegrees / 2.f);
    const float X = FMath::Cos(HalfVisionRad);
    const float Y = FMath::Sin(HalfVisionRad);
    
    const FVector LeftSide = FVector(X, -Y, 0.f) * VisionRange;
    const FVector RightSide = FVector(X, Y, 0.f) * VisionRange;
    
    const FVector BottomFront = Transform.TransformPosition(FVector::ForwardVector * VisionRange);
    const FVector TopFront = BottomFront + RotatedHeightVector;

    const FVector TopLeft = Transform.TransformPosition(HeightVector + LeftSide);
    const FVector TopRight = Transform.TransformPosition(HeightVector + RightSide);

    const FVector BottomLeft = Transform.TransformPosition(LeftSide);
    const FVector BottomRight = Transform.TransformPosition(RightSide);

    const FColor Color = FColor::Cyan;
    const bool bPersistentLines = false;
    const float LifeTime = -1.f;
    const uint8 DepthPriority = 0;
    const float Thickness = 2.5f;

    const int32 Sections = 24;
    const float MinAngle = -HalfVisionRad;
    const float MaxAngle = +HalfVisionRad;
    float AngleStep = (MaxAngle - MinAngle) / static_cast<float>(Sections);
    float CurrentAngle = MinAngle;

    // Middle
    DRAW_LINE(BottomRootPosition, TopRootPosition);

    // Left side
    DRAW_LINE(BottomRootPosition, BottomLeft);
    DRAW_LINE(TopRootPosition, TopLeft);
    DRAW_LINE(BottomLeft, TopLeft);

    // Right side
    DRAW_LINE(BottomRootPosition, BottomRight);
    DRAW_LINE(TopRootPosition, TopRight);
    DRAW_LINE(BottomRight, TopRight);

    // Front side
    DRAW_ARC(FVector::ZeroVector);
    DRAW_ARC(HeightVector);
    DRAW_LINE(BottomRootPosition, BottomFront);
    DRAW_LINE(TopRootPosition, TopFront);
    DRAW_LINE(BottomFront, TopFront);
}

float ASBS_BuildGhostActor::GetMeshHeight() const
{
    const FBox Box = StaticMesh->GetStaticMesh()->GetBoundingBox();
    return Box.Max.Z;
}

#undef DRAW_ARC
#undef COMPUTE_VERTEX
#undef COMPUTE_DIRECTION
#undef DRAW_LINE
