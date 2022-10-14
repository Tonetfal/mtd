#include "Character/MTD_PawnExtensionComponent.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"

UMTD_PawnExtensionComponent::UMTD_PawnExtensionComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UMTD_PawnExtensionComponent::InitializeAbilitySystem(
	UMTD_AbilitySystemComponent *InAsc, AActor *InOwnerActor)
{
	check(InAsc);
	check(InOwnerActor);

	if (AbilitySystemComponent == InAsc)
	{
		// The ability system compont hasn't changed
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component
		UninitializeAbilitySystem();
	}

	APawn *Pawn = GetPawnChecked<APawn>();
	const AActor *ExistingAvatar = InAsc->GetAvatarActor();

	MTDS_VERBOSE("Settings up ASC [%s] on pawn [%s] owner [%s], existing [%s]",
		*GetNameSafe(InAsc), *GetNameSafe(Pawn),
		*GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if (IsValid(ExistingAvatar) && ExistingAvatar != Pawn)
	{
		UMTD_PawnExtensionComponent *OtherExtComp =
			FindPawnExtensionComponent(ExistingAvatar);
		if (IsValid(OtherExtComp))
		{
			OtherExtComp->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InAsc;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	OnAbilitySystemInitialized.Broadcast();
}

void UMTD_PawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
		return;

	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		AbilitySystemComponent->CancelAbilities(nullptr, nullptr);
		// AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor())
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			AbilitySystemComponent->ClearActorInfo();
		}

		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UMTD_PawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent &&
		AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>())
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor
			== AbilitySystemComponent->GetOwnerActor());
		if (!AbilitySystemComponent->GetOwnerActor())
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}
	
	CheckPawnReadyToInitialize();
}

void UMTD_PawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckPawnReadyToInitialize();
}

bool UMTD_PawnExtensionComponent::CheckPawnReadyToInitialize()
{
	if (bPawnReadyToInitialize)
	{
		return true;
	}

	const APawn *Pawn = GetPawnChecked<APawn>();

	const bool bHasAuthority = Pawn->HasAuthority();
	const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

	if (bHasAuthority || bIsLocallyControlled)
	{
		// Check for being possessed by a controller
		if (!IsValid(Pawn->GetController()))
		{
			return false;
		}
	}

	 // Allow pawn components to have requirements.
	 TArray<UActorComponent*> InteractableComponents =
		Pawn->GetComponentsByInterface(UMTD_ReadyInterface::StaticClass());
	 for (UActorComponent* InteractableComponent : InteractableComponents)
	 {
	 	const IMTD_ReadyInterface *Ready =
	 		CastChecked<IMTD_ReadyInterface>(InteractableComponent);
	 	if (!Ready->IsPawnComponentReadyToInitialize())
	 		return false;
	 }

	// Pawn is ready to initialize
	bPawnReadyToInitialize = true;
	OnPawnReadyToInitialize.Broadcast();
	BP_OnPawnReadyToInitialize.Broadcast();

	return true;
}

void UMTD_PawnExtensionComponent::OnPawnReadyToInitialize_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnPawnReadyToInitialize.IsBoundToObject(Delegate.GetUObject()))
	{
		OnPawnReadyToInitialize.Add(Delegate);
	}

	if (bPawnReadyToInitialize)
	{
		Delegate.Execute();
	}
}

void UMTD_PawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}

	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void UMTD_PawnExtensionComponent::OnAbilitySystemUninitialized_Register(
	FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

void UMTD_PawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn *Owner = GetPawn<APawn>();
	if (!IsValid(Owner))
	{
		MTDS_ERROR("MTD_PawnExtensionComponent on [%s] can only be added to "
			"Pawn actors", *GetNameSafe(GetOwner()));
		return;
	}

	TArray<UActorComponent*> PawnExtensionComponents;
	Owner->GetComponents(
		UMTD_PawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	if (PawnExtensionComponents.Num() != 1)
	{
		MTDS_ERROR("Only one MTD_PawnExtensionComponent should exist on [%s]",
			*GetNameSafe(GetOwner()));
	}
}
