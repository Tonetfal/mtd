#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SBS_BuildGhostActor.generated.h"

class UArrowComponent;

UCLASS(meta=(ToolTip="The used static meshes must stand right on the grid."))
class SIMPLEBUILDSYSTEMRUNTIME_API ASBS_BuildGhostActor : public AActor
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE(
        FMulticastDelegateSignature);

public:
    ASBS_BuildGhostActor();

    virtual void Tick(float DeltaSeconds) override;

    void SetStaticMesh(UStaticMesh *Mesh);
    void SetMaterial(UMaterialInterface *MaterialInterface);

    void SetDrawVision(bool Flag);
    void SetVision(float Range, float Degrees);

    float GetOffsetZ() const;

protected:
    virtual void BeginPlay() override;

private:
    UFUNCTION()
    void OnMeshBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    UFUNCTION()
    void OnMeshEndOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex);

    void DrawVision();
    float GetMeshHeight() const;

public:
    FMulticastDelegateSignature OnBuildAllowedDelegate;
    FMulticastDelegateSignature OnBuildForbidDelegate;

private:
    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<USceneComponent> SceneRoot = nullptr;

    /** The direction the object should be facing. */
    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UArrowComponent> Arrow = nullptr;

    /** The static mesh used to visualize the ghost. */
    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UStaticMeshComponent> StaticMesh = nullptr;

    /** The actors the Build Ghost Actor is overlapping with. */
    UPROPERTY()
    TArray<TObjectPtr<AActor>> OverlappingActors;

    /** Units the Build Ghost Actor will be howering above the ground. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Build Ghost Actor",
        Config, meta=(AllowPrivateAccess="true"))
    float BaseOffsetZ = 0.1f;

    bool bDrawVision = false;
    float VisionRange = 0.f;
    float VisionDegrees = 0.f;
};

inline void ASBS_BuildGhostActor::SetDrawVision(bool Flag)
{
    bDrawVision = Flag;
    SetActorTickEnabled(bDrawVision);
}

inline void ASBS_BuildGhostActor::SetVision(float Range, float Degrees)
{
    VisionRange = Range;
    VisionDegrees = Degrees;
}
