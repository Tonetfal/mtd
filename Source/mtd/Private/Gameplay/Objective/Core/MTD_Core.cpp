#include "Gameplay/Objective/Core/MTD_Core.h"

#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/Components/MTD_HealthComponent.h"
#include "Components/SphereComponent.h"
#include "GameModes/MTD_GameModeBase.h"
#include "Player/MTD_PlayerState.h"
#include "System/MTD_Tags.h"

AMTD_Core::AMTD_Core()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>("Collision Sphere");
    SetRootComponent(CollisionComponent);
    CollisionComponent->SetCollisionProfileName(TowerCollisionProfileName);
    CollisionComponent->SetCanEverAffectNavigation(false);

    HealthComponent = CreateDefaultSubobject<UMTD_HealthComponent>("Health Component");
    MtdAbilitySystemComponent = CreateDefaultSubobject<UMTD_AbilitySystemComponent>("MTD Ability System Component");
    UMTD_HealthSet *HealthSet = CreateDefaultSubobject<UMTD_HealthSet>("Health Set");
    MtdAbilitySystemComponent->AddSpawnedAttribute(HealthSet);
    
    Tags.Add(FMTD_Tags::Core);
}

UMTD_AbilitySystemComponent *AMTD_Core::GetMtdAbilitySystemComponent() const
{
    return MtdAbilitySystemComponent;
}

UAbilitySystemComponent *AMTD_Core::GetAbilitySystemComponent() const
{
    return MtdAbilitySystemComponent;
}

void AMTD_Core::OnCoreHealthChanged_Implementation(UMTD_HealthComponent *InHealthComponent, float OldValue,
    float NewValue, AActor *InInstigator)
{
    if (OldValue > NewValue)
    {
        OnCoreHealthChangedDelegate.Broadcast(OldValue, NewValue);
    }
}

void AMTD_Core::OnCoreDestroyed_Implementation(AActor *CoreActor)
{
    auto Core = CastChecked<AMTD_Core>(CoreActor);
    OnCoreDestroyedDelegate.Broadcast(Core);
}

void AMTD_Core::BeginPlay()
{
    Super::BeginPlay();

    check(IsValid(HealthComponent));

    // Listen for health change and death events
    HealthComponent->OnHealthChangedDelegate.AddDynamic(this, &ThisClass::OnCoreHealthChanged);
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnCoreDestroyed);
    
    UWorld *World = GetWorld();
    AGameModeBase *GameMode = World->GetAuthGameMode();
    if (IsValid(GameMode))
    {
        auto MtdGameMode = CastChecked<AMTD_GameModeBase>(GameMode);

        // Listen for game termination events
        MtdGameMode->OnGameTerminatedDelegate.AddDynamic(this, &ThisClass::OnGameTerminated);
    }
}
