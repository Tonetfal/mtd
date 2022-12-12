#include "CombatSystem/MTD_AttackNotifyState.h"

#include "CombatSystem/MTD_MeleeEventsInterface.h"

void UMTD_AttackNotifyState::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation,
    float TotalDuration)
{
    AActor *Owner = MeshComp->GetOwner();
    auto Interface = Cast<IMTD_MeleeCharacterInterface>(Owner);
    
    if (Interface)
    {
        Interface->AddMeleeHitboxes(MeleeHitboxNicknames);
    }
}

void UMTD_AttackNotifyState::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    AActor *Owner = MeshComp->GetOwner();
    auto Interface = Cast<IMTD_MeleeCharacterInterface>(Owner);
    
    if (Interface)
    {
        Interface->RemoveMeleeHitboxes(MeleeHitboxNicknames);
    }
}
