#include "Equipment/MTD_EquipmentInstance.h"

#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "AbilitySystem/Attributes/MTD_BuilderSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Equipment/MTD_EquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/Items/MTD_WeaponItemData.h"
#include "Player/MTD_PlayerState.h"

UMTD_EquipmentInstance::UMTD_EquipmentInstance()
{
}

UWorld *UMTD_EquipmentInstance::GetWorld() const
{
    APawn *OwningPawn = GetPawn();
    return (IsValid(OwningPawn)) ? (OwningPawn->GetWorld()) : (nullptr);
}

APawn *UMTD_EquipmentInstance::GetPawn() const
{
    if (!IsValid(Owner))
    {
        MTDS_WARN("Owner is invalid.");
        return nullptr;
    }
    
    const auto Ps = Cast<APlayerState>(Owner);
    if (!IsValid(Ps))
    {
        MTDS_WARN("Owner [%s] is not a Player State.", *Owner->GetName());
        return nullptr;
    }
    
    const AController *Controller = Ps->GetOwningController();
    if (!IsValid(Controller))
    {
        MTDS_WARN("Player Controller is invalid.");
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
    if (!ActorToSpawn.ActorClass)
    {
        MTDS_WARN("ActorClass is NULL.");
        return;
    }

    auto Character = GetPawn<ACharacter>();
    if (!IsValid(Character))
    {
        MTDS_WARN("Character is invalid.");
        return;
    }
    
    auto NewActor = GetWorld()->SpawnActorDeferred<AActor>(ActorToSpawn.ActorClass, FTransform::Identity, Character);
    if (!IsValid(NewActor))
    {
        MTDS_WARN("Failed to spawn an equipment actor.");
        return;
    }

    USceneComponent *AttachTarget = Character->GetMesh();
    NewActor->FinishSpawning(FTransform::Identity, true);
    NewActor->SetActorRelativeTransform(ActorToSpawn.AttachTransform);
    NewActor->AttachToComponent(
        AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, ActorToSpawn.AttachToSocket);

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
    
    UMTD_AbilitySystemComponent *MtdAsc = GetMtdAbilitySystemComponent();
    GrantedHandles.TakeFromAbilitySystem(MtdAsc);
    DestroyEquipmentActor();

    if (bModStats)
    {
        TakeBackStats();
    }
}

void UMTD_EquipmentInstance::ModStats(float Multiplier)
{
    if (!IsPlayer())
    {
        return;
    }

    UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
    if (!IsValid(Asc))
    {
        MTDS_WARN("Cannot modify stats to owner [%s] with a NULL ability system.", *GetNameSafe(GetOuter()));
        return;
    }

    if (!IsValid(ItemData))
    {
        MTDS_WARN("Item Data is invalid.");
        return;
    }
    
    ModStats_Internal(Multiplier, Asc);

    MTDS_LOG("Hero [%s] has %s stats.", *GetNameSafe(GetOwner()),
        ((Multiplier > 0.f) ? (TEXT("received")) : (TEXT("lost"))));
}

void UMTD_EquipmentInstance::ModStats_Internal(float Multiplier, UAbilitySystemComponent *Asc)
{
    check(Asc->GetAttributeSet(UMTD_PlayerSet::StaticClass()));
    check(Asc->GetAttributeSet(UMTD_BuilderSet::StaticClass()));
    
    const auto EquippableData = Cast<UMTD_EquippableItemData>(ItemData);
    check(EquippableData);

    MOD_ATTRIBUTE_BASE(Player, HealthStat, EquippableData->PlayerHealth);
    MOD_ATTRIBUTE_BASE(Player, DamageStat, EquippableData->PlayerDamage);
    MOD_ATTRIBUTE_BASE(Player, SpeedStat, EquippableData->PlayerDamage);
    
    MOD_ATTRIBUTE_BASE(Builder, HealthStat, EquippableData->TowerHealth);
    MOD_ATTRIBUTE_BASE(Builder, DamageStat, EquippableData->TowerDamage);
    MOD_ATTRIBUTE_BASE(Builder, RangeStat, EquippableData->TowerRange);
    MOD_ATTRIBUTE_BASE(Builder, SpeedStat, EquippableData->TowerSpeed);
}

bool UMTD_EquipmentInstance::IsPlayer() const
{
    if (!IsValid(Owner))
    {
        MTDS_WARN("Owner is invalid.");
        return false;
    }

    const auto Ps = Cast<APlayerState>(Owner);
    if (!IsValid(Ps))
    {
        MTDS_WARN("Owner is not a Player State.");
        return false;
    }

    // Bots' equip must not have any stats, so there's nothing to modify
    const bool bBot = Ps->IsABot();
    return (!bBot);
}

void UMTD_EquipmentInstance::GrantStats()
{
    ModStats(+1.f);
}

void UMTD_EquipmentInstance::TakeBackStats()
{
    ModStats(-1.f);
}
