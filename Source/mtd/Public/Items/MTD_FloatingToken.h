#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "mtd.h"

#include "MTD_FloatingToken.generated.h"

class UBoxComponent;
class UMTD_TokenMovementComponent;
class USphereComponent;

UCLASS(Abstract)
class MTD_API AMTD_FloatingToken
    : public AActor
{
    GENERATED_BODY()

public:
    AMTD_FloatingToken();

    UFUNCTION(BlueprintCallable, Category="MTD|Floating Token")
    void IgnoreTriggersFor(float Seconds);

protected:
    //~AActor Interface
    virtual void BeginPlay() override;
    //~End of AActor Interface

    virtual bool CanBeActivatedOn(APawn *Pawn) const;

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Floating Token")
    void OnActivate(APawn *Pawn);
    virtual void OnActivate_Implementation(APawn *Pawn);

    virtual APawn *FindNewTarget() const;

    virtual void OnPawnAdded(APawn *Pawn);
    virtual void OnPawnRemoved(APawn *Pawn);
    virtual void SetNewTarget(APawn *Target);
    
    virtual void EnableScan();
    virtual void DisableScan();

    virtual void AddToListening(AActor *Actor);
    virtual void RemoveFromListening(AActor *Actor);
    void RemoveCurrentTargetFromListening();

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

    UFUNCTION()
    void OnStopIgnoreTriggers();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components")
    TObjectPtr<UMTD_TokenMovementComponent> MovementComponent = nullptr;
    
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Trigger|Debug")
    TArray<TObjectPtr<APawn>> DetectedPawns;

    UPROPERTY(VisibleInstanceOnly, Category="MTD|Trigger|Debug")
    bool bIgnoreTriggers = false;
    
    /**
     * If true, all detected triggers have full mana pool, thus wait for their current & max mana attribute to change
     * to start flying towards them.
     */
    UPROPERTY(VisibleInstanceOnly, Category="MTD|Trigger|Debug")
    bool bScanMode = false;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> CollisionComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> ActivationTriggerComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<USphereComponent> DetectTriggerComponent = nullptr;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Trigger", meta=(AllowPrivateAccess="true"))
    float MinimalImpulseTowardsTarget = 50.f;

    FTimerHandle IgnoreTimerHandle;
};
