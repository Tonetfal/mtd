#include "Player/MTD_PlayerState.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Character/MTD_LevelComponent.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "EquipmentSystem/MTD_EquipmentManagerComponent.h"
#include "InventorySystem/MTD_InventoryManagerComponent.h"
#include "Player/MTD_PlayerController.h"

AMTD_PlayerState::AMTD_PlayerState()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    AbilitySystemComponent = CreateDefaultSubobject<UMTD_AbilitySystemComponent>("MTD Ability System Component");
    EquipmentManagerComponent = CreateDefaultSubobject<UMTD_EquipmentManagerComponent>("MTD Equipment Component");
    InventoryManagerComponent = CreateDefaultSubobject<UMTD_InventoryManagerComponent>("MTD Inventory Manager Component");

    if (!IsABot())
    {
        LevelComponent = CreateDefaultSubobject<UMTD_LevelComponent>("MTD Level Component");
    }

    CreateDefaultSubobject<UMTD_HealthSet>("HealthSet");
    CreateDefaultSubobject<UMTD_ManaSet>("ManaSet");
    CreateDefaultSubobject<UMTD_CombatSet>("CombatSet");
    CreateDefaultSubobject<UMTD_BalanceSet>("BalanceSet");
}

AMTD_PlayerController *AMTD_PlayerState::GetMtdPlayerController() const
{
    return Cast<AMTD_PlayerController>(GetOwner());
}

AMTD_BasePlayerCharacter *AMTD_PlayerState::GetMtdPlayerCharacter() const
{
    const AMTD_PlayerController *MtdPc = GetMtdPlayerController();
    return ((IsValid(MtdPc)) ? (MtdPc->GetMtdPlayerCharacter()) : (nullptr));
}

UAbilitySystemComponent *AMTD_PlayerState::GetAbilitySystemComponent() const
{
    return GetMtdAbilitySystemComponent();
}

void AMTD_PlayerState::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    check(IsValid(AbilitySystemComponent));

    // Set avatar
    AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

    if (IsABot())
    {
        return;
    }

    AMTD_PlayerController *MtdPlayerController = GetMtdPlayerController();
    if (!IsValid(MtdPlayerController))
    {
        MTDS_WARN("MTD player controller is invalid.");
        return;
    }

    // Listen for on possess event
    MtdPlayerController->OnPossessDelegate.AddDynamic(this, &ThisClass::OnControllerPossessed);
}

void AMTD_PlayerState::OnControllerPossessed(AMTD_BasePlayerCharacter *PlayerCharacter)
{
    auto PawnExtComp = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(PlayerCharacter);
    if (!IsValid(PawnExtComp))
    {
        MTDS_ERROR("Owner [%s] must have pawn extension component.", *GetNameSafe(PlayerCharacter));
        return;
    }

    // Call some custom initialization when ability system has been initialized by other components
    PawnExtComp->OnAbilitySystemInitialized_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
}

void AMTD_PlayerState::OnAbilitySystemInitialized()
{
    ensure(!IsABot());
    
    // Initialize level component
    LevelComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
}
