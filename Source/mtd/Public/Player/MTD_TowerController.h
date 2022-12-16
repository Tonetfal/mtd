#pragma once

#include "AIController.h"
#include "Character/MTD_TeamComponent.h"
#include "mtd.h"
#include "Perception/AIPerceptionTypes.h"

#include "MTD_TowerController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class MTD_API AMTD_TowerController : public AAIController
{
    GENERATED_BODY()

public:
    AMTD_TowerController();

protected:
    //~AActor Interface
    virtual void BeginPlay() override;
    //~End of AActor Interface

    //~AAIController Interface
    virtual void OnPossess(APawn *InPawn) override;
    //~End of AAIController Interface

public:
    UFUNCTION(BlueprintCallable, Category="MTD|Vision")
    virtual AActor *GetFireTarget();

    //~IGenericTeamAgentInterface Interface
    virtual FGenericTeamId GetGenericTeamId() const override;
    //~End of IGenericTeamAgentInterface Interface

    UMTD_TeamComponent *GetTeamComponent() const;

protected:
    virtual bool IsFireTargetStillVisible() const;
    virtual AActor *SearchForFireTarget();
    virtual AActor *FindClosestActor(const TArray<AActor *> &Actors) const;
    virtual void InitConfig();
    
private:
    void SetVisionRange(float Range);
    void SetPeripheralVisionHalfAngleDegrees(float Degrees);

    UFUNCTION()
    void UpdateSightAttributes();
    void CacheTowerAttributes();

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TeamComponent> TeamComponent = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UAISenseConfig_Sight> SightConfig = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config",
        meta=(AllowPrivateAccess="true"))
    float SightRadius = 500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config",
        meta=(AllowPrivateAccess="true"))
    float LoseSightRadius = SightRadius;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config",
        meta=(AllowPrivateAccess="true"))
    float PeripheralVisionHalfAngleDegrees = 90.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config",
        meta=(AllowPrivateAccess="true"))
    FAISenseAffiliationFilter DetectionByAffiliation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config",
        meta=(AllowPrivateAccess="true"))
    float AutoSuccessRangeFromLastSeenLocation = FAISystem::InvalidRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config",
        meta=(AllowPrivateAccess="true"))
    float PointOfViewBackwardOffset = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config",
        meta=(AllowPrivateAccess="true"))
    float NearClippingRadius = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Sight Sense Config",
        meta=(AllowPrivateAccess="true"))
    float MaxAge = 5.0f;

    UPROPERTY(VisibleAnywhere, Category="MTD|Tower Controller",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<AActor> FireTarget = nullptr;
};

inline FGenericTeamId AMTD_TowerController::GetGenericTeamId() const
{
    return TeamComponent->GetGenericTeamId();
}

inline UMTD_TeamComponent *AMTD_TowerController::GetTeamComponent() const
{
    return TeamComponent;
}
