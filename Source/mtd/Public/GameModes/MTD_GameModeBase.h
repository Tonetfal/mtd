#pragma once

#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/GameModeBase.h"
#include "mtd.h"

#include "MTD_GameModeBase.generated.h"

class AMTD_PlayerState;
class ANavigationData;
class UMTD_LevelDefinition;
class UNavigationQueryFilter;
    
/**
 * Base game mode class used in this project.
 */

UCLASS()
class MTD_API AMTD_GameModeBase
    : public AGameModeBase
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnGameTerminatedSignature,
        EMTD_GameResult, GameResult);

public:
    AMTD_GameModeBase();

    UFUNCTION(Exec)
    void AddExp(int32 Exp, int32 PlayerIndex = 0);
    
    UFUNCTION(Exec)
    void BroadcastExp(int32 Exp);

    virtual void PreInitializeComponents() override;

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
    UPROPERTY()
    TArray<TObjectPtr<AMTD_PlayerState>> Players;
    
    bool bGameOver = false;
};
