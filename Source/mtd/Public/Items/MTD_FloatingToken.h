#pragma once

#include "mtd.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "MTD_FloatingToken.generated.h"

class UBoxComponent;
class UMTD_TokenMovementComponent;
class USphereComponent;

UCLASS()
class MTD_API AMTD_FloatingToken : public AActor
{
    GENERATED_BODY()

public:
    AMTD_FloatingToken();

    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;

    virtual bool CanBeActivatedOn(APawn *Pawn) const;

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Floating Token")
    void OnActivate(APawn *Pawn);
    virtual void OnActivate_Implementation(APawn *Pawn);

    virtual APawn *FindNewTarget() const;

    virtual void OnPawnAdded(APawn *Pawn);
    virtual void OnPawnRemoved(APawn *Pawn);

private:
    UFUNCTION()
    void OnActivationTriggerBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);
    
    UFUNCTION()
    void OnDetectTriggerBeginOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult &SweepResult);

    UFUNCTION()
    void OnDetectTriggerEndOverlap(
        UPrimitiveComponent *OverlappedComponent,
        AActor *OtherActor,
        UPrimitiveComponent *OtherComp,
        int32 OtherBodyIndex);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components")
    TObjectPtr<UMTD_TokenMovementComponent> MovementComponent = nullptr;
    
    UPROPERTY()
    TArray<TObjectPtr<APawn>> DetectedPawns;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> CollisionComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> ActivationTriggerComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> DetectTriggerComponent = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Trigger", meta=(AllowPrivateAccess="true"))
    float MinimalForceTowardsTarget = 25.f;
};
