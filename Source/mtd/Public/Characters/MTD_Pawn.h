//#pragma once
//
//#include "CoreMinimal.h"
//#include "GenericTeamAgentInterface.h"
//#include "GameFramework/Pawn.h"
//#include "MTD_Pawn.generated.h"
//
//// Forward declarations
//class UMTD_HealthComponent;
//class UMTD_MovementComponent;
//
//class UArrowComponent;
//class UCapsuleComponent;
//class USphereComponent;
//class UTextRenderComponent;
//
//UCLASS()
//class MTD_API AMTD_Pawn : public APawn, public IGenericTeamAgentInterface
//{
//	GENERATED_BODY()
//
//public:
//	AMTD_Pawn();
//
//protected:
//	virtual void BeginPlay() override;
//	virtual void InitDelegates();
//	virtual void InitNotifies();
//
//public:
//	virtual void OnHealthChanged(int32 NewHealth);
//
//protected:
//	UFUNCTION(BlueprintCallable, Category="MTD Attack")
//	virtual void OnAttack();
//
//	virtual void OnDeath();
//
//protected:
//	virtual void OnAttackBegin(USkeletalMeshComponent *MyMesh);
//	virtual void OnAttackEnd(USkeletalMeshComponent *MyMesh);
//	virtual void OnAttackAnimationTerminate(USkeletalMeshComponent *MyMesh);
//	
//	virtual void OnDeathAnimationTerminate(USkeletalMeshComponent *MyMesh);
//
//public:
//	UCapsuleComponent *GetCapsuleComponent() const
//		{ return CollisionCapsule; }
//	USkeletalMeshComponent *GetMesh() const
//		{ return Mesh; }
//	
//public:
//	virtual FGenericTeamId GetGenericTeamId() const override;
//
//protected:
//	UFUNCTION()
//	virtual void OnAttackBeginOverlap(
//		UPrimitiveComponent *OverlappedComponent,
//		AActor *OtherActor,
//		UPrimitiveComponent *OtherComp,
//		int32 OtherBodyIndex,
//		bool bFromSweep,
//		const FHitResult &SweepResult);
//	
//	UFUNCTION()
//	virtual void OnAttackEndOverlap(
//		UPrimitiveComponent *OverlappedComponent,
//		AActor *OtherActor,
//		UPrimitiveComponent *OtherComp,
//		int32 OtherBodyIndex);
//
//private:
//	void PlayAttackAnimation();
//	void PlayDeathAnimation();
//
//private:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Component",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<UCapsuleComponent> CollisionCapsule = nullptr;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Component",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<UArrowComponent> Arrow = nullptr;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<USphereComponent> AttackArea = nullptr;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Components",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<UMTD_MovementComponent> Movement = nullptr;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD Component",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<UMTD_HealthComponent> Health = nullptr;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Animations",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<UAnimMontage> AttackAnimMontage = nullptr;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Animations",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<UAnimMontage> DeathAnimMontage = nullptr;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Collision",
//		meta=(AllowPrivateAccess="true"))
//	FName PawnCollisionProfileName = "Pawn";
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Collision",
//		meta=(AllowPrivateAccess="true"))
//	FName ActivatedAttackCollisionProfileName = "Attack";
//	
//	UPROPERTY(VisibleAnywhere, Category="MTD Collision",
//		meta=(AllowPrivateAccess="true"))
//	FName DeactivatedAttackCollisionProfileName = "NoCollision";
//};
//