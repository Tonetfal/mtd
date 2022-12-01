#pragma once

#include "AI/Navigation/NavQueryFilter.h"
#include "GameFramework/GameModeBase.h"
#include "mtd.h"

#include "MTD_GameModeBase.generated.h"

class UNavigationQueryFilter;
class ANavigationData;
class UMTD_LevelDefinition;

UENUM(BlueprintType)
enum class EMTD_GameResult : uint8
{
    Lose,
    Win
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnGameTerminatedSignature,
    EMTD_GameResult, GameResult);

UCLASS()
class MTD_API AMTD_GameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    AMTD_GameModeBase();

protected:
    virtual void BeginPlay() override;
    virtual void StartPlay() override;

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Game Mode")
    virtual AActor *GetGameTarget(APawn *Client) const;

protected:
    void TerminateGame(EMTD_GameResult Reason);

public:
    UPROPERTY(BlueprintAssignable)
    FOnGameTerminatedSignature OnGameTerminatedDelegate;

private:
    // TODO: Move in an appropriate place instead. Temporary solution.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Level",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_LevelDefinition> LevelDefinition = nullptr;

    bool bGameOver = false;
};
