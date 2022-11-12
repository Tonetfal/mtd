#include "System/MTD_Core.h"

#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_HealthComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/PlayerState.h"
#include "Player/MTD_PlayerState.h"

AMTD_Core::AMTD_Core()
{
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
    SetRootComponent(CollisionSphere);
    CollisionSphere->SetCollisionProfileName(AllyCollisionProfileName);
    CollisionSphere->SetCanEverAffectNavigation(false);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh Component");
    Mesh->SetupAttachment(CollisionSphere);
    Mesh->SetCollisionProfileName("NoCollision");
    Mesh->SetCanEverAffectNavigation(false);

    HealthComponent = CreateDefaultSubobject<UMTD_HealthComponent>(TEXT("Health Component"));
}

UMTD_AbilitySystemComponent *AMTD_Core::GetMtdAbilitySystemComponent() const
{
    APlayerState *Ps = GetPlayerState();
    if (!IsValid(Ps))
    {
        return nullptr;
    }

    return CastChecked<AMTD_PlayerState>(Ps)->GetMtdAbilitySystemComponent();
}

UAbilitySystemComponent *AMTD_Core::GetAbilitySystemComponent() const
{
    return GetMtdAbilitySystemComponent();
}

void AMTD_Core::BeginPlay()
{
    Super::BeginPlay();

    check(HealthComponent);

    HealthComponent->OnHealthChangedDelegate.AddDynamic(this, &ThisClass::OnCoreHealthChanged);
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnCoreDestroyed);
}
