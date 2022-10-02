#pragma once

#include "mtd.h"
#include "Components/ActorComponent.h"

#include "MTD_PathFollowingComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDestinationSignature, bool bSuccess);

// Forward declarations
class AMTD_LevelPathManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_PathFollowingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMTD_PathFollowingComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category="MTD Path Following Component")
	void PreparePath();
	
	UFUNCTION(BlueprintCallable, Category="MTD Path Following Component")
	bool SelectNextPathPoint();
	
	UFUNCTION(BlueprintCallable, Category="MTD Path Following Component")
	FVector GetCurrentPathPoint() const
		{ return Path[PathIndex]; }

	UFUNCTION(BlueprintCallable, Category="MTD Path Following Component")
	virtual void OnMoveToFinished(bool bSuccess);
	
	UFUNCTION(BlueprintCallable, Category="MTD Path Following Component")
	virtual float GetAcceptanceRadius()
		{ return bFailedToGetFollowPoint ?
			FailedFollowPointAcceptanceRadius : FollowPointAcceptanceRadius; }

private:
	void AllowToPrepare();

public:
	FOnDestinationSignature OnDestinationDelegate;

private:
	UPROPERTY(EditAnywhere, Category="MTD Level Path Follower",
		meta=(AllowPrivateAccess="true"))
	FName PathChannel = FName(TEXT("None"));
	
	UPROPERTY(EditDefaultsOnly, Category="MTD Level Path Follower",
		meta=(AllowPrivateAccess="true"))
	float FollowPointAcceptanceRadius = 200.f;
	
	UPROPERTY(EditDefaultsOnly, Category="MTD Level Path Follower",
		meta=(AllowPrivateAccess="true"))
	float FailedFollowPointAcceptanceRadius = 500.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="MTD Level Path Follower",
		meta=(AllowPrivateAccess="true"))
	bool bFailedToGetFollowPoint = false;

	UPROPERTY()
	TObjectPtr<AMTD_LevelPathManager> PathManager = nullptr;

	TArray<FVector> Path;
	
	int32 PathIndex = 0;

	bool bIsAllowedToPrepare = true;
	
	FTimerHandle AllowToPrepareTimerHandle;
};
