#include "Character/MTD_CombatComponent.h"

#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetSystemLibrary.h"

UMTD_CombatComponent::UMTD_CombatComponent()
{
    // Component has to tick in order to sweep for targets
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    // Make InitializeComponent be called
    bWantsInitializeComponent = true;
}

void UMTD_CombatComponent::InitializeComponent()
{
    Super::InitializeComponent();

    // Construct hitbox map before it's requested to use
    ConstructHitboxMap();
}

void UMTD_CombatComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache owner as MTD Base Character
    PrivateCharacterOwner = Cast<AMTD_BaseCharacter>(GetOwner());
    ensure(IsValid(PrivateCharacterOwner));
}

void UMTD_CombatComponent::AddObjectTypeToHit(EObjectTypeQuery InObjectType)
{
    ObjectTypesToHit.Add(InObjectType);
}

void UMTD_CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Perform hitbox trace check whenever melee is in progress
    if (bIsMeleeInProgress)
    {
        PerformHitboxCheck();
    }
}

void UMTD_CombatComponent::AddMeleeHitboxes(const TArray<FName> &HitboxNicknames)
{
    // Iterate through the query nicknames and check if hitbox map contains any
    for (const FName &Name : HitboxNicknames)
    {
        const auto Found = HitboxMap.Find(Name);
        if (!Found)
        {
            continue;
        }

        // The requested hitbox has been found in the list: Add hitbox into active list
        ActiveHitboxes.Add(Found->HitboxInfo);
    }

    // Melee attack isn't in progress if there is no active hitbox
    bIsMeleeInProgress = (!ActiveHitboxes.IsEmpty());
}

void UMTD_CombatComponent::RemoveMeleeHitboxes(const TArray<FName> &HitboxNicknames)
{
    // Iterate through the query nicknames and check if hitbox map contains any
    for (const FName &Name : HitboxNicknames)
    {
        const auto Found = HitboxMap.Find(Name);
        if (!Found)
        {
            continue;
        }

        // The requested hitbox has been found in the list: Remove hitbox from active list
        ActiveHitboxes.Remove(Found->HitboxInfo);
    }

    // Melee attack isn't in progress if there is no active hitbox
    bIsMeleeInProgress = (!ActiveHitboxes.IsEmpty());
    if (!bIsMeleeInProgress)
    {
        // Discard hit target if melee isn't in progress anymore
        ResetMeleeHitTargets();
    }
}

void UMTD_CombatComponent::DisableMeleeHitboxes()
{
    // Reset everything that keeps track of current attack
    ActiveHitboxes.Empty();
    ResetMeleeHitTargets();
    bIsMeleeInProgress = false;
}

void UMTD_CombatComponent::ResetMeleeHitTargets()
{
    // Discard all hit target gather by the most recent attack
    MeleeHitTargets.Empty();
}

void UMTD_CombatComponent::PerformHitboxCheck()
{
    ensure(bIsMeleeInProgress);

    if (!IsValid(PrivateCharacterOwner))
    {
        MTDS_WARN("Character Owner is invalid.");
        return;
    }

    // Cache data required to use perform the sweeps
    const UWorld *World = GetWorld();
    const FVector ActorLocation = PrivateCharacterOwner->GetActorLocation();
    const FVector Forward = PrivateCharacterOwner->GetActorForwardVector();
    const FRotator ForwardRot = Forward.Rotation();

    const EDrawDebugTrace::Type DrawDebugTrace =
        ((bDebugMelee) ? (EDrawDebugTrace::ForDuration) : (EDrawDebugTrace::None));

    // Iterate through each active hitbox and check for collision
    for (const FMTD_MeleeHitSphereDefinition &HitboxEntry : ActiveHitboxes)
    {
        const FVector Offset = ForwardRot.RotateVector(HitboxEntry.Offset);
        const FVector TraceLocation = (ActorLocation + Offset);

        // Sweep for enemies. Note that MeleeHitTargets are ignored
        TArray<FHitResult> OutHits;
        UKismetSystemLibrary::SphereTraceMultiForObjects(
            World, TraceLocation, TraceLocation, HitboxEntry.Radius, ObjectTypesToHit, false, MeleeHitTargets,
            DrawDebugTrace, OutHits, false, FLinearColor::Red, FLinearColor::Green, DrawTime);

        // Iterate through each hit target and true to perform a hit
        for (const FHitResult &Hit : OutHits)
        {
            PerformHit(Hit);
        }
    }
}

void UMTD_CombatComponent::PerformHit(const FHitResult &Hit)
{
    if (!IsValid(PrivateCharacterOwner))
    {
        MTDS_WARN("Character Owner is invalid.");
        return;
    }
    
    UAbilitySystemComponent *Asc = PrivateCharacterOwner->GetAbilitySystemComponent();
    if (!IsValid(Asc))
    {
        MTDS_WARN("Ability System Component is invalid.");
        return;
    }

    // Get the hit actor and store it inside MeleeHitTargets to avoid hitting it twice with the same attack
    AActor *HitActor = Hit.GetActor();
    MeleeHitTargets.Add(HitActor);

    // Prepare a gameplay event to send to the target
    FGameplayEventData EventData;
    EventData.ContextHandle = Asc->MakeEffectContext();
    EventData.Instigator = PrivateCharacterOwner->GetPlayerState();
    EventData.Target = HitActor;
    EventData.TargetData.Data.Add(
        TSharedPtr<FGameplayAbilityTargetData>(new FGameplayAbilityTargetData_SingleTargetHit(Hit)));

    // Notify the target about the hit
    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    Asc->HandleGameplayEvent(GameplayTags.Gameplay_Event_MeleeHit, &EventData);
}

void UMTD_CombatComponent::ConstructHitboxMap()
{
    // Start with empty map
    HitboxMap.Empty();

    if (HitboxData.IsEmpty())
    {
        MTD_WARN("Hitbox Data is empty.");
        return;
    }

    int32 Index = 0;
    for (const UMTD_MeleeHitboxData *Data : HitboxData)
    {
        if (!IsValid(Data))
        {
            MTD_WARN("Melee Hitbox Data (%d) is invalid.", Index);
            Index++;
            continue;
        }
        
        if (Data->MeleeHitSpheres.IsEmpty())
        {
            MTD_WARN("Melee Hitbox Data [%s]'s Melee Hit Sphere array is empty.", *Data->GetName());
            Index++;
            continue;
        }

        // Copy each entry to the hitbox map. Note that each hitbox should have a unique nickname
        for (const FMTD_MeleeHitSphereDefinition &HitDefinition : Data->MeleeHitSpheres)
        {
            FMTD_ActiveHitboxEntry HitboxEntry;
            HitboxEntry.HitboxInfo = HitDefinition;

            if (HitboxMap.Contains(HitDefinition.Nickname))
            {
                MTDS_WARN("Hitbox with Nickname [%s] is already present. Skipping...",
                    *HitDefinition.Nickname.ToString());
                continue;
            }

            // Bind nickname to entry
            HitboxMap.Add(HitDefinition.Nickname, HitboxEntry);
        }
        
        Index++;
    }
}
