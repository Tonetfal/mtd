#include "Controllers/MTD_EnemyController.h"

#include "Characters/MTD_BaseEnemy.h"
#include "Components/MTD_HealthComponent.h"
#include "Components/MTD_PathFollowingComponent.h"
#include "Utilities/MTD_Utilities.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

AMTD_EnemyController::AMTD_EnemyController()
{
	PrimaryActorTick.bCanEverTick = false;

	StimuliSource =
		CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(
			TEXT("Perception Stimuli Source Component"));
	
	PathFollowing =
		CreateDefaultSubobject<UMTD_PathFollowingComponent>(
			TEXT("MTD Path Following Component"));
	
	BlackboardComp =
		CreateDefaultSubobject<UBlackboardComponent>(
			TEXT("Blackboard Component"));

	BehaviorTreeComp =
		CreateDefaultSubobject<UBehaviorTreeComponent>(
			TEXT("Behavior Tree Component"));

	Team =
		CreateDefaultSubobject<UMTD_TeamComponent>(TEXT("MTD Team Component"));

	// Controllers are stimuli sources, so we must be attached to
	// our pawn in order to "make it" sensible to the perception system
	bAttachToPawn = true;
}

void AMTD_EnemyController::BeginPlay()
{
	Super::BeginPlay();

	check(StimuliSource);
	check(Team);
	
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());

	if (!IsValid(GetPawn()))
	{
		MTD_WARN("Controlled pawn is invalid");
		return;
	}

	auto *Enemy = Cast<AMTD_BaseEnemy>(GetPawn());
	if (!IsValid(Enemy))
	{
		MTD_WARN("Failed to cast %s to AMTD_BaseEnemy", *GetPawn()->GetName());
		return;
	}

	Enemy->OnNewAttackTargetSpottedDelegate.AddUObject(
		this, &AMTD_EnemyController::OnNewAttackTargetSpotted);
}

void AMTD_EnemyController::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);
	
	if (!IsValid(InPawn))
	{
		MTD_WARN("Pawn is invalid");
		return;
	}

	AMTD_BaseEnemy *Enemy = Cast<AMTD_BaseEnemy>(InPawn);
	if (!IsValid(Enemy))
	{
		MTD_WARN("Failed to cast %s to AMTD_BaseEnemyCharacter",
			*InPawn->GetName());
		return;
	}

	UMTD_HealthComponent *Health = 
		FMTD_Utilities::GetActorComponent<UMTD_HealthComponent>(Enemy);
	if (!IsValid(Health))
	{
		MTD_WARN("%s has an invalid health component", *InPawn->GetName());
		return;
	}

	// We have to be seen
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	
	// We have to be unregistered from sense system after our owner dies
	Health->OnDeathDelegate.AddUObject(
		this, &AMTD_EnemyController::OnOwnerDeath);
	
	UBehaviorTree *BehaviorTree = Enemy->GetBehaviorTree();
	if (!IsValid(BehaviorTree))
	{
		MTD_WARN("%s has an invalid behavior tree", *InPawn->GetName());
		return;
	}

	InitAI(BehaviorTree);

}

void AMTD_EnemyController::InitAI(UBehaviorTree *BehaviorTree)
{
	if (!IsValid(BehaviorTree))
	{
		MTD_WARN("Invalid behavior tree has been passed");
		return;
	}
	
	if (!IsValid(BlackboardComp))
	{
		MTD_WARN("%s has an invalid blackboard component", *GetName());
		return;
	}
	
	if (!IsValid(BehaviorTreeComp))
	{
		MTD_WARN("%s has an invalid behavior tree component", *GetName());
		return;
	}
	
	BlackboardComp->InitializeBlackboard(*(BehaviorTree->BlackboardAsset));
	BehaviorTreeComp->StartTree(*BehaviorTree);
}

bool AMTD_EnemyController::CanBeSeenFrom(
	const FVector &ObserverLocation,
	FVector &OutSeenLocation,
	int32 &NumberOfLoSChecksPerformed,
	float &OutSightStrength,
	const AActor *IgnoreActor,
	const bool *bWasVisible,
	int32 *UserData) const
{
	MTDS_WARN("CanBeSeenFrom obs loc %s ign act %s",
		*ObserverLocation.ToString(), IgnoreActor ? *IgnoreActor->GetName() :
		 TEXT("none"));
	if (!GetWorld() || !IsValid(GetPawn()))
		return false;

	FHitResult HitResult;
	const FVector OurLocation = GetPawn()->GetActorLocation();
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		ObserverLocation,
		OurLocation,
		EnemyTraceChannel);

	NumberOfLoSChecksPerformed++;

	const UPrimitiveComponent *Comp = HitResult.GetComponent();
	const bool bCanBeSeen = IsValid(Comp) &&
		Comp->GetCollisionObjectType() == EnemyCollisionChannel;

	if (IsValid(HitResult.GetActor()))
	{
		MTDS_WARN("%s", *HitResult.GetActor()->GetName());
	}
	
	if (bCanBeSeen)
	{
		OutSeenLocation = OurLocation;
		OutSightStrength = 1.f;
	}
	else
	{
		OutSeenLocation = FVector::ZeroVector;
		OutSightStrength = 0.f;
	}

	MTDS_WARN("Return %d", bCanBeSeen);
	return bCanBeSeen;
}

void AMTD_EnemyController::OnOwnerDeath()
{
	if (!IsValid(StimuliSource))
		return;

	// Dead actors should not be seen
	StimuliSource->UnregisterFromSense(UAISense_Sight::StaticClass());
	BehaviorTreeComp->StopTree();
}

void AMTD_EnemyController::OnNewAttackTargetSpotted(AActor *NewAttackTarget)
{
	if (!IsValid(BlackboardComp))
		return;
	
	BlackboardComp->SetValueAsObject(FName("AttackTarget"), NewAttackTarget);
}
