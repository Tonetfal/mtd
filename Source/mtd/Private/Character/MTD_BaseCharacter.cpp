#include "Character/MTD_BaseCharacter.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_BalanceComponent.h"
#include "Character/MTD_CombatComponent.h"
#include "Character/MTD_HealthComponent.h"
#include "Character/MTD_HeroComponent.h"
#include "Character/MTD_ManaComponent.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Equipment/MTD_EquipmentManagerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/MTD_GameModeBase.h"
#include "Player/MTD_PlayerState.h"

AMTD_BaseCharacter::AMTD_BaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    GetMesh()->SetCollisionProfileName("NoCollision");

    PawnExtentionComponent = CreateDefaultSubobject<UMTD_PawnExtensionComponent>("MTD Pawn Extension Component");
    HeroComponent = CreateDefaultSubobject<UMTD_HeroComponent>("MTD Hero Component");
    HealthComponent = CreateDefaultSubobject<UMTD_HealthComponent>("MTD Health Component");
    ManaComponent = CreateDefaultSubobject<UMTD_ManaComponent>("MTD Mana Component");
    BalanceComponent = CreateDefaultSubobject<UMTD_BalanceComponent>("MTD Balance Component");
    CombatComponent = CreateDefaultSubobject<UMTD_CombatComponent>("MTD Combat Component");
    
    PawnExtentionComponent->OnAbilitySystemInitialized_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));

    PawnExtentionComponent->OnAbilitySystemUninitialized_Register(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
    HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
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
    
    UWorld *World = GetWorld();
    AGameModeBase *Gm = World->GetAuthGameMode();
    if (IsValid(Gm))
    {
        auto MtdGm = CastChecked<AMTD_GameModeBase>(Gm);
        MtdGm->OnGameTerminatedDelegate.AddDynamic(this, &ThisClass::OnGameTerminated);
    }
}

void AMTD_BaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AMTD_BaseCharacter::Reset()
{
    Super::Reset();
}

void AMTD_BaseCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}

void AMTD_BaseCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    PawnExtentionComponent->HandleControllerChanged();
}

float AMTD_BaseCharacter::GetMovementDirectionAngle() const
{
    const FVector Velocity = GetVelocity();
    if (Velocity.IsZero())
    {
        return 0.f;
    }

    const FVector Forward = GetActorForwardVector();
    const FVector Direction = Velocity.GetUnsafeNormal();
    
    const float Dot = Forward | Direction;
    const FVector Cross = Forward ^ Direction;
    
    const float Degrees = FMath::RadiansToDegrees(FMath::Acos(Dot));
    return (Cross.IsZero()) ? (Degrees) : (Degrees * FMath::Sign(Cross.Z));
}

void AMTD_BaseCharacter::InitializeAttributes()
{
    // Empty
}

void AMTD_BaseCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
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
    ManaComponent->InitializeWithAbilitySystem(MtdAsc);
    BalanceComponent->InitializeWithAbilitySystem(MtdAsc);
}

void AMTD_BaseCharacter::OnAbilitySystemUninitialized()
{
    HealthComponent->UninitializeFromAbilitySystem();
    ManaComponent->UninitializeFromAbilitySystem();
    BalanceComponent->UninitializeFromAbilitySystem();
}

void AMTD_BaseCharacter::DestroyDueToDeath()
{
    DetachFromControllerPendingDestroy();
    SetLifeSpan(0.1f);

    Uninit();
}

void AMTD_BaseCharacter::Uninit()
{
    // Uninit if we are still the avatar; we could be kicked from ASC if someone else owned it instead
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
    // Empty
}

void AMTD_BaseCharacter::OnDeathFinished_Implementation(AActor *OwningActor)
{
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
    
    AMTD_PlayerState *MtdPs = GetMtdPlayerState();
    if (!IsValid(MtdPs))
    {
        MTDS_WARN("MTD Player State is invalid.");
        return;
    }
    
    UMTD_EquipmentManagerComponent *EquipManager = MtdPs->GetEquipmentManagerComponent();
    if (!IsValid(EquipManager))
    {
        MTDS_WARN("Equipment Manager Component is invalid.");
        return;
    }
    
    EquipManager->UnequipAll();
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
