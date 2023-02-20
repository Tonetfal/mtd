#include "Character/MTD_BaseCharacter.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_BalanceComponent.h"
#include "Character/MTD_HealthComponent.h"
#include "Character/MTD_HeroComponent.h"
#include "Character/MTD_ManaComponent.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "CombatSystem/MTD_CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "EquipmentSystem/MTD_EquipmentManagerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/MTD_GameModeBase.h"
#include "Player/MTD_PlayerState.h"

AMTD_BaseCharacter::AMTD_BaseCharacter()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    USkeletalMeshComponent *OurMesh = GetMesh();
    OurMesh->SetCollisionProfileName("NoCollision");

    PawnExtentionComponent = CreateDefaultSubobject<UMTD_PawnExtensionComponent>("MTD Pawn Extension Component");
    HeroComponent = CreateDefaultSubobject<UMTD_HeroComponent>("MTD Hero Component");
    HealthComponent = CreateDefaultSubobject<UMTD_HealthComponent>("MTD Health Component");
    ManaComponent = CreateDefaultSubobject<UMTD_ManaComponent>("MTD Mana Component");
    BalanceComponent = CreateDefaultSubobject<UMTD_BalanceComponent>("MTD Balance Component");
    CombatComponent = CreateDefaultSubobject<UMTD_CombatComponent>("MTD Combat Component");

    // Listen for death events
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
    HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
}

void AMTD_BaseCharacter::InitializeAttributes()
{
    // Empty
}

void AMTD_BaseCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Listen for ability system component initialization and uninitialization
    PawnExtentionComponent->OnAbilitySystemInitialized_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
    PawnExtentionComponent->OnAbilitySystemUninitialized_Register(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
}

void AMTD_BaseCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    UWorld *World = GetWorld();
    AGameModeBase *GameMode = World->GetAuthGameMode();
    if (IsValid(GameMode))
    {
        auto MtdGameMode = CastChecked<AMTD_GameModeBase>(GameMode);

        // Listen for game terminated event
        MtdGameMode->OnGameTerminatedDelegate.AddDynamic(this, &ThisClass::OnGameTerminated);
    }
}

void AMTD_BaseCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    // Notify pawn extension component about controller change
    PawnExtentionComponent->HandleControllerChanged();
}

void AMTD_BaseCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Notify pawn extension component about playet input component setup
    PawnExtentionComponent->SetupPlayerInputComponent();
}

void AMTD_BaseCharacter::OnAbilitySystemInitialized()
{
    UMTD_AbilitySystemComponent *MtdAbilitySystemComponent = GetMtdAbilitySystemComponent();
    check(IsValid(MtdAbilitySystemComponent));

    // Initialize our components that use ability system component
    HealthComponent->InitializeWithAbilitySystem(MtdAbilitySystemComponent);
    ManaComponent->InitializeWithAbilitySystem(MtdAbilitySystemComponent);
    BalanceComponent->InitializeWithAbilitySystem(MtdAbilitySystemComponent);
}

void AMTD_BaseCharacter::OnAbilitySystemUninitialized()
{
    HealthComponent->UninitializeFromAbilitySystem();
    ManaComponent->UninitializeFromAbilitySystem();
    BalanceComponent->UninitializeFromAbilitySystem();
}

void AMTD_BaseCharacter::DestroyDueToDeath()
{
    Uninit();
    DetachFromControllerPendingDestroy();
    SetLifeSpan(0.1f);
}

void AMTD_BaseCharacter::Uninit()
{
    // Uninit if we are still the avatar; we could be kicked from ASC if someone else owned it instead
    const UMTD_AbilitySystemComponent *AbilitySystemComponent = GetMtdAbilitySystemComponent();
    if (IsValid(AbilitySystemComponent))
    {
        const AActor * Avatar = AbilitySystemComponent->GetAvatarActor();
        if (Avatar == this)
        {
            PawnExtentionComponent->UninitializeAbilitySystem();
        }
    }

    SetActorHiddenInGame(true);
}

AMTD_PlayerState *AMTD_BaseCharacter::GetMtdPlayerState() const
{
    return CastChecked<AMTD_PlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UMTD_AbilitySystemComponent *AMTD_BaseCharacter::GetMtdAbilitySystemComponent() const
{
    return PawnExtentionComponent->GetMtdAbilitySystemComponent();
}

UAbilitySystemComponent *AMTD_BaseCharacter::GetAbilitySystemComponent() const
{
    return GetMtdAbilitySystemComponent();
}

float AMTD_BaseCharacter::GetMovementDirectionAngle() const
{
    const FVector Velocity = GetVelocity();
    if (Velocity.IsZero())
    {
        return 0.f;
    }

    // Work with normalized vectors
    const FVector Forward = GetActorForwardVector();
    const FVector Direction = Velocity.GetUnsafeNormal();

    // Find angle difference between two vectors in normalized form [0.0, 1.0]
    const float Dot = (Forward | Direction);

    // Find cross product to decide whether the direction is on left or right relatively to the forward
    const FVector Cross = (Forward ^ Direction);

    // Transform normalized angle in degrees
    const float Degrees = FMath::RadiansToDegrees(FMath::Acos(Dot));

    // Multiply degrees by cross Z sign to return the correct side
    return ((Cross.IsZero()) ? (Degrees) : (Degrees * FMath::Sign(Cross.Z)));
}

void AMTD_BaseCharacter::OnDeathStarted_Implementation(AActor *OwningActor)
{
    // Empty
}

void AMTD_BaseCharacter::OnDeathFinished_Implementation(AActor *OwningActor)
{
    DestroyDueToDeath();
    // GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AMTD_BaseCharacter::FellOutOfWorld(const UDamageType &DamageType)
{
    HealthComponent->SelfDestruct(true);
}
