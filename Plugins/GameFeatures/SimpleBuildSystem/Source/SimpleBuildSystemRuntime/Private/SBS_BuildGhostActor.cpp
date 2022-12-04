#include "SBS_BuildGhostActor.h"

#include "Components/ArrowComponent.h"

ASBS_BuildGhostActor::ASBS_BuildGhostActor()
{
    PrimaryActorTick.bCanEverTick = false;
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
    
    const FBox Box = StaticMesh->GetStaticMesh()->GetBoundingBox();
    const float HalfHeight = Box.Max.Z / 2.f;
    
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
