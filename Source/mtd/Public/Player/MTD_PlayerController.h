#pragma once

#include "Character/Components/MTD_TeamComponent.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "mtd.h"

#include "MTD_PlayerController.generated.h"

class AMTD_BasePlayerCharacter;
class AMTD_PlayerState;
class UMTD_AbilitySystemComponent;

/**
 * Default player controller used in this project. 
 */
UCLASS()
class MTD_API AMTD_PlayerController
    : public APlayerController
    , public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
        FOnPossessSignature,
        AMTD_BasePlayerCharacter *, PlayerCharacter);

public:
    AMTD_PlayerController();

    /**
     * Get MTD ability system component.
     * @return  MTD ability system component.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Player Controller")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    /**
     * Get MTD player state.
     * @return  MTD player state.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Player Controller")
    AMTD_PlayerState *GetMtdPlayerState() const;

    /**
     * Get MTD player character.
     * @return  MTD player character.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Player Controller")
    AMTD_BasePlayerCharacter *GetMtdPlayerCharacter() const;

    /**
     * Consume last yaw rotation. Does not actually alter the rotation.
     * @return Last yaw rotation.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Player Controller")
    float ConsumeLastYawRotation();

    //~APlayerController Interface
    virtual void OnPossess(APawn *InPawn) override;;
    
    virtual void AddYawInput(float Val) override;

protected:
    virtual void PostProcessInput(const float DeltaSeconds, const bool bGamePaused) override;

public:
    virtual FGenericTeamId GetGenericTeamId() const override;
    //~End of APlayerController Interface

public:
    UPROPERTY(BlueprintAssignable)
    FOnPossessSignature OnPossessDelegate;

private:
    /** Component that associates us to a team. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TeamComponent> Team = nullptr;

    /** Last yaw rotation. */
    float LastYaw = 0.f;
};

inline FGenericTeamId AMTD_PlayerController::GetGenericTeamId() const
{
    return Team->GetGenericTeamId();
}
