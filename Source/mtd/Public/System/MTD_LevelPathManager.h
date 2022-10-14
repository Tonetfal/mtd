#pragma once

#include "GameFramework/Actor.h"
#include "mtd.h"
#include "MTD_CoreTypes.h"

#include "MTD_LevelPathManager.generated.h"

UCLASS()
class MTD_API AMTD_LevelPathManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AMTD_LevelPathManager();

protected:
	virtual void BeginPlay() override;

public:
	TArray<FVector> GetNavigationPath(FVector Location, FName PathChannel);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MTD Level Path Manager",
		meta=(AllowPrivateAccess="true"))
	// TMap<FName, FMTD_Path> Paths;
	TArray<FMTD_Path> Paths;
};
