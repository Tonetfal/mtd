//#pragma once
//
//#include "Characters/MTD_BaseCharacter.h"
//#include "mtd.h"
//
//#include "MTD_BaseEnemy.generated.h"
//
//class USphereComponent;
//class UMTD_WeaponComponent;
//class UBehaviorTree;
//DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewAttackTargetSpottedSignature,AActor*);
//
//UCLASS()
//class MTD_API AMTD_BaseEnemy : public AMTD_BaseCharacter
//{
//	GENERATED_BODY()
//
//public:
//	AMTD_BaseEnemy();
//
//protected:
//	virtual void PostInitProperties() override;
//	virtual void BeginPlay() override;
//	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
//	virtual void InitDelegates();
//
//public:
//	virtual void Tick(float DeltaSeconds) override;
//	
//	UFUNCTION(BlueprintCallable, Category="MTD|Enemy")
//	virtual void Attack() {	};
//	
//	UBehaviorTree *GetBehaviorTree() const
//		{ return BehaviorTree; }
//
//protected:
//	virtual int32 CleanAttackTargets();
//	virtual AActor *SelectAttackTarget();
//	virtual FHitResult GetTraceResultByLookingAt(FVector TargetLocation) const;
//	virtual bool SeesAttackTarget() const;
//	
//	UFUNCTION(BlueprintCallable, Category="MTD|Functions")
//	virtual void OnTargetNonReacheable();
//	
//	UFUNCTION()
//	virtual void OnAttackTargetDied();
//	virtual void OnAttackTargetIgnore(bool bOverrideAttackTarget = true);
//	virtual void OnIgnoreTargetClear();
//	
//	UFUNCTION()
//	virtual void OnSightSphereBeginOverlap(
//		UPrimitiveComponent *OverlappedComponent,
//		AActor *OtherActor,
//		UPrimitiveComponent *OtherComp,
//		int32 OtherBodyIndex,
//		bool bFromSweep,
//		const FHitResult &SweepResult);
//
//	UFUNCTION()
//	virtual void OnLoseSightSphereEndOverlap(
//		UPrimitiveComponent *OverlappedComponent,
//		AActor *OtherActor,
//		UPrimitiveComponent *OtherComp,
//		int32 OtherBodyIndex);
//	
//	virtual void OnDestination(bool bSuccess);
//
//	UFUNCTION()
//	virtual void OnHealthChanged(
//		UMTD_HealthComponent *InHealthComponent, 
//		float OldValue,
//		float NewValue,
//		AActor *InInstigator);
//
//	virtual void SetNewAttackTarget(AActor *Actor, bool bForceNewTarget = false);
//	virtual void OnStartForceTargetting(float Duration);
//	virtual void OnStopForceTargetting();
//
//	virtual void OnTryChangeTarget();
//
//private:
//	FVector GetDirectionToAttackTarget() const;
//
//public:
//	FOnNewAttackTargetSpottedSignature OnNewAttackTargetSpottedDelegate;
//
//private:
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<USphereComponent> SightSphere = nullptr;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<USphereComponent> LoseSightSphere = nullptr;
//	
//	/** Sphere we can detect players/towers in */
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD|AI",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<UBehaviorTree> BehaviorTree = nullptr;
//	
//	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|AI",
//		meta=(AllowPrivateAccess="true"))
//	float AttackAcceptanceRadius = 100.f;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Detection Component|Runtime",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<AActor> AttackTarget = nullptr;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Detection Component|Runtime",
//		meta=(AllowPrivateAccess="true"))
//	TObjectPtr<AActor> IgnoredActor = nullptr;
//	
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Detection Component|Runtime",
//		meta=(AllowPrivateAccess="true"))
//	bool bForceTarget = false;
//
//	UPROPERTY()
//	TArray<AActor*> DetectedTargets;
//
//	UPROPERTY()
//	TMap<TObjectPtr<AActor>, int32> LastDamageDealers;
//
//	FTimerHandle StopForceTargettingTimerHandle;
//	FTimerHandle TryChaneTargetTimerHandle;
//	FDelegateHandle TargetDiedDelegateHandle;
//
//	float SecondsToForceTarget = 5.f;
//	float SecondsToWaitBeforeTryingToChangeTarget = 2.f;
//};
//