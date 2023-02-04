#pragma once

#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/GameModeBase.h"
#include "mtd.h"

#include "MTD_GameModeBase.generated.h"

struct FGameplayTagContainer;
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
    virtual void PostActorCreated() override;

    UFUNCTION(Exec)
    void AddExp(int32 Exp, int32 PlayerIndex = 0);
    
    UFUNCTION(Exec)
    void AddHealth(int32 Health, int32 PlayerIndex = 0);
    
    UFUNCTION(Exec)
    void AddMana(int32 Mana, int32 PlayerIndex = 0);
    
    UFUNCTION(Exec)
    void BroadcastExp(int32 Exp);

public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Game Mode")
    virtual AActor *GetGameTarget(APawn *Client) const;

protected:
    UFUNCTION(BlueprintCallable, Category="MTD|Game Mode")
    void TerminateGame(EMTD_GameResult Reason);

private:
    void StartListeningForPlayers();
    void OnActorSpawned(AActor *Actor);
    void OnAddPlayer(AActor *Actor);
    
    UFUNCTION()
    void OnRemovePlayer(AActor *Actor);
    void OnHeroClassesChanged(const FGameplayTagContainer &HeroClasses);

    AMTD_PlayerState *GetPlayerState(int32 PlayerIndex);

public:
    UPROPERTY(BlueprintAssignable)
    FOnGameTerminatedSignature OnGameTerminatedDelegate;

private:
    UPROPERTY()
    TArray<TObjectPtr<AMTD_PlayerState>> Players;
    
    bool bGameOver = false;
};
