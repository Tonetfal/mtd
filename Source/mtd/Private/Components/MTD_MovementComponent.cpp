//#include "Components/MTD_MovementComponent.h"
//
//#include "Characters/MTD_Pawn.h"
//#include "Components/CapsuleComponent.h"
//#include "GameFramework/Character.h"
//#include "GameFramework/Pawn.h"
//
//void UMTD_MovementComponent::BeginPlay()
//{
//	Super::BeginPlay();
//
//	AActor *ActorOwner = GetOwner();
//	if (IsValid(ActorOwner))
//	{
//		PawnOwner = Cast<APawn>(ActorOwner);
//		check(PawnOwner);
//		
//		MtdPawnOwner = Cast<AMTD_Pawn>(ActorOwner);
//		check(MtdPawnOwner);
//	}
//
//	bUseAccelerationForPaths = true;
//}
//
//void UMTD_MovementComponent::TickComponent(
//	float DeltaSeconds,
//	ELevelTick TickType,
//	FActorComponentTickFunction *ThisTickFunction) 
//{
//	FVector InputVector = FVector::ZeroVector;
//	InputVector = ConsumeInputVector();
//
//	if (!HasValidData() || ShouldSkipUpdate(DeltaSeconds))
//		return;
//	
//	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);
//	
//	// Super tick may destroy/invalidate Owner or UpdatedComponent,
//	// so we need to re-check it
//	if (!HasValidData())
//		return;
//
//	// We don't update if simulating physics (eg ragdolls).
//	if (UpdatedComponent->IsSimulatingPhysics())
//		return;
//
//	UE_LOG(LogTemp, Warning, TEXT("TickComponent InputVector %s"),
//		*InputVector.ToString());
//}
//
//void UMTD_MovementComponent::ControlledCharacterMove(
//	const FVector &InputVector, float DeltaSeconds)
//{
//	// TODO: Add this to MTD_CharacterMovementComponent
//	// We need to check the jump state before adjusting input acceleration,
//	// to minimize latency and to make sure acceleration respects our
//	// potentially new falling state.
//	// Owner->CheckJumpInput(DeltaSeconds);
//
//	// Apply acceleration to input
//	Acceleration =
//		ScaleInputAcceleration(ConstrainInputAcceleration(InputVector));
//	AnalogInputModifier = ComputeAnalogInputModifier();
//
//	PerformMovement(DeltaSeconds);
//}
//
//FVector UMTD_MovementComponent::ScaleInputAcceleration(
//	const FVector &InputAcceleration) const
//{
//	return MaxAcceleration * InputAcceleration.GetClampedToMaxSize(1.0f);
//}
//
//FVector UMTD_MovementComponent::ConstrainInputAcceleration(
//	const FVector &InputAcceleration) const
//{
//	// Walking or falling pawns ignore up/down sliding
//	if (InputAcceleration.Z != 0.f && (IsMovingOnGround() || IsFalling()))
//	{
//		return FVector(InputAcceleration.X, InputAcceleration.Y, 0.f);
//	}
//
//	return InputAcceleration;
//}
//
//float UMTD_MovementComponent::ComputeAnalogInputModifier() const
//{
//	const float MaxAccel = MaxAcceleration;
//	if (Acceleration.SizeSquared() > 0.f && MaxAccel > SMALL_NUMBER)
//	{
//		return FMath::Clamp<FVector::FReal>(
//			Acceleration.Size() / MaxAccel, 0.f, 1.f);
//	}
//
//	return 0.f;
//}
//
//void UMTD_MovementComponent::PerformMovement(float DeltaSeconds)
//{
//	const UWorld *MyWorld = GetWorld();
//	if (!HasValidData() || !IsValid(MyWorld))
//		return;
//	
//// 	FVector OldVelocity;
//// 	FVector OldLocation;
////
//// 	FScopedMovementUpdate ScopedMovementUpdate(
//// 		UpdatedComponent, EScopedUpdate::ImmediateUpdates);
//// 	UpdateBasedMovement(DeltaSeconds);
//// 	
//// 	OldVelocity = Velocity;
//// 	OldLocation = UpdatedComponent->GetComponentLocation();
////
//// 	// TODO: Add Impulse and Force impulses
//// 	// ApplyAccumulatedForces(DeltaSeconds);
////
//// 	// TODO: Crouching
//// 	// Update the character state before we do our movement
//// 	// UpdateCharacterStateBeforeMovement(DeltaSeconds);
////
//// 	// TODO: ...
//// 	// if (MovementMode == MOVE_NavWalking && bWantsToLeaveNavWalking)
//// 	// {
//// 	// 	TryToLeaveNavWalking();
//// 	// }
////
//// 	// TODO: ...
//// 	// Character::LaunchCharacter() has been deferred until now.
//// 	// HandlePendingLaunch();
//// 	// ClearAccumulatedForces();
////
////
//// 	// Update saved LastPreAdditiveVelocity with any external changes to character Velocity that happened due to ApplyAccumulatedForces/HandlePendingLaunch
//// 	// if(CurrentRootMotion.HasAdditiveVelocity())
//// 	// {
//// 	// 	const FVector Adjustment = (Velocity - OldVelocity);
//// 	// 	CurrentRootMotion.LastPreAdditiveVelocity += Adjustment;
//// 	// }
////
//// 	// Prepare Root Motion (generate/accumulate from root motion sources to be used later)
//// 	if (bHasRootMotionSources && !CharacterOwner->bClientUpdating && !CharacterOwner->bServerMoveIgnoreRootMotion)
//// 	{
//// 		// Animation root motion - If using animation RootMotion, tick animations before running physics.
//// 		if( CharacterOwner->IsPlayingRootMotion() && CharacterOwner->GetMesh() )
//// 		{
//// 			TickCharacterPose(DeltaSeconds);
////
//// 			// Make sure animation didn't trigger an event that destroyed us
//// 			if (!HasValidData())
//// 			{
//// 				return;
//// 			}
////
//// 			// For local human clients, save off root motion data so it can be used by movement networking code.
//// 			if( CharacterOwner->IsLocallyControlled() && (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy) && CharacterOwner->IsPlayingNetworkedRootMotionMontage() )
//// 			{
//// 				CharacterOwner->ClientRootMotionParams = RootMotionParams;
//// 			}
//// 		}
////
//// 		// Generates root motion to be used this frame from sources other than animation
//// 		{
//// 			SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceCalculate);
//// 			CurrentRootMotion.PrepareRootMotion(DeltaSeconds, *CharacterOwner, *this, true);
//// 		}
////
//// 		// For local human clients, save off root motion data so it can be used by movement networking code.
//// 		if( CharacterOwner->IsLocallyControlled() && (CharacterOwner->GetLocalRole() == ROLE_AutonomousProxy) )
//// 		{
//// 			CharacterOwner->SavedRootMotion = CurrentRootMotion;
//// 		}
//// 	}
////
//// 	// Apply Root Motion to Velocity
//// 	if( CurrentRootMotion.HasOverrideVelocity() || HasAnimRootMotion() )
//// 	{
//// 		// Animation root motion overrides Velocity and currently doesn't allow any other root motion sources
//// 		if( HasAnimRootMotion() )
//// 		{
//// 			// Convert to world space (animation root motion is always local)
//// 			USkeletalMeshComponent * SkelMeshComp = CharacterOwner->GetMesh();
//// 			if( SkelMeshComp )
//// 			{
//// 				// Convert Local Space Root Motion to world space. Do it right before used by physics to make sure we use up to date transforms, as translation is relative to rotation.
//// 				RootMotionParams.Set( ConvertLocalRootMotionToWorld(RootMotionParams.GetRootMotionTransform(), DeltaSeconds) );
//// 			}
////
//// 			// Then turn root motion to velocity to be used by various physics modes.
//// 			if( DeltaSeconds > 0.f )
//// 			{
//// 				AnimRootMotionVelocity = CalcAnimRootMotionVelocity(RootMotionParams.GetRootMotionTransform().GetTranslation(), DeltaSeconds, Velocity);
//// 				Velocity = ConstrainAnimRootMotionVelocity(AnimRootMotionVelocity, Velocity);
//// 				if (IsFalling())
//// 				{
//// 					Velocity += FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f);
//// 				}
//// 			}
//// 			
//// 			UE_LOG(LogRootMotion, Log,  TEXT("PerformMovement WorldSpaceRootMotion Translation: %s, Rotation: %s, Actor Facing: %s, Velocity: %s")
//// 				, *RootMotionParams.GetRootMotionTransform().GetTranslation().ToCompactString()
//// 				, *RootMotionParams.GetRootMotionTransform().GetRotation().Rotator().ToCompactString()
//// 				, *CharacterOwner->GetActorForwardVector().ToCompactString()
//// 				, *Velocity.ToCompactString()
//// 				);
//// 		}
//// 		else
//// 		{
//// 			// We don't have animation root motion so we apply other sources
//// 			if( DeltaSeconds > 0.f )
//// 			{
//// 				SCOPE_CYCLE_COUNTER(STAT_CharacterMovementRootMotionSourceApply);
////
//// 				const FVector VelocityBeforeOverride = Velocity;
//// 				FVector NewVelocity = Velocity;
//// 				CurrentRootMotion.AccumulateOverrideRootMotionVelocity(DeltaSeconds, *CharacterOwner, *this, NewVelocity);
//// 				if (IsFalling())
//// 				{
//// 					NewVelocity += CurrentRootMotion.HasOverrideVelocityWithIgnoreZAccumulate() ? FVector(DecayingFormerBaseVelocity.X, DecayingFormerBaseVelocity.Y, 0.f) : DecayingFormerBaseVelocity;
//// 				}
//// 				Velocity = NewVelocity;
////
//// 			}
//// 		}
//// 	}
////
////
//// 	// NaN tracking
//// 	devCode(ensureMsgf(!Velocity.ContainsNaN(), TEXT("UCharacterMovementComponent::PerformMovement: Velocity contains NaN (%s)\n%s"), *GetPathNameSafe(this), *Velocity.ToString()));
////
//// 	// Clear jump input now, to allow movement events to trigger it for next update.
//// 	CharacterOwner->ClearJumpInput(DeltaSeconds);
//// 	NumJumpApexAttempts = 0;
////
//// 	// change position
//// 	StartNewPhysics(DeltaSeconds, 0);
////
//// 	if (!HasValidData())
//// 	{
//// 		return;
//// 	}
////
//// 	// Update character state based on change from movement
//// 	UpdateCharacterStateAfterMovement(DeltaSeconds);
////
//// 	if (bAllowPhysicsRotationDuringAnimRootMotion || !HasAnimRootMotion())
//// 	{
//// 		PhysicsRotation(DeltaSeconds);
//// 	}
////
//// 	// Apply Root Motion rotation after movement is complete.
//// 	if( HasAnimRootMotion() )
//// 	{
//// 		const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
//// 		const FQuat RootMotionRotationQuat = RootMotionParams.GetRootMotionTransform().GetRotation();
//// 		if( !RootMotionRotationQuat.IsIdentity() )
//// 		{
//// 			const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
//// 			MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
//// 		}
////
//// 		// Root Motion has been used, clear
//// 		RootMotionParams.Clear();
//// 	}
//// 	else if (CurrentRootMotion.HasActiveRootMotionSources())
//// 	{
//// 		FQuat RootMotionRotationQuat;
//// 		if (CharacterOwner && UpdatedComponent && CurrentRootMotion.GetOverrideRootMotionRotation(DeltaSeconds, *CharacterOwner, *this, RootMotionRotationQuat))
//// 		{
//// 			const FQuat OldActorRotationQuat = UpdatedComponent->GetComponentQuat();
//// 			const FQuat NewActorRotationQuat = RootMotionRotationQuat * OldActorRotationQuat;
//// 			MoveUpdatedComponent(FVector::ZeroVector, NewActorRotationQuat, true);
//// 		}
//// 	}
////
//// 	// consume path following requested velocity
//// 	bHasRequestedVelocity = false;
////
//// 	OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
//// } // End scoped movement update
////
//// 	// Call external post-movement events. These happen after the scoped movement completes in case the events want to use the current state of overlaps etc.
//// 	CallMovementUpdateDelegate(DeltaSeconds, OldLocation, OldVelocity);
////
//// 	if (CharacterMovementCVars::BasedMovementMode == 0)
//// 	{
//// 		SaveBaseLocation(); // behaviour before implementing this fix
//// 	}
//// 	else
//// 	{
//// 		MaybeSaveBaseLocation();
//// 	}
//// 	UpdateComponentVelocity();
////
//// 	const bool bHasAuthority = CharacterOwner && CharacterOwner->HasAuthority();
////
//// 	// If we move we want to avoid a long delay before replication catches up to notice this change, especially if it's throttling our rate.
//// 	if (bHasAuthority && UNetDriver::IsAdaptiveNetUpdateFrequencyEnabled() && UpdatedComponent)
//// 	{
//// 		UNetDriver* NetDriver = MyWorld->GetNetDriver();
//// 		if (NetDriver && NetDriver->IsServer())
//// 		{
//// 			FNetworkObjectInfo* NetActor = NetDriver->FindOrAddNetworkObjectInfo(CharacterOwner);
//// 				
//// 			if (NetActor && MyWorld->GetTimeSeconds() <= NetActor->NextUpdateTime && NetDriver->IsNetworkActorUpdateFrequencyThrottled(*NetActor))
//// 			{
//// 				if (ShouldCancelAdaptiveReplication())
//// 				{
//// 					NetDriver->CancelAdaptiveReplication(*NetActor);
//// 				}
//// 			}
//// 		}
//// 	}
////
//// 	const FVector NewLocation = UpdatedComponent ? UpdatedComponent->GetComponentLocation() : FVector::ZeroVector;
//// 	const FQuat NewRotation = UpdatedComponent ? UpdatedComponent->GetComponentQuat() : FQuat::Identity;
////
//// 	if (bHasAuthority && UpdatedComponent && !IsNetMode(NM_Client))
//// 	{
//// 		const bool bLocationChanged = (NewLocation != LastUpdateLocation);
//// 		const bool bRotationChanged = (NewRotation != LastUpdateRotation);
//// 		if (bLocationChanged || bRotationChanged)
//// 		{
//// 			// Update ServerLastTransformUpdateTimeStamp. This is used by Linear smoothing on clients to interpolate positions with the correct delta time,
//// 			// so the timestamp should be based on the client's move delta (ServerAccumulatedClientTimeStamp), not the server time when receiving the RPC.
//// 			const bool bIsRemotePlayer = (CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy);
//// 			const FNetworkPredictionData_Server_Character* ServerData = bIsRemotePlayer ? GetPredictionData_Server_Character() : nullptr;
//// 			if (bIsRemotePlayer && ServerData && CharacterMovementCVars::NetUseClientTimestampForReplicatedTransform)
//// 			{
//// 				ServerLastTransformUpdateTimeStamp = float(ServerData->ServerAccumulatedClientTimeStamp);
//// 			}
//// 			else
//// 			{
//// 				ServerLastTransformUpdateTimeStamp = MyWorld->GetTimeSeconds();
//// 			}
//// 		}
//// 	}
////
//// 	LastUpdateLocation = NewLocation;
//// 	LastUpdateRotation = NewRotation;
//// 	LastUpdateVelocity = Velocity;
//}
//
//void UMTD_MovementComponent::UpdateBasedMovement(float DeltaSeconds)
//{
//	if (!HasValidData())
//		return;
//
//	const UPrimitiveComponent *MovementBase = MtdPawnOwner->GetMovementBase();
//	if (!MovementBaseUtility::UseRelativeLocation(MovementBase))
//		return;
//
//	if (!IsValid(MovementBase) || !IsValid(MovementBase->GetOwner()))
//		return;
//
//	// Ignore collision with bases during these movements
//	TGuardValue<EMoveComponentFlags> ScopedFlagRestore(
//		MoveComponentFlags, MoveComponentFlags | MOVECOMP_IgnoreBases);
//
//	FQuat DeltaQuat = FQuat::Identity;
//	FVector DeltaPosition = FVector::ZeroVector;
//
//	// TODO: BasedMovement stuff in MTD_Pawn
//	FQuat NewBaseQuat;
//	FVector NewBaseLocation;
//	// if (!MovementBaseUtility::GetMovementBaseTransform(
//	// 	MovementBase, MtdPawnOwner->GetBasedMovement().BoneName, NewBaseLocation, 
//	// 	NewBaseQuat))
//	// {
//	// 	return;
//	// }
//
//	// Find change in rotation
//	const bool bRotationChanged = !OldBaseQuat.Equals(NewBaseQuat, 1e-8f);
//	if (bRotationChanged)
//	{
//		DeltaQuat = NewBaseQuat * OldBaseQuat.Inverse();
//	}
//
//	// Only if base moved
//	if (bRotationChanged || (OldBaseLocation != NewBaseLocation))
//	{
//		// Calculate new transform matrix of base actor (ignoring scale).
//		const FQuatRotationTranslationMatrix OldLocalToWorld(OldBaseQuat, OldBaseLocation);
//		const FQuatRotationTranslationMatrix NewLocalToWorld(NewBaseQuat, NewBaseLocation);
//
//		FQuat FinalQuat = UpdatedComponent->GetComponentQuat();
//			
//		if (bRotationChanged && !bIgnoreBaseRotation)
//		{
//			// Apply change in rotation and pipe through FaceRotation to
//			// maintain axis restrictions
//			const FQuat PawnOldQuat = UpdatedComponent->GetComponentQuat();
//			const FQuat TargetQuat = DeltaQuat * FinalQuat;
//			FRotator TargetRotator(TargetQuat);
//			MtdPawnOwner->FaceRotation(TargetRotator, 0.f);
//			FinalQuat = UpdatedComponent->GetComponentQuat();
//
//			if (PawnOldQuat.Equals(FinalQuat, 1e-6f))
//			{
//				// Nothing changed. This means we probably are using another
//				// rotation mechanism (bOrientToMovement etc).
//				// We should still follow the base object.
//				// @todo: This assumes only Yaw is used, currently a valid assumption. This is the only reason FaceRotation() is used above really, aside from being a virtual hook.
//				if (bOrientRotationToMovement ||
//					(bUseControllerDesiredRotation && MtdPawnOwner->Controller))
//				{
//					TargetRotator.Pitch = 0.f;
//					TargetRotator.Roll = 0.f;
//					MoveUpdatedComponent(FVector::ZeroVector, TargetRotator, false);
//					FinalQuat = UpdatedComponent->GetComponentQuat();
//				}
//			}
//
//			// Pipe through ControlRotation, to affect camera
//			if (MtdPawnOwner->Controller)
//			{
//				const FQuat PawnDeltaRotation =
//					FinalQuat * PawnOldQuat.Inverse();
//				FRotator FinalRotation = FinalQuat.Rotator();
//				UpdateBasedRotation(FinalRotation, PawnDeltaRotation.Rotator());
//				FinalQuat = UpdatedComponent->GetComponentQuat();
//			}
//		}
//
//		// We need to offset the base of the character here,
//		// not its origin, so offset by half height
//		float HalfHeight, Radius;
//		MtdPawnOwner->GetCapsuleComponent()->GetScaledCapsuleSize(
//			Radius, HalfHeight);
//
//		const FVector BaseOffset(0.0f, 0.0f, HalfHeight);
//		const FVector LocalBasePos = OldLocalToWorld.InverseTransformPosition(
//			UpdatedComponent->GetComponentLocation() - BaseOffset);
//		const FVector NewWorldPos = ConstrainLocationToPlane(
//			NewLocalToWorld.TransformPosition(LocalBasePos) + BaseOffset);
//		DeltaPosition = ConstrainDirectionToPlane(
//			NewWorldPos - UpdatedComponent->GetComponentLocation());
//
//		// move attached actor
//		// if (bFastAttachedMove)
//		// {
//		// 	// we're trusting no other obstacle can prevent the move here
//		// 	UpdatedComponent->SetWorldLocationAndRotation(NewWorldPos, FinalQuat, false);
//		// }
//		// else
//		// {
//			// hack - transforms between local and world space introducing slight error FIXMESTEVE - discuss with engine team: just skip the transforms if no rotation?
//			FVector BaseMoveDelta = NewBaseLocation - OldBaseLocation;
//			if (!bRotationChanged &&
//				(BaseMoveDelta.X == 0.f) &&
//				(BaseMoveDelta.Y == 0.f))
//			{
//				DeltaPosition.X = 0.f;
//				DeltaPosition.Y = 0.f;
//			}
//
//			FHitResult MoveOnBaseHit(1.f);
//			const FVector OldLocation =
//				UpdatedComponent->GetComponentLocation();
//			MoveUpdatedComponent(
//				DeltaPosition, FinalQuat, true, &MoveOnBaseHit);
//			if ((UpdatedComponent->GetComponentLocation() -
//				(OldLocation + DeltaPosition)).IsNearlyZero() == false)
//			{
//				OnUnableToFollowBaseMove(DeltaPosition, OldLocation, MoveOnBaseHit);
//			}
//		// }
//
//		if (MovementBase->IsSimulatingPhysics() && MtdPawnOwner->GetMesh())
//		{
//			MtdPawnOwner->GetMesh()->ApplyDeltaToAllPhysicsTransforms(
//				DeltaPosition, DeltaQuat);
//		}
//	}
//}
//
//void UMTD_MovementComponent::UpdateBasedRotation(
//	FRotator &FinalRotation, const FRotator &ReducedRotation)
//{
//	AController *Controller =
//		IsValid(MtdPawnOwner) ? PawnOwner->Controller : nullptr;
//	float ControllerRoll = 0.f;
//
//	if (IsValid(Controller) && !bIgnoreBaseRotation)
//	{
//		const FRotator ControllerRot = Controller->GetControlRotation();
//		ControllerRoll = ControllerRot.Roll;
//		Controller->SetControlRotation(ControllerRot + ReducedRotation);
//	}
//
//	// Remove roll
//	FinalRotation.Roll = 0.f;
//	if (IsValid(Controller))
//	{
//		check(IsValid(UpdatedComponent.Get()));
//		FinalRotation.Roll = UpdatedComponent->GetComponentRotation().Roll;
//		FRotator NewRotation = Controller->GetControlRotation();
//		NewRotation.Roll = ControllerRoll;
//		Controller->SetControlRotation(NewRotation);
//	}
//}
//
//void UMTD_MovementComponent::OnUnableToFollowBaseMove(
//	const FVector &DeltaPosition,
//	const FVector &OldLocation,
//	const FHitResult &MoveOnBaseHit)
//{
//	// no default implementation
//}
//
//void UMTD_MovementComponent::DisableMovement(bool Disable)
//{
//}
//
//bool UMTD_MovementComponent::HasValidData() const
//{
//	return IsValid(MtdPawnOwner) && IsValid(UpdatedComponent);
//}
//