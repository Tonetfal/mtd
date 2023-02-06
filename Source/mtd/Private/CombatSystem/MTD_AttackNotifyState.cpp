#include "CombatSystem/MTD_AttackNotifyState.h"

#include "Character/MTD_CombatComponent.h"

void UMTD_AttackNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
    float TotalDuration)
{
    const AActor *Owner = MeshComp->GetOwner();
    UMTD_CombatComponent *CombatComponent = UMTD_CombatComponent::FindCombatComponent(Owner);
    
    if (CombatComponent)
    {
        CombatComponent->AddMeleeHitboxes(MeleeHitboxNicknames);
    }
}

void UMTD_AttackNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    const AActor *Owner = MeshComp->GetOwner();
    UMTD_CombatComponent *CombatComponent = UMTD_CombatComponent::FindCombatComponent(Owner);
    
    if (CombatComponent)
    {
        CombatComponent->RemoveMeleeHitboxes(MeleeHitboxNicknames);
    }
}
