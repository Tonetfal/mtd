#include "Items/MTD_ManaToken.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "Character/MTD_ManaComponent.h"
#include "Items/MTD_TokenMovementComponent.h"
#include "Kismet/GameplayStatics.h"

bool AMTD_ManaToken::CanBeActivatedOn(APawn *Pawn) const
{
    if (!IsValid(Pawn))
    {
        return false;
    }

    const UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Pawn);
    return (!ManaComponent->IsManaFull());
}

void AMTD_ManaToken::OnActivate_Implementation(APawn *Pawn)
{
    check(IsValid(Pawn));
    
    // Note: The cue and other visuals should be implemented in BPs

    // Grant mana
    UAbilitySystemComponent *AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn);
    AbilitySystemComponent->ApplyModToAttribute(UMTD_ManaSet::GetManaAttribute(), EGameplayModOp::Additive, ManaAmount);
    
    Super::OnActivate_Implementation(Pawn);
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
    // Notify about the error as soon as possible
    ensureMsgf(IsValid(UMTD_ManaComponent::FindManaComponent(Pawn)),
        TEXT("Pawn [%s] does not have a UMTD_ManaComponent."), *Pawn->GetName());
    ensureMsgf(Cast<IAbilitySystemInterface>(Pawn), TEXT("Pawn [%s] does not implement IAbilitySystemInterface."),
        *Pawn->GetName());
    
    // Make the pawn be homing target if there was none, and it's OK
    Super::OnPawnAdded(Pawn);
}

void AMTD_ManaToken::SetNewTarget(APawn *Pawn)
{
    if (bIgnoreTriggers)
    {
        return;
    }

    RemoveCurrentTargetFromListening();
    
    if (IsValid(Pawn))
    {
        // Bind a delegate to know when the target could fulfill the mana
        AddToListening(Pawn);
    }
    
    Super::SetNewTarget(Pawn);
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
    Mana->IgnoreTriggersFor(TriggersIgnoreTime);
    UGameplayStatics::FinishSpawningActor(Mana, SpawnTransform);
    
    return Mana;
}

TArray<AMTD_ManaToken *> AMTD_ManaToken::SpawnMana(
    AActor *Owner,
    const FTransform &SpawnTransform,
    int32 ManaAmount,
    const TMap<int32, TSubclassOf<AMTD_ManaToken>> &ManaTokensTable,
    float TriggersIgnoreTime)
{
    // Note: ManaTokensTable must be sorted from the smallest to the greatest in order to make this function work
    // correctly
    
    TArray<AMTD_ManaToken *> SpawnedTokens;

    if (!IsValid(Owner))
    {
        MTD_WARN("Owner is invalid.");
        return SpawnedTokens;
    }
    
    if (ManaAmount <= 0)
    {
        MTD_WARN("Mana amount (%d) is less than or equal to zero.", ManaAmount);
        return SpawnedTokens;
    }

    if (ManaTokensTable.IsEmpty())
    {
        MTD_WARN("Possible mana tokens array is empty.");
        return SpawnedTokens;
    }

    TArray<int32> Tokens;
    ManaTokensTable.GetKeys(Tokens);

    UWorld *World = Owner->GetWorld();

    int32 CurrentTokenAmount = -1;
    while (ManaAmount > 0)
    {
        // Handle first iteration and situations where CurrentTokenAmount is greater remained ManaAmount
        if (CurrentTokenAmount == -1)
        {
            CurrentTokenAmount = Tokens.Last();
            continue;
        }

        // Each iteration drains some mana, thus token that has been used previously may not suit into decreased mana amount
        if (CurrentTokenAmount > ManaAmount)
        {
            Tokens.Pop();
            CurrentTokenAmount = -1;

#ifdef WITH_EDITOR
            if (Tokens.IsEmpty())
            {
                MTD_WARN("There is not enough mana tokens to cover the required needs.");
                return SpawnedTokens;
            }
#endif
            continue;
        }

        const TSubclassOf<AMTD_ManaToken> &ManaTokenClass = ManaTokensTable.FindChecked(CurrentTokenAmount);
        AMTD_ManaToken *ManaToken = SpawnManaToken(*World, Owner, SpawnTransform, ManaTokenClass, TriggersIgnoreTime);
        
        if (!IsValid(ManaToken))
        {
            MTD_WARN("Failed to spawn a Mana Token.");
        }
        else
        {
            SpawnedTokens.Add(ManaToken);
        }

        // Decrease mana amount respectivelly per each spawned mana token
        ManaAmount -= CurrentTokenAmount;
    }

    return SpawnedTokens;
}

void AMTD_ManaToken::GiveStartVelocity(AMTD_ManaToken *ManaToken, const float BaseSpeed, const float MaxSpeedBonus)
{
    // The method presumes that the velocity is 0, and the starting force is going to be just ours, hence nullify
    // anything applied previously
    ManaToken->MovementComponent->ClearPendingForce();
    
    const FVector Direction = FVector(
        FMath::RandRange(-1.f, 1.f),
        FMath::RandRange(-1.f, 1.f),
        FMath::RandRange(0.2f, 0.5f)).GetUnsafeNormal();

    const float BonusRatio = FMath::FRand();
    const float SpeedBonus = BonusRatio * MaxSpeedBonus;
    const float Speed = BaseSpeed + SpeedBonus;

    const FVector Force = Direction * Speed;
    ManaToken->MovementComponent->AddForce(Force);
}

void AMTD_ManaToken::GiveStartVelocityToTokens(const TArray<AMTD_ManaToken *> &ManaTokens, const float BaseSpeed,
    const float MaxSpeedBonus)
{
    int32 Index = 0;
    for (AMTD_ManaToken *ManaToken : ManaTokens)
    {
        if (!IsValid(ManaToken))
        {
            MTD_WARN("Mana token (%d) is invalid.", Index);
            Index++;
            continue;
        }
        
        GiveStartVelocity(ManaToken, BaseSpeed, MaxSpeedBonus);
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

void AMTD_ManaToken::EnableScan()
{
    ensure(!DetectedPawns.IsEmpty());

    for (APawn *Pawn : DetectedPawns)
    {
        AddToListening(Pawn);
    }

    Super::EnableScan();
}

void AMTD_ManaToken::DisableScan()
{
    for (APawn *Pawn : DetectedPawns)
    {
        RemoveFromListening(Pawn);
    }

    Super::DisableScan();
}

void AMTD_ManaToken::AddToListening(AActor *Actor)
{
    if (IsValid(Actor))
    {
        UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Actor);
        check(IsValid(ManaComponent));
        
        ManaComponent->OnManaChangedDelegate.AddDynamic(this, &ThisClass::OnTargetManaAttributeChanged);
        ManaComponent->OnMaxManaChangedDelegate.AddDynamic(this, &ThisClass::OnTargetMaxManaAttributeChanged);
    }
}

void AMTD_ManaToken::RemoveFromListening(AActor *Actor)
{
    if (IsValid(Actor))
    {
        UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Actor);
        check(IsValid(ManaComponent));
        
        ManaComponent->OnManaChangedDelegate.RemoveDynamic(this, &ThisClass::OnTargetManaAttributeChanged);
        ManaComponent->OnMaxManaChangedDelegate.RemoveDynamic(this, &ThisClass::OnTargetMaxManaAttributeChanged);
    }
}
