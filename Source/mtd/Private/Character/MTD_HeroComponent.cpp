#include "Character/MTD_HeroComponent.h"

#include "Character/MTD_PawnExtensionComponent.h"
#include "Player/MTD_PlayerController.h"
#include "Player/MTD_PlayerState.h"

UMTD_HeroComponent::UMTD_HeroComponent()
{
}

void UMTD_HeroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMTD_HeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const auto Pawn = GetPawn<APawn>();
	if (IsValid(Pawn))
	{
		auto ExtComp =
			UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Pawn);
		if (IsValid(ExtComp))
		{
			ExtComp->UninitializeAbilitySystem();
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UMTD_HeroComponent::OnPawnReadyToInitialize()
{
	// Don't initialize twice
	if (!ensure(!bPawnHasInitialized))
		return;

	const APawn *Pawn = GetPawn<APawn>();
	if (!IsValid(Pawn))
		return;
	
	AMTD_PlayerState *MtdPs = Pawn->GetPlayerStateChecked<AMTD_PlayerState>();
	check(MtdPs);
	
	UMTD_PawnExtensionComponent *PawnExtComp =
		UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Pawn);
	if (IsValid(PawnExtComp))
	{
		PawnExtComp->InitializeAbilitySystem(
			MtdPs->GetMtdAbilitySystemComponent(), MtdPs);
	}

	auto *MtdPc = Pawn->GetController<AMTD_PlayerController>();
	if (IsValid(MtdPc))
	{
		if (!Pawn->InputComponent)
		{
			// InitializePlayerInput(Pawn->InputComponent);
		}
	}

	// if (Owner->IsLocallyControlled() && PawnData)
	// {
	//	auto CameraComponent = UMTD_CameraComponent::FindCameraComponent(Owner);
	// 	if (IsValid(CameraComponent))
	// 	{
	// 		CameraComponent->DetermineCameraModeDelegate.BindUObject(
	// 			this, &ThisClass::DetermineCameraMode);
	// 	}
	// }

	bPawnHasInitialized = true;
}

bool UMTD_HeroComponent::IsPawnComponentReadyToInitialize() const
{
	const APawn *Pawn = GetPawn<APawn>();

	// A pawn is required
	if (!IsValid(Pawn))
		return false;
	
	// If we're authority or autonomous, we need to wait for a controller with
	// registered ownership of the player state
	if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
	{
		const AController* Controller = GetController<AController>();

		const bool bHasControllerPairedWithPS =
			(Controller) &&
			(Controller->PlayerState) &&
			(Controller->PlayerState->GetOwner() == Controller);

		if (!bHasControllerPairedWithPS)
			return false;
	}

	return true;
}

void UMTD_HeroComponent::OnRegister()
{
	Super::OnRegister();

	const APawn *Owner = GetPawn<APawn>();
	if (!IsValid(Owner))
	{
		MTDS_ERROR("Owner [%s] must be a Pawn actor", *GetNameSafe(GetOwner()));
		return;
	}

	auto *PawnExtComp =
		UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Owner);

	if (!IsValid(PawnExtComp))
	{
		MTDS_ERROR("Owner [%s] must have MTD_PawnExtensionComponent",
			*GetNameSafe(Owner));
		return;
	}

	PawnExtComp->OnPawnReadyToInitialize_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(
			this, &ThisClass::OnPawnReadyToInitialize));
}
