#include "Player/MTD_PlayerState.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_CombatSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "Equipment/MTD_EquipmentManagerComponent.h"
#include "Inventory/MTD_InventoryManagerComponent.h"
#include "Player/MTD_PlayerController.h"

AMTD_PlayerState::AMTD_PlayerState()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    AbilitySystemComponent = CreateDefaultSubobject<UMTD_AbilitySystemComponent>("MTD Ability System Component");
    EquipmentManagerComponent = CreateDefaultSubobject<UMTD_EquipmentManagerComponent>("MTD Equipment Component");
    InventoryManagerComponent = CreateDefaultSubobject<UMTD_InventoryManagerComponent>("MTD Inventory Manager Component");

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

void AMTD_PlayerState::GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, int32 InputCode)
{
    if ((IsValid(AbilitySystemComponent)) && (IsValid(AbilityClass)))
    {
        auto Ability = AbilityClass->GetDefaultObject<UGameplayAbility>();

        if (IsValid(Ability))
        {
            const FGameplayAbilitySpec AbilitySpec(Ability, Level, InputCode);
            AbilitySystemComponent->GiveAbility(AbilitySpec);
        }
    }
}

void AMTD_PlayerState::ActivateAbility(int32 InputCode)
{
    if (!IsValid(AbilitySystemComponent))
    {
        return;
    }

    AbilitySystemComponent->AbilityLocalInputPressed(InputCode);
}

void AMTD_PlayerState::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AMTD_PlayerState::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    check(AbilitySystemComponent);
    AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}
