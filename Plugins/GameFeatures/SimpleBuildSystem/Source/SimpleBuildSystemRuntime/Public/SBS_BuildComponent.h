#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"

#include "SBS_BuildComponent.generated.h"

class ASBS_BuildGhostActor;
class UCameraComponent;
class UInputMappingContext;

/** Enumeration defining build state. */
UENUM(BlueprintType)
enum class ESBS_BuildState : uint8
{
    Moving,
    Rotating,
    Building,
    Finished,
    Invalid
};

/** Data defining a building object. */
USTRUCT(BlueprintType)
struct FSBS_BuildingData
{
    GENERATED_BODY()

public:
    bool IsValid() const;
    void Invalidate();

public:
    /** The object that will be built at the end. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AActor> ObjectClass = nullptr;

    /**
     * The static mesh used by the ghost building actor. Its collision determines whether the final object will be
     * placed at the location the player is willing to place it at.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UStaticMesh> StaticMesh = nullptr;

    /** Build duration in seconds. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0.0"))
    float Duration = 0.f;

    /** The material used by the ghost building actor when it's allowed to build. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UMaterialInterface> AllowMaterial = nullptr;

    /** The material used by the ghost building actor when it's forbid to build. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UMaterialInterface> ForbidMaterial = nullptr;

    /** Should vision be drawn in front of building actor? */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bDrawVision = false;

    /** Units the building can see. */
    UPROPERTY(BlueprintReadWrite)
    float VisionRange = 0.f;
    
    /** Degrees the building can see. */
    UPROPERTY(BlueprintReadWrite)
    float VisionDegrees = 0.f;
};

/**
 * Simple build component that gives a player the possibility to build an object.
 * There are 4 steps before finishing the building process:
 * - Moving
 * - Rotating
 * - Building
 * - Finilizing the result.
 */
UCLASS(ClassGroup="Player", meta=(BlueprintSpawnableComponent))
class SIMPLEBUILDSYSTEMRUNTIME_API USBS_BuildComponent : public UPawnComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(
        FDynamicMulticastDelegateSignature);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnBuildFinishedSignature,
        AActor*, BuiltActor);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnBuildStateChangedSignature,
        ESBS_BuildState, NewState,
        ESBS_BuildState, OldState);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnBuildProgressChangedSignature,
        float, NewProgressRatio,
        float, OldProgressRatio);

