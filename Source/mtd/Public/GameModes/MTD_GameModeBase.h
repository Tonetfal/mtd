#pragma once

#include "GameFramework/GameModeBase.h"
#include "mtd.h"

#include "MTD_GameModeBase.generated.h"

class AMTD_LevelPathManager;
class UMTD_LevelDefinition;

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
    UPROPERTY()
    TObjectPtr<AMTD_LevelPathManager> LevelPathManager = nullptr;

    // TODO: Tmp
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Level",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_LevelDefinition> LevelDefinition = nullptr;
};
