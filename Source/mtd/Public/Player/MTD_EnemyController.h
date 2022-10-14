#pragma once

#include "AIController.h"
#include "mtd.h"
#include "Character/MTD_TeamComponent.h"
#include "Perception/AISightTargetInterface.h"

#include "MTD_EnemyController.generated.h"

UCLASS()
class MTD_API AMTD_EnemyController : public AAIController,
	public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	AMTD_EnemyController();

	virtual bool CanBeSeenFrom(
		const FVector &ObserverLocation,
		FVector &OutSeenLocation,
		int32 &NumberOfLoSChecksPerformed,
		float &OutSightStrength,
		const AActor *IgnoreActor,
		const bool *bWasVisible,
		int32 *UserData) const override;

	virtual FGenericTeamId GetGenericTeamId() const override
		{ return Team->GetGenericTeamId(); }
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMTD_TeamComponent> Team = nullptr;
};
