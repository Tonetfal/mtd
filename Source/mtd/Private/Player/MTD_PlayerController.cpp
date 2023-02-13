#include "Player/MTD_PlayerController.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Character/MTD_TeamComponent.h"
#include "Player/MTD_PlayerState.h"

AMTD_PlayerController::AMTD_PlayerController()
{
    // Tick to process input
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    Team = CreateDefaultSubobject<UMTD_TeamComponent>("MTD Team Component");
}

UMTD_AbilitySystemComponent *AMTD_PlayerController::GetMtdAbilitySystemComponent() const
{
    const AMTD_PlayerState *MtdPlayerState = GetMtdPlayerState();
    return ((IsValid(MtdPlayerState)) ? (MtdPlayerState->GetMtdAbilitySystemComponent()) : (nullptr));
}

AMTD_PlayerState *AMTD_PlayerController::GetMtdPlayerState() const
{
    return GetPlayerState<AMTD_PlayerState>();
}

AMTD_BasePlayerCharacter *AMTD_PlayerController::GetMtdPlayerCharacter() const
{
    return Cast<AMTD_BasePlayerCharacter>(GetCharacter());
}

void AMTD_PlayerController::AddYawInput(float Val)
{
    Super::AddYawInput(Val);

    // Save yaw input for SBS
    LastYaw = Val;
}

float AMTD_PlayerController::ConsumeLastYawRotation()
{
    // Return value
    const float Tmp = LastYaw;
    
    // Consume yaw
    LastYaw = 0.f;
    
    return Tmp;
}

void AMTD_PlayerController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);

    auto PlayerCharacter = CastChecked<AMTD_BasePlayerCharacter>(InPawn);
    
    // Notify about possessing a player character
    OnPossessDelegate.Broadcast(PlayerCharacter);
}

void AMTD_PlayerController::PostProcessInput(const float DeltaSeconds, const bool bGamePaused)
{
    UMTD_AbilitySystemComponent *MtdAbilitySystemComponent = GetMtdAbilitySystemComponent();
    if (IsValid(MtdAbilitySystemComponent))
    {
        // Process ability inputs beforehand
        MtdAbilitySystemComponent->ProcessAbilityInput(DeltaSeconds, bGamePaused);
    }

    Super::PostProcessInput(DeltaSeconds, bGamePaused);
}
