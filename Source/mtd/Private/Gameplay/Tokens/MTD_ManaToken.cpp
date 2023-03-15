#include "Gameplay/Tokens/MTD_ManaToken.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "Character/Components/MTD_ManaComponent.h"
#include "Gameplay/Tokens/MTD_TokenMovementComponent.h"
#include "Kismet/GameplayStatics.h"

bool AMTD_ManaToken::CanBeActivatedBy(APawn *Trigger) const
{
    if (!IsValid(Trigger))
    {
        return false;
    }

    // Mana token can be activated only if mana pool is not full
    const UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Trigger);
    const bool bIsManaFull = (!ManaComponent->IsManaFull());
    
    return bIsManaFull;
}

void AMTD_ManaToken::OnActivate_Implementation(APawn *Trigger)
{
    check(IsValid(Trigger));
    
    // Note: The cue and other visuals should be implemented in BPs

    // Give mana to trigger actor
    UAbilitySystemComponent *AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Trigger);
    AbilitySystemComponent->ApplyModToAttribute(UMTD_ManaSet::GetManaAttribute(), EGameplayModOp::Additive, ManaAmount);
    
    Super::OnActivate_Implementation(Trigger);
}

APawn *AMTD_ManaToken::FindNewTarget() const
{
    for (APawn *Pawn : DetectedTriggers)
    {
        const UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Pawn);
        if (!ManaComponent->IsManaFull())
        {
            // Only a trigger whose mana pool is not full can be triggered
            return Pawn;
        }
    }

    // Everyone has full mana pool
    return nullptr;
}

void AMTD_ManaToken::OnTriggerAdded(APawn *Trigger)
{
    // Notify about an error as soon as possible
    ensureMsgf(IsValid(UMTD_ManaComponent::FindManaComponent(Trigger)),
        TEXT("Pawn [%s] does not have a UMTD_ManaComponent."), *Trigger->GetName());
    ensureMsgf(Cast<IAbilitySystemInterface>(Trigger), TEXT("Pawn [%s] does not implement IAbilitySystemInterface."),
        *Trigger->GetName());
    
    // Make the pawn be homing target if there was none, and it's OK
    Super::OnTriggerAdded(Trigger);
}

void AMTD_ManaToken::SetNewTarget(APawn *Target)
{
    if (bDontHome)
    {
        return;
    }

    // We're not interested in old target anymore
    RemoveCurrentTargetFromListening();
    
    if (IsValid(Target))
    {
        // Bind a delegate to know when the target fill ups his mana pool
        AddToListening(Target);
    }
    
    Super::SetNewTarget(Target);
}

void AMTD_ManaToken::AddToListening(AActor *Trigger)
{
    if (IsValid(Trigger))
    {
        UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Trigger);
        check(IsValid(ManaComponent));

        // Listen for mana changes
        ManaComponent->OnManaChangedDelegate.AddDynamic(this, &ThisClass::OnTargetManaAttributeChanged);
        ManaComponent->OnMaxManaChangedDelegate.AddDynamic(this, &ThisClass::OnTargetMaxManaAttributeChanged);
    }
}

void AMTD_ManaToken::RemoveFromListening(AActor *Trigger)
{
    if (IsValid(Trigger))
    {
        UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Trigger);
        check(IsValid(ManaComponent));
        
        // Stop listening mana changes
        ManaComponent->OnManaChangedDelegate.RemoveDynamic(this, &ThisClass::OnTargetManaAttributeChanged);
        ManaComponent->OnMaxManaChangedDelegate.RemoveDynamic(this, &ThisClass::OnTargetMaxManaAttributeChanged);
    }
}

AMTD_ManaToken *SpawnManaToken(UWorld &World, AActor *Owner, const FTransform &SpawnTransform,
    const TSubclassOf<AMTD_ManaToken> &TokenClass, float TriggersIgnoreTime)
{
    if (!TokenClass.Get())
    {
        MTD_WARN("Token class is invalid.");
        return nullptr;
    }
    
    constexpr auto CollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    auto Mana = World.SpawnActorDeferred<AMTD_ManaToken>(TokenClass, SpawnTransform, Owner, nullptr, 
        CollisionHandlingMethod);
    
    if (!IsValid(Mana))
    {
        MTD_WARN("Failed to spawn mana token");
        return nullptr;
    }

    // Setup ignore time
    Mana->StopHomingFor(TriggersIgnoreTime);

    // Finilize token
    UGameplayStatics::FinishSpawningActor(Mana, SpawnTransform);
    
    return Mana;
}

