// #include "Tasks/MTD_BTT_FollowPath.h"
//
// #include "AISystem.h"
// #include "GameplayTasksComponent.h"
// #include "Navigation/PathFollowingComponent.h"
// #include "Tasks/AITask_MoveTo.h"
//
// DECLARE_LOG_CATEGORY_CLASS(LogBttFollowPath, All, All);
//
// UMTD_BTT_FollowLevelPath::UMTD_BTT_FollowLevelPath(
// 	const FObjectInitializer &ObjectInitializer)
// : Super(ObjectInitializer)
// {
// 	NodeName = "WIP Follow Level Path";
// 	INIT_TASK_NODE_NOTIFY_FLAGS();
//
// 	AcceptableRadius = GET_AI_CONFIG_VAR(AcceptanceRadius);
// 	bReachTestIncludesGoalRadius =
// 	bReachTestIncludesAgentRadius =
// 	bStopOnOverlap = GET_AI_CONFIG_VAR(bFinishMoveOnGoalOverlap);
// 	bAllowStrafe = GET_AI_CONFIG_VAR(bAllowStrafing);
// 	bAllowPartialPath = GET_AI_CONFIG_VAR(bAcceptPartialPaths);
// 	bTrackMovingGoal = true;
// 	bProjectGoalLocation = true;
// 	bUsePathfinding = true;
//
// 	bStopOnOverlapNeedsUpdate = true;
// }
//
// EBTNodeResult::Type UMTD_BTT_FollowLevelPath::ExecuteTask(
// 	UBehaviorTreeComponent &OwnerComp,
// 	uint8 *NodeMemory)
// {
// 	FMTD_BTT_FollowLevelPathMemory *MyMemory =
// 		CastInstanceNodeMemory<FMTD_BTT_FollowLevelPathMemory>(NodeMemory);
// 	memset(&*MyMemory, 0, GetInstanceMemorySize());
// 	
// 	AAIController *AiController = OwnerComp.GetAIOwner();
// 	if (!IsValid(AiController))
// 	{
// 		UE_LOG(LogBttFollowPath, Error,
// 			TEXT("ExecuteTask failed because AIController is missing"));
// 		return EBTNodeResult::Failed;
// 	}
//
// 	MyMemory->EnemyAiController = Cast<AMTD_ObserverAiController>(AiController);
// 	if (!IsValid(MyMemory->EnemyAiController))
// 	{
// 		UE_LOG(LogBttFollowPath, Error,
// 			TEXT("ExecuteTask failed because AIController cannot be casted to "
// 				"MTD enemy AI controller"));
// 		return EBTNodeResult::Failed;
// 	}
// 	
// 	// MyMemory->EnemyAiController->PreparePath();
// 	// MyMemory->CurrentPathPoint =
// 	// 	MyMemory->EnemyAiController->GetCurrentPathPoint();
// 	
// 	return PerformMoveTask(OwnerComp, NodeMemory);
// }
//
// EBTNodeResult::Type UMTD_BTT_FollowLevelPath::PerformMoveTask(
// 	UBehaviorTreeComponent &OwnerComp,
// 	uint8 *NodeMemory)
// {
// 	FMTD_BTT_FollowLevelPathMemory *MyMemory =
// 		CastInstanceNodeMemory<FMTD_BTT_FollowLevelPathMemory>(NodeMemory);
//
// 	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
// 	if (!IsValid(MyMemory->EnemyAiController))
// 	{
// 		return EBTNodeResult::Failed;
// 	}
// 	
// 	// Prepare movement related request data
// 	FAIMoveRequest MoveReq;
// 	MoveReq.SetNavigationFilter(
// 		MyMemory->EnemyAiController->GetDefaultNavigationFilterClass());
// 	MoveReq.SetAllowPartialPath(true);
// 	MoveReq.SetAcceptanceRadius(true);
// 	MoveReq.SetCanStrafe(true);
// 	MoveReq.SetReachTestIncludesAgentRadius(true);
// 	MoveReq.SetReachTestIncludesGoalRadius(true);
// 	MoveReq.SetProjectGoalLocation(true);
// 	MoveReq.SetUsePathfinding(true);
// 	MoveReq.SetGoalLocation(MyMemory->CurrentPathPoint);
// 	MyMemory->PreviousGoalLocation = MyMemory->CurrentPathPoint;
//
// 	if (!MoveReq.IsValid())
// 	{
// 		return EBTNodeResult::Failed;
// 	}
//
// 	/** bEnableBTAITasks is deprecated. BT AITasks should be enabled instead. */
// 	/** The if statement has to be made if the node will not work properly!   */
// 	
// 	// if (GET_AI_CONFIG_VAR(bEnableBTAITasks))
// 	// {
// 	
// 	UAITask_MoveTo *MoveTask = MyMemory->Task.Get();
// 	const bool bReuseExistingTask = (MoveTask != nullptr);
//
// 	MoveTask = PrepareMoveTask(OwnerComp, MoveTask, MoveReq);
// 	if (IsValid(MoveTask))
// 	{
// 		// Perform the actual movement
// 		if (bReuseExistingTask)
// 		{
// 			if (MoveTask->IsActive())
// 			{
// 				UE_LOG(LogBttFollowPath, VeryVerbose,
// 					TEXT("'%s' reusing AITask %s"),
// 					*GetNodeName(), *MoveTask->GetName());
// 				MoveTask->ConditionalPerformMove();
// 			}
// 			else
// 			{
// 				UE_LOG(LogBttFollowPath, VeryVerbose,
// 					TEXT("'%s' reusing AITask %s, but task is not active"),
// 						*GetNodeName(), *MoveTask->GetName());
// 			}
// 		}
// 		else
// 		{
// 			MyMemory->Task = MoveTask;
// 			UE_LOG(LogBttFollowPath, VeryVerbose,
// 				TEXT("'%s' task implementing move with task %s"),
// 				*GetNodeName(), *MoveTask->GetName());
// 			MoveTask->ReadyForActivation();
// 		}
//
// 		// Determine the movement result
// 		NodeResult =
// 			MoveTask->GetState() != EGameplayTaskState::Finished ?
// 			EBTNodeResult::InProgress : MoveTask->WasMoveSuccessful() ?
// 			EBTNodeResult::Succeeded : EBTNodeResult::Failed;
//
// 		UE_LOG(LogTemp, Warning, TEXT("BTT Follow Path %d"),
// 			MoveTask->GetState());
// 	}
//
// 	
// 	/** Check the note above the commented if statement above */
//
// 	// }
// 	// else
// 	// {
// 	// 	const FPathFollowingRequestResult RequestResult = 
// 	// 		MyMemory->EnemyAiController->MoveTo(MoveReq);
// 	// 	if (RequestResult.Code ==
// 	// 		EPathFollowingRequestResult::RequestSuccessful)
// 	// 	{
// 	// 		MyMemory->MoveRequestID = RequestResult.MoveId;
// 	// 		
// 	// 		WaitForMessage(
// 	// 			OwnerComp,
// 	// 			UBrainComponent::AIMessage_MoveFinished,
// 	// 			RequestResult.MoveId);
// 	// 		
// 	// 		WaitForMessage(
// 	// 			OwnerComp,
// 	// 			UBrainComponent::AIMessage_RepathFailed);
// 	//
// 	// 		NodeResult = EBTNodeResult::InProgress;
// 	// 	}
// 	// 	else if (RequestResult.Code ==
// 	// 		EPathFollowingRequestResult::AlreadyAtGoal)
// 	// 	{
// 	// 		NodeResult = EBTNodeResult::Succeeded;
// 	// 	}
// 	// }
// 	
// 	return NodeResult;
// }
//
// UAITask_MoveTo *UMTD_BTT_FollowLevelPath::PrepareMoveTask(
// 	UBehaviorTreeComponent &OwnerComp, 
// 	UAITask_MoveTo *ExistingTask,
// 	FAIMoveRequest &MoveRequest)
// {
// 	UAITask_MoveTo *MoveTask = ExistingTask ?
// 		ExistingTask : NewBTAITask<UAITask_MoveTo>(OwnerComp);
// 	if (MoveTask)
// 	{
// 		MoveTask->SetUp(MoveTask->GetAIController(), MoveRequest);
// 	}
//
// 	return MoveTask;
// }
//
// bool UMTD_BTT_FollowLevelPath::TrySelectNextPathPoint(uint8 *NodeMemory) const
// {
// 	// FMTD_BTT_FollowLevelPathMemory *MyMemory =
// 	// 	CastInstanceNodeMemory<FMTD_BTT_FollowLevelPathMemory>(NodeMemory);
// 	//
// 	// if (MyMemory->EnemyAiController->MoveOnNextPathPoint())
// 	// {
// 	// 	MyMemory->CurrentPathPoint = 
// 	// 		MyMemory->EnemyAiController->GetCurrentPathPoint();
// 	// 	
// 	// 	UE_LOG(LogBttFollowPath, VeryVerbose,
// 	// 		TEXT("Moving on next point"));
// 	// 	
// 	// 	return true;
// 	// }
// 	// else
// 	// {
// 	// 	// No path point is available: Finish the task
// 	// 	UE_LOG(LogBttFollowPath, VeryVerbose,
// 	// 		TEXT("Finished the path"));
// 	// 	
// 	// 	return false;
// 	// }
// 	return false;
// }
//
// EBTNodeResult::Type UMTD_BTT_FollowLevelPath::AbortTask(
// 	UBehaviorTreeComponent &OwnerComp, uint8 *NodeMemory)
// {
// 	FMTD_BTT_FollowLevelPathMemory *MyMemory =
// 		CastInstanceNodeMemory<FMTD_BTT_FollowLevelPathMemory>(NodeMemory);
// 	if (MyMemory->MoveRequestID.IsValid())
// 	{
// 		AAIController* MyController = OwnerComp.GetAIOwner();
// 		if (MyController && MyController->GetPathFollowingComponent())
// 		{
// 			MyController->GetPathFollowingComponent()->AbortMove(
// 				*this,
// 				FPathFollowingResultFlags::OwnerFinished,
// 				MyMemory->MoveRequestID);
// 		}
// 	}
// 	else
// 	{
// 		MyMemory->bObserverCanFinishTask = false;
// 		UAITask_MoveTo *MoveTask = MyMemory->Task.Get();
// 		if (MoveTask)
// 		{
// 			MoveTask->ExternalCancel();
// 		}
// 	}
//
// 	return Super::AbortTask(OwnerComp, NodeMemory);
// }
//
// void UMTD_BTT_FollowLevelPath::OnTaskFinished(
// 	UBehaviorTreeComponent &OwnerComp, 
// 	uint8 *NodeMemory, 
// 	EBTNodeResult::Type TaskResult)
// {
// 	FMTD_BTT_FollowLevelPathMemory *MyMemory = 
// 		CastInstanceNodeMemory<FMTD_BTT_FollowLevelPathMemory>(NodeMemory);
// 	MyMemory->Task.Reset();
//
// 	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
//
// 	UE_LOG(LogBttFollowPath, VeryVerbose, TEXT("Task has been finished"));
// }
//
// void UMTD_BTT_FollowLevelPath::OnMessage(
// 	UBehaviorTreeComponent &OwnerComp,
// 	uint8 *NodeMemory, 
// 	FName Message,
// 	int32 SenderID,
// 	bool bSuccess)
// {
// 	// AIMessage_RepathFailed means task has failed
// 	bSuccess &= (Message != UBrainComponent::AIMessage_RepathFailed);
// 	Super::OnMessage(OwnerComp, NodeMemory, Message, SenderID, bSuccess);
// }
//
// void UMTD_BTT_FollowLevelPath::OnGameplayTaskDeactivated(UGameplayTask &Task)
// {
// 	// AI move task finished
// 	UAITask_MoveTo *MoveTask = Cast<UAITask_MoveTo>(&Task);
// 	if (IsValid(MoveTask) &&
// 		MoveTask->GetAIController() &&
// 		MoveTask->GetState() != EGameplayTaskState::Paused)
// 	{
// 		UBehaviorTreeComponent *BehaviorComp = GetBTComponentForTask(Task);
// 		if (IsValid(BehaviorComp))
// 		{
// 			uint8 *NodeMemory = BehaviorComp->GetNodeMemory(
// 				this, BehaviorComp->FindInstanceContainingNode(this));
// 			FMTD_BTT_FollowLevelPathMemory *MyMemory =
// 				CastInstanceNodeMemory<FMTD_BTT_FollowLevelPathMemory>(
// 					NodeMemory);
//
// 			// We are interested in keeping the pace until the path ends
// 			if (MyMemory &&
// 				!MyMemory->bObserverCanFinishTask &&
// 				(MoveTask == MyMemory->Task))
// 			{
// 				if (TrySelectNextPathPoint(NodeMemory)) // There's a path point
// 				{
// 					PerformMoveTask(*BehaviorComp, NodeMemory);
// 					MoveTask->ConditionalPerformMove();
// 				}
// 				else // We've finished our path
// 				{
// 					MyMemory->bObserverCanFinishTask = true;
// 				}
// 			}
// 			
// 			if (MyMemory &&
// 				MyMemory->bObserverCanFinishTask &&
// 				(MoveTask == MyMemory->Task))
// 			{
// 				const bool bSuccess = MoveTask->WasMoveSuccessful();
// 				FinishLatentTask(*BehaviorComp, bSuccess ?
// 					EBTNodeResult::Succeeded : EBTNodeResult::Failed);
// 			}
// 		}
// 	}
// }
//
// uint16 UMTD_BTT_FollowLevelPath::GetInstanceMemorySize() const
// {
// 	return sizeof(FMTD_BTT_FollowLevelPathMemory);
// }
//
// void UMTD_BTT_FollowLevelPath::PostLoad()
// {
// 	Super::PostLoad();
// 	
// 	if (bStopOnOverlapNeedsUpdate)
// 	{
// 		bStopOnOverlapNeedsUpdate = false;
// 		bReachTestIncludesAgentRadius = bStopOnOverlap;
// 		bReachTestIncludesGoalRadius = false;
// 	}
// }
