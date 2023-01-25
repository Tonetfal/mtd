#include "Character/MTD_BasePlayerCharacter.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_PlayerExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Player/MTD_PlayerState.h"
#include "System/MTD_Tags.h"

AMTD_BasePlayerCharacter::AMTD_BasePlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    UCapsuleComponent *CollisionComponent = GetCapsuleComponent();
    CollisionComponent->SetCollisionProfileName(PlayerCollisionProfileName);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->TargetArmLength = 400.f;
    SpringArmComponent->SocketOffset.Z = 200.f;
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera Component");
    CameraComponent->SetupAttachment(SpringArmComponent, SpringArmComponent->SocketName);
    CameraComponent->bUsePawnControlRotation = false;

    PlayerExtensionComponent = CreateDefaultSubobject<UMTD_PlayerExtensionComponent>("MTD Player Extension Component");

    UCharacterMovementComponent *Cmc = GetCharacterMovement();
    Cmc->bIgnoreBaseRotation = true;
    Cmc->bOrientRotationToMovement = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    Tags.Add(FMTD_Tags::Character);
}

FGameplayTagContainer AMTD_BasePlayerCharacter::GetHeroClasses() const
{
    const auto PlayerData = PlayerExtensionComponent->GetPlayerData<UMTD_PlayerData>();
    return ((IsValid(PlayerData) ? (PlayerData->HeroClasses) : (FGameplayTagContainer::EmptyContainer)));
}

void AMTD_BasePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitializeInput();
    InitializeAttributes();

    const auto PlayerData = PlayerExtensionComponent->GetPlayerData<UMTD_PlayerData>();
    if (!IsValid(PlayerData))
    {
        MTDS_WARN("Player Data is invalid.");
    }
    else
    {
        AMTD_PlayerState *MtdPs = GetMtdPlayerState();
        if (!IsValid(MtdPs))
        {
            MTDS_WARN("MTD Player State is invalid.");
        }
        else
        {
            MtdPs->SetHeroClasses(PlayerData->HeroClasses);
        }
    }
}

void AMTD_BasePlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    if (EndPlayReason == EEndPlayReason::Destroyed)
    {
        AMTD_PlayerState *MtdPs = GetMtdPlayerState();
        if (!IsValid(MtdPs))
        {
            MTDS_WARN("MTD Player State is invalid.");
        }
        else
        {
            MtdPs->ClearHeroClasses();
        }
    }
}

void AMTD_BasePlayerCharacter::InitializeAttributes()
{
    const auto PlayerData = PlayerExtensionComponent->GetPlayerData<UMTD_PlayerData>();
    if (!IsValid(PlayerData))
    {
        MTDS_WARN("Player Data on Player [%s] is invalid.", *GetName());
        return;
    }

    if (!IsValid(PlayerData->AttributeTable))
    {
        MTDS_WARN("Attribute Table on Owner [%s]'s Player Data is invalid.", *GetName());
        return;
    }

    UAbilitySystemComponent *Asc = GetAbilitySystemComponent();
    if (!IsValid(Asc))
    {
        MTDS_WARN("Ability System Component on Player [%s] is invalid.", *GetName());
        return;
    }

    float Value;

    // @todo Get from an actual source
    const float Level = 1.f;
    Asc->ApplyModToAttribute(UMTD_PlayerSet::GetLevelStatAttribute(), EGameplayModOp::Type::Override, Level);

    // @todo Load from save file
    const float Experience = 0.f;
    Asc->ApplyModToAttribute(UMTD_PlayerSet::GetExperienceStatAttribute(), EGameplayModOp::Type::Override, Experience);

    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, HealthAttributeName, Level, Value);
    Asc->ApplyModToAttribute(UMTD_HealthSet::GetMaxHealthAttribute(), EGameplayModOp::Type::Override, Value);
    Asc->ApplyModToAttribute(UMTD_HealthSet::GetHealthAttribute(), EGameplayModOp::Type::Override, Value);
    
    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, ManaAttributeName, Level, Value);
    Asc->ApplyModToAttribute(UMTD_ManaSet::GetMaxManaAttribute(), EGameplayModOp::Type::Override, Value);
    Asc->ApplyModToAttribute(UMTD_ManaSet::GetManaAttribute(), EGameplayModOp::Type::Override, 0.f);
    
    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, BalanceDamageAttributeName, Level, Value);
    Asc->ApplyModToAttribute(UMTD_BalanceSet::GetDamageAttribute(), EGameplayModOp::Type::Override, Value);
    
    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, BalanceResistAttributeName, Level, Value);
    Asc->ApplyModToAttribute(UMTD_BalanceSet::GetResistAttribute(), EGameplayModOp::Type::Override, Value);

    MTDS_VERBOSE("Player [%s]'s attributes have been initialized.", *GetName());
}

void AMTD_BasePlayerCharacter::OnDeathStarted_Implementation(AActor *OwningActor)
{
    Super::OnDeathStarted_Implementation(OwningActor);
    
    DisableControllerInput();
    DisableMovement();
    DisableCollision();
}

void AMTD_BasePlayerCharacter::OnGameTerminated_Implementation(EMTD_GameResult GameResult)
{
    Super::OnGameTerminated_Implementation(GameResult);
}

void AMTD_BasePlayerCharacter::InitializeInput()
{
    auto Pc = GetController<APlayerController>();
    check(Pc);

    auto EiSubsystem = Pc->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

    FModifyContextOptions ContextOptions;
    ContextOptions.bIgnoreAllPressedKeysUntilRelease = false;

    const auto Data = PlayerExtensionComponent->GetPlayerData<UMTD_PlayerData>();
    if (!IsValid(Data))
    {
        MTDS_WARN("Player Data on Player [%s] is invalid.", *GetName());
        return;
    }

    for (TObjectPtr<const UInputMappingContext> Context : Data->InputContexts)
    {
        if (!Context)
        {
            MTDS_WARN("A Mapping Input Context on Player [%s] is invalid.", *GetName());
            continue;
        }
        
        EiSubsystem->AddMappingContext(Context, 0, ContextOptions);
    }
}

void AMTD_BasePlayerCharacter::DisableControllerInput()
{
    if (!IsValid(Controller))
    {
        return;
    }

    auto PlayerController = GetController<APlayerController>();
    check(PlayerController);

    DisableInput(PlayerController);
}

void AMTD_BasePlayerCharacter::DisableMovement()
{
    UCharacterMovementComponent *MoveComp = GetCharacterMovement();
    check(MoveComp);

    MoveComp->StopMovementImmediately();
}

void AMTD_BasePlayerCharacter::DisableCollision()
{
    UCapsuleComponent *CapsuleComp = GetCapsuleComponent();
    check(CapsuleComp);

    CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    CapsuleComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}
