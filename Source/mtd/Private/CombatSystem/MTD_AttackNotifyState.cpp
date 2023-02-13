#include "CombatSystem/MTD_AttackNotifyState.h"

#include "CombatSystem/MTD_CombatComponent.h"

void UMTD_AttackNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
    float TotalDuration)
{
    const AActor *Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        MTDS_WARN("Owner is invalid.");
        return;
    }
    
    UMTD_CombatComponent *CombatComponent = UMTD_CombatComponent::FindCombatComponent(Owner);
    if (!IsValid(CombatComponent))
    {
        MTDS_WARN("Owner [%s] does not have a combat component.", *Owner->GetName());
        return;
    }

    // Add our hitboxes to active list
    CombatComponent->ActivateHitboxes(MeleeHitboxNicknames);
}

void UMTD_AttackNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    const AActor *Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        MTDS_WARN("Owner is invalid.");
        return;
    }
    
    UMTD_CombatComponent *CombatComponent = UMTD_CombatComponent::FindCombatComponent(Owner);
    if (!IsValid(CombatComponent))
    {
        MTDS_WARN("Owner [%s] does not have a combat component.", *Owner->GetName());
        return;
    }

    // Remove our hitboxes from active list
    CombatComponent->DeactivateHitboxes(MeleeHitboxNicknames);
}
