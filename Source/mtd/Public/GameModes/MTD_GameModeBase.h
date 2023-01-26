#pragma once

#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/GameModeBase.h"
#include "mtd.h"

#include "MTD_GameModeBase.generated.h"

class UNavigationQueryFilter;
class ANavigationData;
class UMTD_LevelDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnGameTerminatedSignature,
    EMTD_GameResult, GameResult);

UCLASS(meta=(ShortTooltip="The base Game Mode Base class used by this project."))
class MTD_API AMTD_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMTD_GameModeBase();

    UFUNCTION(Exec)
    void AddExp(int32 Exp, int32 PlayerIndex = 0);

protected:
    virtual void BeginPlay() override;
    virtual void StartPlay() override;

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Game Mode")
    virtual AActor *GetGameTarget(APawn *Client) const;

protected:
    UFUNCTION(BlueprintCallable, Category="MTD|Game Mode")
    void TerminateGame(EMTD_GameResult Reason);

public:
    UPROPERTY(BlueprintAssignable)
    FOnGameTerminatedSignature OnGameTerminatedDelegate;

private:
    // TODO: Move in an appropriate place instead. Temporary solution.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Level", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_LevelDefinition> LevelDefinition = nullptr;

    bool bGameOver = false;
};
