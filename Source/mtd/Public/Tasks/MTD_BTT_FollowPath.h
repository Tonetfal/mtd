// #pragma once
//
// #include "AITypes.h"
// #include "CoreMinimal.h"
// #include "BehaviorTree/BTTaskNode.h"
// #include "Navigation/PathFollowingComponent.h"
//
// #include "MTD_BTT_FollowPath.generated.h"
//
// // Forward declarations
// class AMTD_ObserverAiController;
//
// class UAITask_MoveTo;
// class UBlackboardComponent;
//
// struct FMTD_BTT_FollowLevelPathMemory
// {
// 	/** Move request ID */
// 	FAIRequestID MoveRequestID;
//
// 	FDelegateHandle BBObserverDelegateHandle;
// 	FVector PreviousGoalLocation;
//
// 	TWeakObjectPtr<UAITask_MoveTo> Task;
//
// 	uint8 bObserverCanFinishTask : 1;
//
// 	AMTD_ObserverAiController *EnemyAiController;
// 	FVector CurrentPathPoint;
// };
//
// /**
//  * Follow Level Path task node.
//  * Moves an AI pawn through a path specified by its MTD AI Enemy Controller
//  * using the navigation system.
//  */
// UCLASS(config=Game)
// class MTD_API UMTD_BTT_FollowLevelPath : public UBTTaskNode
// {
// 	GENERATED_UCLASS_BODY()
//
// private:
// 	virtual EBTNodeResult::Type ExecuteTask(
// 		UBehaviorTreeComponent &OwnerComp,
// 		uint8 *NodeMemory) override;
//
// 	virtual EBTNodeResult::Type AbortTask(
// 		UBehaviorTreeComponent &OwnerComp, 
// 		uint8 *NodeMemory) override;
// 	
// 	virtual void OnTaskFinished(
// 		UBehaviorTreeComponent &OwnerComp,
// 		uint8 *NodeMemory,
// 		EBTNodeResult::Type TaskResult) override;
// 	
// 	virtual uint16 GetInstanceMemorySize() const override;
// 	virtual void PostLoad() override;
//
// 	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
// 	virtual void OnMessage(
// 		UBehaviorTreeComponent &OwnerComp,
// 		uint8 *NodeMemory, 
// 		FName Message, 
// 		int32 RequestID,
// 		bool bSuccess) override;
//
// protected:
// 	virtual EBTNodeResult::Type PerformMoveTask(
// 		UBehaviorTreeComponent &OwnerComp,
// 		uint8 *NodeMemory);
// 	
// 	/** prepares move task for activation */
// 	virtual UAITask_MoveTo *PrepareMoveTask(
// 		UBehaviorTreeComponent &OwnerComp,
// 		UAITask_MoveTo *ExistingTask,
// 		FAIMoveRequest &MoveRequest);
//
// private:
// 	bool TrySelectNextPathPoint(uint8 *NodeMemory) const;
// 	
// private:
// 	/** fixed distance added to threshold between AI and goal location in destination reach test */
// 	UPROPERTY(config, Category="Node", EditAnywhere,
// 		meta=(ClampMin = "0.0", UIMin="0.0"))
// 	float AcceptableRadius;
//
// 	/** "None" will result in default filter being used */
// 	UPROPERTY(Category="Node", EditAnywhere)
// 	TSubclassOf<UNavigationQueryFilter> FilterClass;
//
// 	UPROPERTY(Category="Node", EditAnywhere)
// 	uint32 bAllowStrafe : 1;
//
// 	/** if set, use incomplete path when goal can't be reached */
// 	UPROPERTY(Category="Node", EditAnywhere, AdvancedDisplay)
// 	uint32 bAllowPartialPath : 1;
//
// 	/** if set, path to goal actor will update itself when actor moves */
// 	UPROPERTY(Category="Node", EditAnywhere, AdvancedDisplay)
// 	uint32 bTrackMovingGoal : 1;
//
// 	/** if set, goal location will be projected on navigation data (navmesh) before using */
// 	UPROPERTY(Category="Node", EditAnywhere, AdvancedDisplay)
// 	uint32 bProjectGoalLocation : 1;
//
// 	/** if set, radius of AI's capsule will be added to threshold between AI and goal location in destination reach test  */
// 	UPROPERTY(Category="Node", EditAnywhere)
// 	uint32 bReachTestIncludesAgentRadius : 1;
// 	
// 	/** if set, radius of goal's capsule will be added to threshold between AI and goal location in destination reach test  */
// 	UPROPERTY(Category="Node", EditAnywhere)
// 	uint32 bReachTestIncludesGoalRadius : 1;
//
// 	/** DEPRECATED, please use combination of bReachTestIncludes*Radius instead */
// 	UPROPERTY(Category="Node", VisibleInstanceOnly)
// 	uint32 bStopOnOverlap : 1;
//
// 	UPROPERTY()
// 	uint32 bStopOnOverlapNeedsUpdate : 1;
//
// 	/** if set, move will use pathfinding. Not exposed on purpose, please use BTTask_MoveDirectlyToward */
// 	uint32 bUsePathfinding : 1;
// };
