#pragma once

#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/GameModeBase.h"
#include "mtd.h"

#include "MTD_GameModeBase.generated.h"

class AMTD_PlayerState;
class ANavigationData;
class UMTD_LevelDefinition;
class UNavigationQueryFilter;
struct FGameplayTagContainer;

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

    //~AActor Interface
    virtual void PostActorCreated() override;
    //~End of AActor Interface

    /**
     * [Cheat] Add experience to a player.
     *
     * @param   Exp: experience to give
     * @param   PlayerIndex: player to give experience to.
     */
    UFUNCTION(Exec)
    void AddExp(int32 Exp, int32 PlayerIndex = 0);
    
    /**
     * [Cheat] Add health to a player.
     *
     * @param   Health: health to give
     * @param   PlayerIndex: player to give health to.
     */
    UFUNCTION(Exec)
    void AddHealth(int32 Health, int32 PlayerIndex = 0);
    
    /**
     * [Cheat] Add mana to a player.
     *
     * @param   Mana: mana to give
     * @param   PlayerIndex: player to give mana to.
     */
    UFUNCTION(Exec)
    void AddMana(int32 Mana, int32 PlayerIndex = 0);
    
    /**
     * [Cheat] Add experience to all player.
     *
     * @param   Exp: experience to give
     */
    UFUNCTION(Exec)
    void BroadcastExp(int32 Exp);

    /**
     * Get target AI has to follow.
     * @param   Client: AI pawn that is asking for a target.
     * @return  Game target. May be nullptr.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Game Mode")
    virtual AActor *GetGameTarget(APawn *Client) const;

protected:
    /**
     * Terminate game session.
     * @param   Reason: reason the game has come to termination for.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Game Mode")
    void TerminateGame(EMTD_GameResult Reason);

private:
    /**
     * Start listening for player spawns.
     */
    void ListenForPlayers();

    /**
     * Event to fire when an actor is spawned.
     * @param   Actor: spawned actor.
     */
    void OnActorSpawned(AActor *Actor);

    /**
     * Event to fire when a player is added.
     * @param   InPlayerState: spawned player state.
     */
    void OnAddPlayer(AMTD_PlayerState *InPlayerState);

    /**
     * Event to fire when a player is removed.
     * @param   Actor: removed player.
     */
    UFUNCTION()
    void OnRemovePlayer(AActor *Actor);

    /**
     * Event to fire when hero changes his class.
     * @param   HeroClasses: new hero classes.
     */
    void OnHeroClassesChanged(const FGameplayTagContainer &HeroClasses);

    /**
     * Get a player state using given player index.
     * @param   PlayerIndex: player index to search for.
     * @return  Player state. May be nullptr.
     */
    AMTD_PlayerState *GetPlayerState(int32 PlayerIndex);

public:
    /** Delegate to fire when game is terminated. */
    UPROPERTY(BlueprintAssignable)
    FOnGameTerminatedSignature OnGameTerminatedDelegate;

private:
    /** Container of all active player states. */
    UPROPERTY()
    TArray<TObjectPtr<AMTD_PlayerState>> Players;

    /** Is game over? */
    bool bGameOver = false;
};