public:
    USBS_BuildComponent(const FObjectInitializer &ObjectInitializer);

    virtual void TickComponent(
        float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

    /** Enable build mode by initializing everything related and giving the new inputs. */
    UFUNCTION(BlueprintCallable, Category="Build Component")
    bool BuildModeEnable(FSBS_BuildingData Data);

    /** Forget about old building data, use the new ones. Reset the building state to the default one. */
    UFUNCTION(BlueprintCallable, Category="Build Component")
    void ChangeBuildingData(FSBS_BuildingData Data);

    /** Remove everything build mode related, abort any building process. */
    UFUNCTION(BlueprintCallable, Category="Build Component")
    void BuildModeDisable();

    /** Procede to the next building stage. For example, moving, rotating, building. */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Build Component")
    void BuildConfirm();

protected:
    virtual void BuildConfirm_Implementation();

    //~AActor interface
    virtual void BeginPlay() override;
    //~End of AActor interface

    /** Spawn and setup ghost build actor. */
    UFUNCTION(BlueprintNativeEvent, Category="Build Component")
    void BuildStart();
    virtual void BuildStart_Implementation();

    /** Invalidate build related data. */
    UFUNCTION(BlueprintNativeEvent, Category="Build Component")
    void BuildAbort();
    virtual void BuildAbort_Implementation();

    /** Called after BuildFinish. Should be overrode with custom functionality. Disables Build Mode by default. */
    UFUNCTION(BlueprintNativeEvent, Category="Build Component")
    void BuildStop();
    virtual void BuildStop_Implementation();

    /** Called when Build Progress finishes. Spawns and setups the building object. */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Build Component")
    void BuildFinish();
    virtual void BuildFinish_Implementation();

    /** Move building ghost actor around. */
    UFUNCTION(BlueprintNativeEvent, Category="Build Component")
    void MoveBuildGhostActor();
    virtual void MoveBuildGhostActor_Implementation();

    /** Rotate building ghost actor around itself. */
    UFUNCTION(BlueprintNativeEvent, Category="Build Component")
    void RotateBuildGhostActor();
    virtual void RotateBuildGhostActor_Implementation();

    /** Called each tick when Building State is Building. Builds up the progress, and calls BuildFinish on finish. */
    UFUNCTION(BlueprintNativeEvent, Category="Build Component")
    void ProgressBuilding(float DeltaSeconds);
    virtual void ProgressBuilding_Implementation(float DeltaSeconds);

    /** Computes how much progress should be made given the passed time. */
    UFUNCTION(BlueprintNativeEvent, Category="Build Component")
    float GetBuildProgressRatioPerTick(float DeltaSeconds) const;
    virtual float GetBuildProgressRatioPerTick_Implementation(float DeltaSeconds) const;

    /** Adds some build progress and notifies about it using OnBuildProgressChangedDelegate. */
    void AddBuildProgressRatio(float ProgressRatio);

    /** Sets a new state and notifies about it using OnBuildStateChangedDelegate. */
    UFUNCTION(BlueprintCallable, Category="Build Component")
    void SetBuildState(ESBS_BuildState NewState);

private:
    void BindBuildDelegates();

    void CreateGhostBuildActor();
    AActor *SpawnBuilding() const;

    void OnBuildAllowed();
    void OnBuildForbid();

    void AddInputContext(const UInputMappingContext *InputMappingContext);
    void RemoveInputContext(const UInputMappingContext *InputMappingContext);

    FVector FindObservedPoint(float TraceLength, FHitResult *OutHit = nullptr) const;
    FVector FindGround(FVector LineStart, FHitResult *OutHit = nullptr) const;

public:
    UPROPERTY(BlueprintAssignable)
    FOnBuildProgressChangedSignature OnBuildProgressChangedDelegate;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FDynamicMulticastDelegateSignature OnBuildEnabledDelegate;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FDynamicMulticastDelegateSignature OnBuildDisabledDelegate;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FDynamicMulticastDelegateSignature OnBuildConfirmDelegate;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FDynamicMulticastDelegateSignature OnStartBuildingDelegate;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FDynamicMulticastDelegateSignature OnBuildingDataChangedDelegate;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnBuildFinishedSignature OnBuildFinishedDelegate;

    UPROPERTY(BlueprintCallable, BlueprintAssignable)
    FOnBuildStateChangedSignature OnBuildStateChangedDelegate;

private:
    /** Should the component work? */
    UPROPERTY(BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true"))
    bool bIsBuildModeActive = false;

    /** Can the building be placed at the location it is on? */
    UPROPERTY(BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true"))
    bool bCanPlaceBuilding = false;

    /** If the building actor can have a controller, it will be granted one through SpawnDefaultController function. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true"))
    bool bTryCreateDefaultControllersOnSpawn = true;

    /** Data defining the building the owner is willing to build. */
    UPROPERTY(BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true"))
    FSBS_BuildingData ActiveBuildingData;

    /** Actor class that visualizes the building placement and process. */
    UPROPERTY(EditDefaultsOnly, Category="Build Component")
    TSubclassOf<ASBS_BuildGhostActor> BuildGhostActorClass = nullptr;

    /** Actor that visualizes the building placement and process. */
    UPROPERTY(BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<ASBS_BuildGhostActor> BuildGhostActor = nullptr;

    /** State the build process is on. */
    UPROPERTY(BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true"))
    ESBS_BuildState BuildState = ESBS_BuildState::Invalid;

    /** Progress normalized value. */
    UPROPERTY(BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true"))
    float BuildProgressRatio = 0.f;

    /** Input mapping context that will be active only when the build mode is active as well. */
    UPROPERTY(EditDefaultsOnly, Category="Build Component")
    TObjectPtr<const UInputMappingContext> BuildModeMappingContext = nullptr;

    /** Priority the BuildModeMappingContext will be assigned with. */
    UPROPERTY(EditDefaultsOnly, Category="Build Component")
    int32 MappingContextPriority = 0;

    /** Length of the trace done from the camera location following its forward vector to find any surface. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true",
        ClampMin="1.0"))
    float MoveTraceLength = 1000.f;

    /** Length of the trace done downwards when placing an object to find a horizontal surface. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true",
        ClampMin="1.0"))
    float MoveSurfaceTraceLength = 1000.f;

    /**
     * Length of the trace done from the camera location following its forward vector to find a surface the ghost build
     * object has to face.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true",
        ClampMin="1.0"))
    float RotateTraceLength = 1000.f;

    /** Object types that the building object can be tried to be placed on, and rotated towards. */
    UPROPERTY(EditDefaultsOnly, Category="Build Component")
    TArray<TEnumAsByte<ECollisionChannel>> ObjectTypesToQueryOnTraces;

    /** Character that owns this component. */
    UPROPERTY(BlueprintReadOnly, Category="Build Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<ACharacter> Owner = nullptr;

    /** Character's controller that owns this component. */
    UPROPERTY()
    TObjectPtr<APlayerController> OwnerController = nullptr;

    /** Character's camerathat owns this component. */
    UPROPERTY()
    TObjectPtr<UCameraComponent> OwnerCamera = nullptr;

    UPROPERTY(BlueprintReadWrite, Category="Build Component", meta=(AllowPrivateAccess="true"))
    float GhostActorYaw = 0.f;
};
