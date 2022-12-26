#include "Items/MTD_ManaToken.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "Character/MTD_ManaComponent.h"
#include "Items/MTD_TokenMovementComponent.h"

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
    // Don't call the base implementation, but make a mana-based one
    
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

    if (bScanMode)
    {
        AddToListening(Pawn);
    }
    else if (!MovementComponent->HomingTargetComponent.IsValid())
    {
        EnableScan();
    }
}

void AMTD_ManaToken::OnPawnRemoved(APawn *Pawn)
{
    // Don't call the base implementation
    
    if (bScanMode)
    {
        if (DetectedPawns.IsEmpty())
        {
            DisableScan();
        }
        
        RemoveFromListening(Pawn);
    }
}

void AMTD_ManaToken::SetNewTarget(APawn *Pawn)
{
    if (bIgnoreTriggers)
    {
        return;
    }
    
    if (MovementComponent->HomingTargetComponent.IsValid())
    {
        AActor *OldTarget = MovementComponent->HomingTargetComponent->GetOwner();
        RemoveFromListening(OldTarget);
    }
    
    if (IsValid(Pawn))
    {
        // Bind a delegate to know when the target could fulfill the mana
        AddToListening(Pawn);
    }
    
    Super::SetNewTarget(Pawn);
}

AMTD_ManaToken *SpawnManaToken(UWorld &World, const FTransform &SpawnTransform,
    const TSubclassOf<AMTD_ManaToken> &TokenClass)
{
    if (!TokenClass.Get())
    {
        MTD_WARN("Token class is invalid.");
        return nullptr;
    }
    
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    
    AActor *Actor = World.SpawnActor(TokenClass, &SpawnTransform, Params);
    return Cast<AMTD_ManaToken>(Actor);
}

TArray<AMTD_ManaToken *> AMTD_ManaToken::SpawnMana(
    AActor *Owner,
    const FTransform &SpawnTransform,
    int32 ManaAmount,
    const TMap<int32, TSubclassOf<AMTD_ManaToken>> &ManaTokensTable)
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
        AMTD_ManaToken *ManaToken = SpawnManaToken(*World, SpawnTransform, ManaTokenClass);
        SpawnedTokens.Add(ManaToken);

        // Decrease mana amount respectivelly per each spawned mana token
        ManaAmount -= CurrentTokenAmount;
    }

    return SpawnedTokens;
}

void AMTD_ManaToken::GiveStartVelocity(AMTD_ManaToken *ManaToken, const float BaseSpeed, const float MaxSpeedBonus)
{
    const FVector Direction = FVector(
        FMath::RandRange(-1.f, 1.f),
        FMath::RandRange(-1.f, 1.f),
        FMath::RandRange(0.5f, 1.f)).GetUnsafeNormal();
    
    const float BonusRatio = FMath::FRand();
    const float SpeedBonus = BonusRatio * MaxSpeedBonus;
    const float Speed = BaseSpeed + SpeedBonus;
    
    const FVector Force = Direction * Speed;
    ManaToken->MovementComponent->AddForce(Force);
}

void AMTD_ManaToken::GiveStartVelocityToTokens(const TArray<AMTD_ManaToken *> &ManaTokens, const float BaseSpeed,
    const float MaxSpeedBonus)
{
    for (AMTD_ManaToken *ManaToken : ManaTokens)
    {
        GiveStartVelocity(ManaToken, BaseSpeed, MaxSpeedBonus);
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
        check(IsValid(ManaComponent));
        
        AActor *ComponentOwner = ManaComponent->GetOwner();
        check(IsValid(ComponentOwner));
        
        auto Pawn = CastChecked<APawn>(ComponentOwner);

        DisableScan();
        SetNewTarget(Pawn);
        
        return;
    }

    // Regular case -- There is a specific target to listen to
    const float MaxMana = ManaComponent->GetMaxMana();
    if (MaxMana > NewValue)
    {
        return;
    }

    // If (MaxMana <= NewValue) then the target has full mana pool, hence stop following him and try to find a new one
    APawn *NewTarget = FindNewTarget();
    SetNewTarget(NewTarget);

    // If there is no specific target, listen for everyone's mana
    if ((!DetectedPawns.IsEmpty()) && (!IsValid(NewTarget)))
    {
        EnableScan();
    }
}

void AMTD_ManaToken::EnableScan()
{
    ensure(!DetectedPawns.IsEmpty());
    ensure(!bScanMode);

    for (APawn *Pawn : DetectedPawns)
    {
        AddToListening(Pawn);
    }

    bScanMode = true;
}

void AMTD_ManaToken::DisableScan()
{
    ensure(bScanMode);
    
    for (APawn *Pawn : DetectedPawns)
    {
        RemoveFromListening(Pawn);
    }

    bScanMode = false;
}

void AMTD_ManaToken::AddToListening(AActor *Actor)
{
    if (IsValid(Actor))
    {
        UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Actor);
        if (IsValid(ManaComponent))
        {
            ManaComponent->OnManaChangedDelegate.AddDynamic(this, &ThisClass::OnTargetManaAttributeChanged);
            ManaComponent->OnMaxManaChangedDelegate.AddDynamic(this, &ThisClass::OnTargetManaAttributeChanged);
        }
    }
}

void AMTD_ManaToken::RemoveFromListening(AActor *Actor)
{
    if (IsValid(Actor))
    {
        UMTD_ManaComponent *ManaComponent = UMTD_ManaComponent::FindManaComponent(Actor);
        if (IsValid(ManaComponent))
        {
            ManaComponent->OnManaChangedDelegate.RemoveDynamic(this, &ThisClass::OnTargetManaAttributeChanged);
            ManaComponent->OnMaxManaChangedDelegate.RemoveDynamic(this, &ThisClass::OnTargetManaAttributeChanged);
        }
    }
}
