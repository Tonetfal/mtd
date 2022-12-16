#pragma once

#include "Character/MTD_TeamComponent.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "mtd.h"

#include "MTD_PlayerController.generated.h"

class AMTD_PlayerState;
class UMTD_AbilitySystemComponent;

UCLASS()
class MTD_API AMTD_PlayerController : public APlayerController, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    AMTD_PlayerController();

    UFUNCTION(BlueprintCallable, Category="MTD|Player Controller")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Player Controller")
    AMTD_PlayerState *GetMtdPlayerState() const;

    virtual void AddYawInput(float Val) override;

    UFUNCTION(BlueprintCallable, Category="MTD|Player Controller")
    float ConsumeLastYawRotation();

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn *InPawn) override;
    virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

public:
    virtual FGenericTeamId GetGenericTeamId() const override
    {
        return Team->GetGenericTeamId();
    }

private:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_TeamComponent> Team = nullptr;

    float LastYaw = 0.f;
};
