#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SBS_BuildGhostActor.generated.h"

class UArrowComponent;

DECLARE_MULTICAST_DELEGATE(FMulticastDelegateSignature);

UCLASS()
class SIMPLEBUILDSYSTEMRUNTIME_API ASBS_BuildGhostActor : public AActor
{
	GENERATED_BODY()

public:
	ASBS_BuildGhostActor();

	void SetStaticMesh(UStaticMesh *Mesh);
	void SetMaterial(UMaterialInterface *MaterialInterface);
	float GetOffsetZ() const
		{ return OffsetZ; }

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

public:
	FMulticastDelegateSignature OnBuildAllowedDelegate;
	FMulticastDelegateSignature OnBuildForbidDelegate;

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USceneComponent> SceneRoot = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UArrowComponent> Arrow = nullptr;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh = nullptr;

	/// The actors the Build Ghost Actor is overlapping with.
	UPROPERTY()
	TArray<TObjectPtr<AActor>> OverlappingActors;

	/// Units the Build Ghost Actor is moved by Z axis.
	float OffsetZ = 0.f;

	/// Units the Build Ghost Actor will be howering above the ground.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Build Ghost Actor",
		Config, meta=(AllowPrivateAccess="true"))
	float BaseOffsetZ = 0.1f;
};
