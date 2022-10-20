#include "Character/MTD_BaseCharacter.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_HealthComponent.h"
#include "Character/MTD_HeroComponent.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/MTD_PlayerState.h"

AMTD_BaseCharacter::AMTD_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	GetMesh()->SetCollisionProfileName("NoCollision");

	PawnExtentionComponent =
		CreateDefaultSubobject<UMTD_PawnExtensionComponent>(
			TEXT("MTD Pawn Extension Component"));
	
	PawnExtentionComponent->OnAbilitySystemInitialized_RegisterAndCall(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(
			this, &ThisClass::OnAbilitySystemInitialized));
	
	PawnExtentionComponent->OnAbilitySystemUninitialized_Register(
		FSimpleMulticastDelegate::FDelegate::CreateUObject(
			this, &ThisClass::OnAbilitySystemUninitialized));
	
	HeroComponent =
		CreateDefaultSubobject<UMTD_HeroComponent>(
			TEXT("MTD Hero Component"));
	
	HealthComponent =
		CreateDefaultSubobject<UMTD_HealthComponent>(
			TEXT("MTD Health Component"));
}

void AMTD_BaseCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AMTD_BaseCharacter::PostInitProperties()
{
	Super::PostInitProperties();
	
}

void AMTD_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->OnDeathStarted.AddDynamic(
		this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(
		this, &ThisClass::OnDeathFinished);
}

void AMTD_BaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AMTD_BaseCharacter::Reset()
{
	Super::Reset();
}

void AMTD_BaseCharacter::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMTD_BaseCharacter::PreReplication(
	IRepChangedPropertyTracker &ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

void AMTD_BaseCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	PawnExtentionComponent->HandleControllerChanged();
}

void AMTD_BaseCharacter::SetupPlayerInputComponent(
	UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtentionComponent->SetupPlayerInputComponent();
}

void AMTD_BaseCharacter::FellOutOfWorld(const UDamageType &DamageType)
{
	HealthComponent->SelfDestruct(true);
}

void AMTD_BaseCharacter::OnAbilitySystemInitialized()
{
	UMTD_AbilitySystemComponent *MtdAsc = GetMtdAbilitySystemComponent();
	check(MtdAsc);

	HealthComponent->InitializeWithAbilitySystem(MtdAsc);
}

void AMTD_BaseCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void AMTD_BaseCharacter::DestroyDueToDeath()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	Uninit();
}

void AMTD_BaseCharacter::Uninit()
{
	// Uninit if we are still the avatar; we could be kicked from ASC if someone
	// else owned it instead
	const UMTD_AbilitySystemComponent *Asc = GetMtdAbilitySystemComponent();
	if (IsValid(Asc))
	{
		if (Asc->GetAvatarActor() == this)
		{
			PawnExtentionComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void AMTD_BaseCharacter::OnDeathStarted_Implementation(AActor *OwningActor)
{
	DisableControllerInput();
	DisableMovement();
	DisableCollision();
}

void AMTD_BaseCharacter::OnDeathFinished_Implementation(AActor *OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(
		this, &ThisClass::DestroyDueToDeath);
}

void AMTD_BaseCharacter::DisableControllerInput()
{
	if (!IsValid(Controller))
		return;

	Controller->SetIgnoreMoveInput(true);
	Controller->SetIgnoreLookInput(true);
}

void AMTD_BaseCharacter::DisableMovement()
{
	UCharacterMovementComponent *MoveComp = GetCharacterMovement();
	check(MoveComp);
	
	MoveComp->StopMovementImmediately();
}

void AMTD_BaseCharacter::DisableCollision()
{
	UCapsuleComponent *CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
}

AMTD_PlayerState *AMTD_BaseCharacter::GetMtdPlayerState() const
{
	return CastChecked<AMTD_PlayerState>(
		GetPlayerState(), ECastCheckedType::NullAllowed);
}

UMTD_AbilitySystemComponent*
	AMTD_BaseCharacter::GetMtdAbilitySystemComponent() const
{
	return PawnExtentionComponent->GetMtdAbilitySystemComponent();
}

UAbilitySystemComponent* AMTD_BaseCharacter::GetAbilitySystemComponent() const
{
	return GetMtdAbilitySystemComponent();
}
