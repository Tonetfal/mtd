#include "Character/MTD_BaseCharacter.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_BalanceComponent.h"
#include "Character/MTD_HealthComponent.h"
#include "Character/MTD_HeroComponent.h"
#include "Character/MTD_ManaComponent.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Equipment/MTD_EquipmentManagerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameModes/MTD_GameModeBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/MTD_PlayerState.h"

AMTD_BaseCharacter::AMTD_BaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    GetMesh()->SetCollisionProfileName("NoCollision");

    PawnExtentionComponent = CreateDefaultSubobject<UMTD_PawnExtensionComponent>(TEXT("MTD Pawn Extension Component"));
    HeroComponent = CreateDefaultSubobject<UMTD_HeroComponent>(TEXT("MTD Hero Component"));
    EquipmentManagerComponent = CreateDefaultSubobject<UMTD_EquipmentManagerComponent>(TEXT("MTD Equipment Component"));
    HealthComponent = CreateDefaultSubobject<UMTD_HealthComponent>(TEXT("MTD Health Component"));
    ManaComponent = CreateDefaultSubobject<UMTD_ManaComponent>(TEXT("MTD Mana Component"));
    BalanceComponent = CreateDefaultSubobject<UMTD_BalanceComponent>(TEXT("MTD Balance Component"));
    
    PawnExtentionComponent->OnAbilitySystemInitialized_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));

    PawnExtentionComponent->OnAbilitySystemUninitialized_Register(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
    HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);
}

void AMTD_BaseCharacter::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AMTD_BaseCharacter::PostInitProperties()
{
    Super::PostInitProperties();
}

void AMTD_BaseCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    UWorld *World = GetWorld();
    AGameModeBase *Gm = World->GetAuthGameMode();
    if (IsValid(Gm))
    {
        auto MtdGm = CastChecked<AMTD_GameModeBase>(Gm);
        MtdGm->OnGameTerminatedDelegate.AddDynamic(this, &ThisClass::OnGameTerminated);
    }

    ConstructHitboxMap();
}

void AMTD_BaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AMTD_BaseCharacter::Reset()
{
    Super::Reset();
}

void AMTD_BaseCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bIsMeleeInProgress)
    {
        PerformHitboxTrace();
    }
}

void AMTD_BaseCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();

    PawnExtentionComponent->HandleControllerChanged();
}

void AMTD_BaseCharacter::AddMeleeHitboxes(const TArray<FName> &HitboxNicknames)
{
    for (const FName &Name : HitboxNicknames)
    {
        const auto Found = HitboxMap.Find(Name);
        if (!Found)
        {
            continue;
        }
        
        ActiveHitboxes.Add(Found->HitboxInfo);
    }

    bIsMeleeInProgress = !ActiveHitboxes.IsEmpty();
}

void AMTD_BaseCharacter::RemoveMeleeHitboxes(const TArray<FName> &HitboxNicknames)
{
    for (const FName &Name : HitboxNicknames)
    {
        const auto Found = HitboxMap.Find(Name);
        if (!Found)
        {
            continue;
        }

        ActiveHitboxes.Remove(Found->HitboxInfo);
    }

    bIsMeleeInProgress = !ActiveHitboxes.IsEmpty();
    if (!bIsMeleeInProgress)
    {
        ResetMeleeHitTargets();
    }
}

void AMTD_BaseCharacter::DisableMeleeHitboxes()
{
    ActiveHitboxes.Empty();
    ResetMeleeHitTargets();
    bIsMeleeInProgress = false;
}

void AMTD_BaseCharacter::ResetMeleeHitTargets()
{
    MeleeHitTargets.Empty();
}

float AMTD_BaseCharacter::GetMovementDirectionAngle() const
{
    const FVector Velocity = GetVelocity();
    if (Velocity.IsZero())
    {
        return 0.f;
    }

    const FVector Forward = GetActorForwardVector();
    const FVector Direction = Velocity.GetUnsafeNormal();
    
    const float Dot = Forward | Direction;
    const FVector Cross = Forward ^ Direction;
    
    const float Degrees = FMath::RadiansToDegrees(FMath::Acos(Dot));
    return (Cross.IsZero()) ? (Degrees) : (Degrees * FMath::Sign(Cross.Z));
}

