#include "UI/MTD_PlayerHudWidget.h"

#include "Character/MTD_HealthComponent.h"
#include "Player/MTD_PlayerController.h"

bool UMTD_PlayerHudWidget::Initialize()
{
	return Super::Initialize();
}

bool UMTD_PlayerHudWidget::IsPlayerAlive() const
{
	const auto Health =
		UMTD_HealthComponent::FindHealthComponent(GetOwningPlayer());
	return IsValid(Health) && !Health->IsDeadOrDying();
}

bool UMTD_PlayerHudWidget::IsPlayerSpectating() const
{
	const APlayerController *Pc = GetOwningPlayer();
	return IsValid(Pc) && Pc->GetStateName() == NAME_Spectating;
}
