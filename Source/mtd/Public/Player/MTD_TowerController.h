#pragma once

#include "AIController.h"
#include "Character/Components/MTD_TeamComponent.h"
#include "mtd.h"
#include "Perception/AIPerceptionTypes.h"

#include "MTD_TowerController.generated.h"

class UAISenseConfig_Sight;

/**
 * Tower controller that gives an owning tower core functions and controls some of its decisions.
 *
 * The core functions the controller grants the tower:
 * - sight sense
 * - team association
 *
 * It decides which target the tower has to fire, as well as handles all the sight sense logic.
 *
 * @see AMTD_Tower
 */
UCLASS()
class MTD_API AMTD_TowerController
    : public AAIController
{
    GENERATED_BODY()

public:
    AMTD_TowerController();

protected:
    //~AAIController Interface
    virtual void OnPossess(APawn *InPawn) override;
    //~End of AAIController Interface

private:
    /**
     * Initialize sight config.
     */
    void InitSightConfig();

    /**
     * Cache sight and vision half angle degrees values from owning tower.
     */
    void CacheTowerAttributes();

public:
    /**
     * Get target tower has to fire at.
     * @return  Target tower has to fire at. May be nullptr.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Vision")
    AActor *GetFireTarget();

    //~IGenericTeamAgentInterface Interface
    virtual FGenericTeamId GetGenericTeamId() const override;
    //~End of IGenericTeamAgentInterface Interface

private:
    /**
     * Check whether the current fire target is still visible or not.
     * @return  If true, current fire target is still visible, false otherwise.
     */
    bool IsFireTargetVisible() const;

    /**
     * Search for the closest actor in sight.
     * @return  The closest actor in sight. May be nullptr.
     */
    AActor *SearchForFireTarget() const;

    /**
     * Find the closest actor in given list.
     * @param   Actors: actors container to find the closest actor from.
     * @return  The closest actor in given list. May be nullptr.
     */
    AActor *FindClosestActor(const TArray<AActor *> &Actors) const;
    
    /**
     * Set new vision range, leading perception component to re-configure the sight sense and update stimuli listener.
     * @param   Range: new range the sight sense should use.
     */
    void SetVisionRange(float Range);
    
    /**
     * Set new peripheral vision half degrees, leading perception component to re-configure the sight sense and update
     * stimuli listener.
     * @param   HalfDegrees: new peripheral vision half degrees the sight sense should use.
     */
    void SetPeripheralVisionHalfAngleDegrees(float HalfDegrees);

    /**
     * Event to fire when range related data on the owning tower has changed.
     */
    UFUNCTION()
    void UpdateSightAttributes();

private:
    /** Component that associates us to a team. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TeamComponent> TeamComponent = nullptr;

    /** Config to setup sight sense. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UAISenseConfig_Sight> SightConfig = nullptr;

    /** Sight radius enemies are being seen from. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config", meta=(AllowPrivateAccess="true"))
    float SightRadius = 500.0f;

    /** Sight lose radius enemies are being unseen after. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config", meta=(AllowPrivateAccess="true"))
    float LoseSightRadius = SightRadius;

    /** Peripheral vision half angle degrees sight sense makes use of. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config", meta=(AllowPrivateAccess="true"))
    float PeripheralVisionHalfAngleDegrees = 90.0f;

    /** Behavior towards different teams. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config", meta=(AllowPrivateAccess="true"))
    FAISenseAffiliationFilter DetectionByAffiliation;

	/**
	 * If not an InvalidRange (which is the default), we will always be able to see the target that has already been
	 * seen if they are within this range of their last seen location.
	 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config", meta=(AllowPrivateAccess="true"))
    float AutoSuccessRangeFromLastSeenLocation = FAISystem::InvalidRange;

	/** Point of view move back distance for cone calculation. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config", meta=(AllowPrivateAccess="true"))
    float PointOfViewBackwardOffset = 0.f;

	/**
	 * Near clipping distance, to be used with point of view backward offset. Will act as a close by awareness and
	 * peripheral vision.
	 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config", meta=(AllowPrivateAccess="true"))
    float NearClippingRadius = 0.f;

	/** Age limit after stimuli generated by this sense become forgotten. 0 means "never". */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config", meta=(AllowPrivateAccess="true"))
    float MaxAge = 5.0f;

    /** Current target owning tower has to fire at. */
    UPROPERTY(VisibleAnywhere, Category="MTD|Tower Controller", meta=(AllowPrivateAccess="true"))
    TObjectPtr<AActor> FireTarget = nullptr;
};

inline FGenericTeamId AMTD_TowerController::GetGenericTeamId() const
{
    return TeamComponent->GetGenericTeamId();
}