void AMTD_BaseCharacter::PerformHitboxTrace()
{
    ensure(bIsMeleeInProgress);
    
    const UWorld *World = GetWorld();
    const FVector ActorLocation = GetActorLocation();
    const FVector Forward = GetActorForwardVector();
    const FRotator ForwardRot = Forward.Rotation();

    const EDrawDebugTrace::Type DrawDebugTrace =
        (bDebugMelee) ? (EDrawDebugTrace::ForDuration) : (EDrawDebugTrace::None);

    for (const FMTD_MeleeHitSphereDefinition &HitboxEntry : ActiveHitboxes)
    {
        const FVector Offset = ForwardRot.RotateVector(HitboxEntry.Offset);
        const FVector TraceLocation = ActorLocation + Offset;

        TArray<FHitResult> OutHits;
        UKismetSystemLibrary::SphereTraceMultiForObjects(
            World, TraceLocation, TraceLocation, HitboxEntry.Radius, ObjectTypesToHit, false, MeleeHitTargets,
            DrawDebugTrace, OutHits, false, FLinearColor::Red, FLinearColor::Green, DrawTime);

        for (const FHitResult &Hit : OutHits)
        {
            PerformHit(Hit);
        }
    }
}

void AMTD_BaseCharacter::PerformHit(const FHitResult &Hit)
{
    UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
    
    AActor *HitActor = Hit.GetActor();
    MeleeHitTargets.Add(HitActor);

    FGameplayEventData EventData;
    EventData.ContextHandle = Asc->MakeEffectContext();
    EventData.Instigator = GetPlayerState();
    EventData.Target = HitActor;
    EventData.TargetData.Data.Add(
        TSharedPtr<FGameplayAbilityTargetData>(new FGameplayAbilityTargetData_SingleTargetHit(Hit)));

    const FMTD_GameplayTags &GameplayTags = FMTD_GameplayTags::Get();
    Asc->HandleGameplayEvent(GameplayTags.Gameplay_Event_MeleeHit, &EventData);
}

void AMTD_BaseCharacter::InitializeAttributes()
{
    // Empty
}

void AMTD_BaseCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PawnExtentionComponent->SetupPlayerInputComponent();
}

void AMTD_BaseCharacter::ConstructHitboxMap()
{
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

        for (const FMTD_MeleeHitSphereDefinition &HitDefinition : Data->MeleeHitSpheres)
        {
            FMTD_ActiveHitboxEntry HitboxEntry;
            HitboxEntry.HitboxInfo = HitDefinition;
            
            HitboxMap.Add(HitDefinition.Nickname, HitboxEntry);
        }
        Index++;
    }
}

void AMTD_BaseCharacter::FellOutOfWorld(const UDamageType &DamageType)
{
    HealthComponent->SelfDestruct(true);
}

void AMTD_BaseCharacter::OnAbilitySystemInitialized()
{
    UMTD_AbilitySystemComponent *MtdAsc = GetMtdAbilitySystemComponent();
    check(MtdAsc);

    HealthComponent->InitializeWithAbilitySystem(MtdAsc);
    ManaComponent->InitializeWithAbilitySystem(MtdAsc);
    BalanceComponent->InitializeWithAbilitySystem(MtdAsc);
}

void AMTD_BaseCharacter::OnAbilitySystemUninitialized()
{
    HealthComponent->UninitializeFromAbilitySystem();
    ManaComponent->UninitializeFromAbilitySystem();
    BalanceComponent->UninitializeFromAbilitySystem();
}

void AMTD_BaseCharacter::DestroyDueToDeath()
{
    DetachFromControllerPendingDestroy();
    SetLifeSpan(0.1f);

    Uninit();
}

void AMTD_BaseCharacter::Uninit()
{
    // Uninit if we are still the avatar; we could be kicked from ASC if someone else owned it instead
    const UMTD_AbilitySystemComponent *Asc = GetMtdAbilitySystemComponent();
    if (IsValid(Asc))
    {
        if (Asc->GetAvatarActor() == this)
        {
            PawnExtentionComponent->UninitializeAbilitySystem();
        }
    }

    SetActorHiddenInGame(true);
}

void AMTD_BaseCharacter::OnDeathStarted_Implementation(AActor *OwningActor)
{
    // Empty
}

void AMTD_BaseCharacter::OnDeathFinished_Implementation(AActor *OwningActor)
{
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
    EquipmentManagerComponent->UnequipItem();
}

AMTD_PlayerState *AMTD_BaseCharacter::GetMtdPlayerState() const
{
    return CastChecked<AMTD_PlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UMTD_AbilitySystemComponent *AMTD_BaseCharacter::GetMtdAbilitySystemComponent() const
{
    return PawnExtentionComponent->GetMtdAbilitySystemComponent();
}

UAbilitySystemComponent *AMTD_BaseCharacter::GetAbilitySystemComponent() const
{
    return GetMtdAbilitySystemComponent();
}
