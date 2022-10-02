#pragma once

#include "mtd.h"
#include "AIController.h"
#include "Components/MTD_TeamComponent.h"
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

public:
	virtual AActor *GetFireTarget();
	virtual void SetVisionRange(float Range);
	virtual void SetPeripheralVisionHalfAngleDegrees(float Degrees);

	virtual FGenericTeamId GetGenericTeamId() const override
		{ return Team->GetGenericTeamId(); }

protected:
	virtual bool IsFireTargetStillVisible() const;
	virtual AActor *SearchForFireTarget();
	virtual AActor *FindClosestActor(const TArray<AActor*> &Actors) const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMTD_TeamComponent> Team = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAISenseConfig_Sight> SightConfig = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Sight Sense Config",
		meta=(AllowPrivateAccess="true"))
	float SightRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Sight Sense Config",
		meta=(AllowPrivateAccess="true"))
	float LoseSightRadius = SightRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Sight Sense Config",
		meta=(AllowPrivateAccess="true"))
	float PeripheralVisionHalfAngleDegrees = 90.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Sight Sense Config",
		meta=(AllowPrivateAccess="true"))
	FAISenseAffiliationFilter DetectionByAffiliation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Sight Sense Config",
		meta=(AllowPrivateAccess="true"))
	float AutoSuccessRangeFromLastSeenLocation = SightRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Sight Sense Config",
		meta=(AllowPrivateAccess="true"))
	float PointOfViewBackwardOffset = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Sight Sense Config",
		meta=(AllowPrivateAccess="true"))
	float NearClippingRadius = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Sight Sense Config",
		meta=(AllowPrivateAccess="true"))
	float MaxAge = 5.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Tower Controller",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<AActor> FireTarget = nullptr;
};
