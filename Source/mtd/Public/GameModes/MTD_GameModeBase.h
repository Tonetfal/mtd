#pragma once

#include "GameFramework/GameModeBase.h"
#include "mtd.h"

#include "MTD_GameModeBase.generated.h"

// Forward declarations
class AMTD_LevelPathManager;

UCLASS()
class MTD_API AMTD_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMTD_GameModeBase();

public:
	virtual void BeginPlay() override;
	virtual void StartPlay() override;

	AMTD_LevelPathManager *GetLevelPathManager() const;

private:
	UPROPERTY(VisibleAnywhere, Category="MTD Components")
	TObjectPtr<AMTD_LevelPathManager> LevelPathManager = nullptr;
};
