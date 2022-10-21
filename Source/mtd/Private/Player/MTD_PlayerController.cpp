#include "Player/MTD_PlayerController.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_TeamComponent.h"
#include "Player/MTD_PlayerState.h"

AMTD_PlayerController::AMTD_PlayerController()
{
	// Has to process input each tick
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	Team = CreateDefaultSubobject<UMTD_TeamComponent>("MTD Team Component");
}

UMTD_AbilitySystemComponent*
	AMTD_PlayerController::GetMtdAbilitySystemComponent() const
{
	const AMTD_PlayerState *MtdPs = GetMtdPlayerState();
	return IsValid(MtdPs) ? MtdPs->GetMtdAbilitySystemComponent() : nullptr;
}

AMTD_PlayerState* AMTD_PlayerController::GetMtdPlayerState() const
{
	return GetPlayerState<AMTD_PlayerState>();
}

void AMTD_PlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMTD_PlayerController::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);
}

void AMTD_PlayerController::PostProcessInput(
	const float DeltaTime, const bool bGamePaused)
{
	UMTD_AbilitySystemComponent *MtdAsc = GetMtdAbilitySystemComponent();
	if (IsValid(MtdAsc))
	{
		MtdAsc->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime,bGamePaused);
}
