#include "EquipmentSystem/MTD_EquipmentInstance.h"

#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "AbilitySystem/Attributes/MTD_BuilderSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EquipmentSystem/MTD_EquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "InventorySystem/Items/MTD_WeaponItemData.h"
#include "Player/MTD_PlayerState.h"

UWorld *UMTD_EquipmentInstance::GetWorld() const
{
    const APawn *OwningPawn = GetPawn();
    return (IsValid(OwningPawn)) ? (OwningPawn->GetWorld()) : (nullptr);
}

APawn *UMTD_EquipmentInstance::GetPawn() const
{
    if (!IsValid(Owner))
    {
        MTDS_WARN("Owner is invalid.");
        return nullptr;
    }
    
    const auto PlayerState = Cast<APlayerState>(Owner);
    if (!IsValid(PlayerState))
    {
        MTDS_WARN("Owner [%s] is not a player state.", *Owner->GetName());
        return nullptr;
    }
    
    const AController *Controller = PlayerState->GetOwningController();
    if (!IsValid(Controller))
    {
        MTDS_WARN("Player controller is invalid.");
        return nullptr;
    }

    APawn *Pawn = Controller->GetPawn();
    return Pawn;
}

APawn *UMTD_EquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
    APawn *Pawn = GetPawn();
    return ((PawnType) && (IsValid(Pawn)) && (Pawn->IsA(PawnType))) ? (Cast<APawn>(Pawn)) : (nullptr);
}

UAbilitySystemComponent *UMTD_EquipmentInstance::GetAbilitySystemComponent() const
{
    const auto MtdPs = Cast<AMTD_PlayerState>(Owner);
    return (IsValid(MtdPs)) ? (MtdPs->GetAbilitySystemComponent()) : (nullptr);
}

UMTD_AbilitySystemComponent *UMTD_EquipmentInstance::GetMtdAbilitySystemComponent() const
{
    return CastChecked<UMTD_AbilitySystemComponent>(GetAbilitySystemComponent());
}

EMTD_EquipmentType UMTD_EquipmentInstance::GetEquipmentType() const
{
    return EquipmentType;
}

void UMTD_EquipmentInstance::SpawnEquipmentActor(const FMTD_EquipmentActorDefinition &ActorToSpawn)
{
    if (IsValid(SpawnedActor))
    {
        MTDS_WARN("Equipment actor is already spawned.");
        return;
    }
    
    if (!ActorToSpawn.ActorClass)
    {
        MTDS_WARN("Actor class is NULL.");
        return;
    }

    const auto Character = GetPawn<ACharacter>();
    if (!IsValid(Character))
    {
        MTDS_WARN("Character is invalid.");
        return;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = Character;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Spawn actor
    auto NewActor = GetWorld()->SpawnActor(ActorToSpawn.ActorClass, nullptr, nullptr, SpawnParameters);
    if (!IsValid(NewActor))
    {
        MTDS_WARN("Failed to spawn an equipment actor.");
        return;
    }

    USceneComponent *AttachTarget = Character->GetMesh();
    check(IsValid(AttachTarget));
    
    // Attach actor as defined
    NewActor->SetActorRelativeTransform(ActorToSpawn.AttachTransform);
    NewActor->AttachToComponent(
        AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachToSocket);

    // Save spawned actor
    SpawnedActor = NewActor;
}

void UMTD_EquipmentInstance::DestroyEquipmentActor()
{
    if (!IsValid(GetSpawnedActor()))
    {
        MTDS_LOG("Equipment Actor is invalid.");
        return;
    }
    
    SpawnedActor->Destroy();
}

void UMTD_EquipmentInstance::OnEquipped(bool bModStats)
{
    K2_OnEquipped();

    if (bModStats)
    {
        GrantStats();
    }
}

void UMTD_EquipmentInstance::OnUnequipped(bool bModStats)
{
    K2_OnUnequipped();

    // Retrieve all granted effects and ability if unequipped
    UMTD_AbilitySystemComponent *MtdAbilitySystemComponent = GetMtdAbilitySystemComponent();
    GrantedHandles.TakeFromAbilitySystem(MtdAbilitySystemComponent);
    
    DestroyEquipmentActor();

    if (bModStats)
    {
        RetrieveStats();
    }
}

void UMTD_EquipmentInstance::ModStats(float Multiplier)
{
    if (!IsPlayer())
    {
        return;
    }

    UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Cannot modify stats to owner [%s] with an invalid ability system.", *GetNameSafe(GetOuter()));
        return;
    }

    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item data is invalid.");
        return;
    }
    
    ModStats_Internal(Multiplier, AbilitySystemComponent);

    MTDS_LOG("Hero [%s] has %s stats.", *GetNameSafe(GetOwner()),
        ((Multiplier > 0.f) ? (TEXT("received")) : (TEXT("lost"))));
}

void UMTD_EquipmentInstance::ModStats_Internal(float Multiplier, UAbilitySystemComponent *AbilitySystemComponent)
{
    check(IsValid(AbilitySystemComponent));
    check(AbilitySystemComponent->GetAttributeSet(UMTD_PlayerSet::StaticClass()));
    check(AbilitySystemComponent->GetAttributeSet(UMTD_BuilderSet::StaticClass()));

    // Modify owner's player and builder stats
    const auto EquippableData = CastChecked<UMTD_EquipItemData>(ItemData);
    MOD_ATTRIBUTE_BASE(Player, HealthStat, EquippableData->EquippableItemParameters.PlayerHealth);
    MOD_ATTRIBUTE_BASE(Player, DamageStat, EquippableData->EquippableItemParameters.PlayerDamage);
    MOD_ATTRIBUTE_BASE(Player, SpeedStat, EquippableData->EquippableItemParameters.PlayerDamage);
    
    MOD_ATTRIBUTE_BASE(Builder, HealthStat, EquippableData->EquippableItemParameters.TowerHealth);
    MOD_ATTRIBUTE_BASE(Builder, DamageStat, EquippableData->EquippableItemParameters.TowerDamage);
    MOD_ATTRIBUTE_BASE(Builder, RangeStat, EquippableData->EquippableItemParameters.TowerRange);
    MOD_ATTRIBUTE_BASE(Builder, SpeedStat, EquippableData->EquippableItemParameters.TowerSpeed);
}

bool UMTD_EquipmentInstance::IsPlayer() const
{
    if (!IsValid(Owner))
    {
        MTDS_WARN("Owner is invalid.");
        return false;
    }

    const auto PlayerState = Cast<APlayerState>(Owner);
    if (!IsValid(PlayerState))
    {
        MTDS_WARN("Owner is not a player state.");
        return false;
    }

    // Bots' equip must not have any stats, so there's nothing to modify
    const bool bBot = PlayerState->IsABot();
    return (!bBot);
}

void UMTD_EquipmentInstance::GrantStats()
{
    ModStats(+1.f);
}

void UMTD_EquipmentInstance::RetrieveStats()
{
    ModStats(-1.f);
}
