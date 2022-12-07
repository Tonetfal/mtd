#include "Items/MTD_ManaToken.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "Character/MTD_ManaComponent.h"
#include "Items/MTD_TokenMovementComponent.h"

void AMTD_ManaToken::Tick(float DeltaSeconds)
{
    const USceneComponent *TargetRoot = MovementComponent->HomingTargetComponent.Get();
    if (IsValid(TargetRoot))
    {
        const AActor *Target = TargetRoot->GetOwner();

        // Invalidate the target if it's mana pool has become full since last tick
        if (IsValid(Target))
        {
            ensure(Target->IsA(APawn::StaticClass()));
        
            const UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Target);
            if (ManaComponent->IsManaFull())
            {
                Target = nullptr;
            }
        }
    }

    Super::Tick(DeltaSeconds);
}

bool AMTD_ManaToken::CanBeActivatedOn(APawn *Pawn) const
{
    if (!IsValid(Pawn))
    {
        return false;
    }

    const UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Pawn);
    return !ManaComponent->IsManaFull();
}

void AMTD_ManaToken::OnActivate_Implementation(APawn *Pawn)
{
    Super::OnActivate_Implementation(Pawn);

    // Note: The cue and other stuff should be done in BPs

    check(Pawn);

    auto Interface = Cast<IAbilitySystemInterface>(Pawn);
    UAbilitySystemComponent *Asc = Interface->GetAbilitySystemComponent();
    
    // Grant mana
    Asc->ApplyModToAttribute(UMTD_ManaSet::GetManaAttribute(), EGameplayModOp::Additive, ManaAmount);
}

APawn *AMTD_ManaToken::FindNewTarget() const
{
    for (APawn *Pawn : DetectedPawns)
    {
        const UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Pawn);
        if (!ManaComponent->IsManaFull())
        {
            return Pawn;
        }
    }

    return nullptr;
}

void AMTD_ManaToken::OnPawnAdded(APawn *Pawn)
{
    Super::OnPawnAdded(Pawn);
    
    // Notify about the error as soon as possible
    ensureMsgf(IsValid(UMTD_ManaComponent::FindManaComponent(Pawn)),
        TEXT("Pawn [%s] does not have a UMTD_ManaComponent."), *Pawn->GetName());
    ensureMsgf(Cast<IAbilitySystemInterface>(Pawn), TEXT("Pawn [%s] does not implement IAbilitySystemInterface."),
        *Pawn->GetName());
}
