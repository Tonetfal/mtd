//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/PawnMovementComponent.h"
//
//#include "MTD_MovementComponent.generated.h"
//
//UCLASS()
//class MTD_API UMTD_MovementComponent : public UPawnMovementComponent
//{
//	GENERATED_BODY()
//
//protected:
//	virtual void BeginPlay() override;
//
//protected:
//	virtual void TickComponent(
//		float DeltaSeconds,
//		ELevelTick TickType,
//		FActorComponentTickFunction *ThisTickFunction) override;
//
//	virtual void ControlledCharacterMove(
//		const FVector &InputVector, float DeltaSeconds);
//	virtual FVector ScaleInputAcceleration(
//		const FVector& InputAcceleration) const;
//	virtual FVector ConstrainInputAcceleration(
//		const FVector& InputAcceleration) const;
//	virtual float ComputeAnalogInputModifier() const;
//	virtual void PerformMovement(float DeltaSeconds);
//	virtual void UpdateBasedMovement(float DeltaSeconds);
//	void UpdateBasedRotation(
//		FRotator &FinalRotation, const FRotator &ReducedRotation);
//	void OnUnableToFollowBaseMove(
//		const FVector &DeltaPosition,
//		const FVector &OldLocation,
//		const FHitResult &MoveOnBaseHit);
//
//	virtual void DisableMovement(bool Disable);
//
//private:
//	bool HasValidData() const;
//
//private:
//	// TODO:
//	// MaxStepHeight
//	// JumpZVelocity
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Character Movement (General Settigns)",
//		Transient, DuplicateTransient, meta=(AllowPrivateAccess="true"))
//	TObjectPtr<class AMTD_Pawn> MtdPawnOwner = nullptr;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Character Movement (General Settigns)",
//		meta=(AllowPrivateAccess="true"))
//	float GravityScale = 1.f;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Character Movement (General Settings)",
//		meta=(AllowPrivateAccess="true"))
//	float MaxAcceleration = 500.0f;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Character Movement (General Settigns)",
//		meta=(AllowPrivateAccess="true"))
//	bool bOrientRotationToMovement = false;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Character Movement (General Settigns)",
//		meta=(AllowPrivateAccess="true"))
//	bool bUseControllerDesiredRotation = true;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Character Movement (General Settigns)",
//		meta=(AllowPrivateAccess="true"))
//	bool bIgnoreBaseRotation = true;
//	
//	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Rotation",
//	// 	meta=(AllowPrivateAccess="true"))
//	// FRotator RotationRate { 0.0f, 0.0f, 0.0f };
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Character Movement: Walking",
//		meta=(AllowPrivateAccess="true"))
//	float MaxWalkSpeed = 500.0f;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Character Movement: Walking",
//		meta=(AllowPrivateAccess="true"))
//	float MaxWalkSpeedBackwards = 250.0f;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Character Movement:|Runtime",
//		meta=(AllowPrivateAccess="true"))
//	float Speed = 0.0f;
//	
//	/**
//	 * Current acceleration vector (with magnitude).
//	 * This is calculated each update based on the input vector and the constraints of MaxAcceleration and the current movement mode.
//	 */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD CharacterMovement:|Runtime",
//		meta=(AllowPrivateAccess="true"))
//	FVector Acceleration = FVector::ZeroVector;
//	
//	/**
//	 * Modifier to applied to values such as acceleration and max speed due to analog input.
//	 */
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD CharacterMovement:|Runtime",
//		meta=(AllowPrivateAccess="true"))
//	float AnalogInputModifier;
//
//	/** Saved location of object we are standing on, for UpdateBasedMovement() to determine if base moved in the last frame, and therefore pawn needs an update. */
//	FQuat OldBaseQuat;
//
//	/** Saved location of object we are standing on, for UpdateBasedMovement() to determine if base moved in the last frame, and therefore pawn needs an update. */
//	FVector OldBaseLocation;
//};
//