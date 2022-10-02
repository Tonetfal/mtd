#include "Controllers/MTD_PlayerController.h"

#include "Components/MTD_TeamComponent.h"

AMTD_PlayerController::AMTD_PlayerController()
{
	Team =
		CreateDefaultSubobject<UMTD_TeamComponent>("MTD Team Component");
}

void AMTD_PlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMTD_PlayerController::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);
}
