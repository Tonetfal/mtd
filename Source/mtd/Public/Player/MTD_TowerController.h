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
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn *InPawn) override;

public:
    UFUNCTION(BlueprintCallable, Category="MTD|Vision")
    virtual AActor *GetFireTarget();

    UFUNCTION(BlueprintCallable, Category="MTD|Vision")
    virtual void SetVisionRange(float Range);

    UFUNCTION(BlueprintCallable, Category="MTD|Vision")
    virtual void SetPeripheralVisionHalfAngleDegrees(float Degrees);

    virtual FGenericTeamId GetGenericTeamId() const override
    {
        return TeamComponent->GetGenericTeamId();
    }

    UMTD_TeamComponent *GetTeamComponent() const
    {
        return TeamComponent;
    }

protected:
    virtual bool IsFireTargetStillVisible() const;
    virtual AActor *SearchForFireTarget();
    virtual AActor *FindClosestActor(const TArray<AActor *> &Actors) const;
    virtual void InitConfig();

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
