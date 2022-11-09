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
		return;

	StaticMesh->SetStaticMesh(Mesh);
	
	const FBox Box = Mesh->GetBoundingBox();
	OffsetZ = Box.Max.Z + BaseOffsetZ;

	// Fix floating point error, otherwise the object will likely overlap with
	// the ground it's standing on
	StaticMesh->AddLocalOffset(FVector(0.f, 0.f, OffsetZ));
}

void ASBS_BuildGhostActor::SetMaterial(UMaterialInterface *MaterialInterface)
{
	if (!IsValid(MaterialInterface))
		return;

	StaticMesh->SetMaterial(0, MaterialInterface);
}

void ASBS_BuildGhostActor::BeginPlay()
{
	Super::BeginPlay();

	StaticMesh->OnComponentBeginOverlap.AddDynamic(
		this, &ThisClass::OnMeshBeginOverlap);
	StaticMesh->OnComponentEndOverlap.AddDynamic(
		this, &ThisClass::OnMeshEndOverlap);
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
		OnBuildForbidDelegate.Broadcast();
}

void ASBS_BuildGhostActor::OnMeshEndOverlap(
	UPrimitiveComponent *OverlappedComponent,
	AActor *OtherActor,
	UPrimitiveComponent *OtherComp,
	int32 OtherBodyIndex)
{
	OverlappingActors.Remove(OtherActor);
	
	if (OverlappingActors.IsEmpty())
		OnBuildAllowedDelegate.Broadcast();
}
