#pragma once

#include "mtd.h"
#include "AIController.h"
#include "Perception/AISightTargetInterface.h"

#include "Components/MTD_TeamComponent.h"

#include "MTD_EnemyController.generated.h"

class UMTD_PathFollowingComponent;
class UAIPerceptionStimuliSourceComponent;
class UBehaviorTreeComponent;

UCLASS()
class MTD_API AMTD_EnemyController : public AAIController,
	public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	AMTD_EnemyController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn *InPawn) override;
	virtual void InitAI(UBehaviorTree *BehaviorTree);

public:
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
	
protected:
	virtual void OnOwnerDeath();
	virtual void OnNewAttackTargetSpotted(AActor *NewAttackTarget);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components")
	TObjectPtr<UBlackboardComponent> BlackboardComp = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMTD_TeamComponent> Team = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<UMTD_PathFollowingComponent> MtdPathFollowingComp = nullptr;
};