TArray<AMTD_ManaToken *> AMTD_ManaToken::SpawnMana(
    AActor *Target,
    const FTransform &SpawnTransform,
    const TMap<TSubclassOf<AMTD_ManaToken>, int32> &ManaTokens,
    float TriggersIgnoreTime)
{
    TArray<AMTD_ManaToken *> SpawnedTokens;

    if (!IsValid(Target))
    {
        MTD_WARN("World context object is invalid.");
        return SpawnedTokens;
    }
    
    if (ManaTokens.IsEmpty())
    {
        MTD_WARN("Mana tokens array is empty.");
        return SpawnedTokens;
    }

    UWorld *World = Target->GetWorld();
    for (const auto &[ManaTokenClass, Amount] : ManaTokens)
    {
        for (int32 i = 0; (i < Amount); i++)
        {
            // Spawn mana token
            AMTD_ManaToken *ManaToken = SpawnManaToken(
                *World, Target, SpawnTransform, ManaTokenClass, TriggersIgnoreTime);

            if (IsValid(ManaToken))
            {
                // Add token to result list
                SpawnedTokens.Add(ManaToken);
            }
        }
    }

    return SpawnedTokens;
}

void AMTD_ManaToken::AddRandomDirectionForce(const float MinSpeed, const float MaxSpeed)
{
    // Randomize direction towards up
    const FVector Direction = FVector(
        FMath::RandRange(-1.f, 1.f),
        FMath::RandRange(-1.f, 1.f),
        FMath::RandRange(0.2f, 0.5f)).GetUnsafeNormal();

    // Randomize speed
    const float Speed = FMath::RandRange(MinSpeed, (MinSpeed + MaxSpeed));

    // Apply force on token
    const FVector Force = (Direction * Speed);
    MovementComponent->AddForce(Force);
}

void AMTD_ManaToken::AddRandomDirectionForceToTokens(const TArray<AMTD_ManaToken *> &ManaTokens, const float MinSpeed,
    const float MaxSpeed)
{
    // Iterate through each mana token, and call AddRandomDirectionForce
    int32 Index = 0;
    for (AMTD_ManaToken *ManaToken : ManaTokens)
    {
        if (!IsValid(ManaToken))
        {
            MTD_WARN("Mana token (%d) is invalid.", Index);
            Index++;
            continue;
        }
        
        ManaToken->AddRandomDirectionForce(MinSpeed, MaxSpeed);
        Index++;
    }
}

void AMTD_ManaToken::OnManaChangeHandleScanCase(UMTD_ManaComponent *ManaComponent)
{
    check(IsValid(ManaComponent));

    const float CurrentMana = ManaComponent->GetMana();
    const float MaxMana = ManaComponent->GetMaxMana();

    // Select the new target only if it has enough mana pool to store any positive amount of mana
    if (MaxMana > CurrentMana)
    {
        AActor *ComponentOwner = ManaComponent->GetOwner();
        check(IsValid(ComponentOwner));
    
        auto Pawn = CastChecked<APawn>(ComponentOwner);

        DisableScan();
        SetNewTarget(Pawn);
    }
}

void AMTD_ManaToken::OnTargetManaAttributeChanged(UMTD_ManaComponent *ManaComponent, float OldValue, float NewValue,
    AActor* InInstigator)
{
    if (NewValue == OldValue)
    {
        return;
    }
    
    // Handle scan case
    if (bScanMode)
    {
        OnManaChangeHandleScanCase(ManaComponent);
        return;
    }

    // Regular case -- There is a specific target to listen to
    const float MaxMana = ManaComponent->GetMaxMana();
    if (MaxMana > NewValue)
    {
        return;
    }

    // If (MaxMana <= NewValue) then the target has full mana pool, hence stop following him and try to find a new one
    RemoveCurrentTargetFromListening();
    APawn *NewTarget = FindNewTarget();
    SetNewTarget(NewTarget);

    // If there is no specific target, listen for everyone's mana
    if (!IsValid(NewTarget))
    {
        EnableScan();
    }
}

void AMTD_ManaToken::OnTargetMaxManaAttributeChanged(UMTD_ManaComponent *ManaComponent, float OldValue,
    float NewValue, AActor *InInstigator)
{
    if (NewValue == OldValue)
    {
        return;
    }
    
    if (bScanMode)
    {
        OnManaChangeHandleScanCase(ManaComponent);
        return;
    }

    const float CurrentMana = ManaComponent->GetMana();
    if (CurrentMana > NewValue)
    {
        // Current mana will notify us about the change because it'll be clamped by new max mana
        return;
    }

    if (CurrentMana == NewValue)
    {
        // Current mana will not be changed since the new max mana is the same, but it also means that it cannot store
        // any new mana, hence stop following this actor, and find a new one
        RemoveCurrentTargetFromListening();
        APawn *NewTarget = FindNewTarget();
        SetNewTarget(NewTarget);

        // If there is no specific target, listen for everyone's mana
        if (!IsValid(NewTarget))
        {
            EnableScan();
        }
    }
}
