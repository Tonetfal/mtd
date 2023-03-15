#include "Character/MTD_BasePlayerCharacter.h"

#include "AbilitySystem/Attributes/MTD_BalanceSet.h"
#include "AbilitySystem/Attributes/MTD_HealthSet.h"
#include "AbilitySystem/Attributes/MTD_ManaSet.h"
#include "AbilitySystem/Attributes/MTD_PlayerSet.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/Components/MTD_LevelComponent.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "CombatSystem/MTD_CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Player/MTD_PlayerState.h"
#include "System/MTD_Tags.h"

AMTD_BasePlayerCharacter::AMTD_BasePlayerCharacter()
{
    // Nothing to tick for
    PrimaryActorTick.bCanEverTick = false;
    PrimaryActorTick.bStartWithTickEnabled = false;

    UCapsuleComponent *CollisionComponent = GetCapsuleComponent();
    CollisionComponent->SetCollisionProfileName(PlayerCollisionProfileName);

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Component");
    SpringArmComponent->SetupAttachment(GetRootComponent());
    SpringArmComponent->TargetArmLength = 400.f;
    SpringArmComponent->SocketOffset.Z = 200.f;
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera Component");
    CameraComponent->SetupAttachment(SpringArmComponent, SpringArmComponent->SocketName);

    // Rotate character towards camera rotation
    UCharacterMovementComponent *CharacterMovementComponent = GetCharacterMovement();
    CharacterMovementComponent->bIgnoreBaseRotation = true;
    CharacterMovementComponent->bOrientRotationToMovement = false;

    CameraComponent->bUsePawnControlRotation = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;
    
    CombatComponent->AddObjectTypeToHit(FoeQuery);

    Tags.Add(FMTD_Tags::Player);
}

FGameplayTagContainer AMTD_BasePlayerCharacter::GetHeroClasses() const
{
    return ((IsValid(PlayerData) ? (PlayerData->HeroClasses) : (FGameplayTagContainer::EmptyContainer)));
}

void AMTD_BasePlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    ListenForLevelUp();
    InitializeInput();
    InitializeAttributes();
}

void AMTD_BasePlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    
    if (EndPlayReason == EEndPlayReason::Destroyed)
    {
        AMTD_PlayerState *MtdPlayerState = GetMtdPlayerState();
        if (!IsValid(MtdPlayerState))
        {
            MTDS_WARN("MTD player state is invalid.");
        }
        else
        {
            MtdPlayerState->ClearHeroClasses();
        }
    }
}

void AMTD_BasePlayerCharacter::InitializeAttributes()
{
    if (!IsValid(PlayerData))
    {
        MTDS_WARN("Player data is invalid.");
        return;
    }

    if (!IsValid(PlayerData->AttributeTable))
    {
        MTDS_WARN("Attribute table is invalid.");
        return;
    }

    UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability system component is invalid.");
        return;
    }

    float Value;

    // @todo Load from save file
    
    const float Level = 1.f;
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_PlayerSet::GetLevelAttribute(), Level);

    const float Experience = 0.f;
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_PlayerSet::GetLevelExperienceAttribute(), Experience);

    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, HealthAttributeName, Level, Value);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_HealthSet::GetMaxHealthAttribute(), Value);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_HealthSet::GetHealthAttribute(), Value);
    
    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, ManaAttributeName, Level, Value);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_ManaSet::GetMaxManaAttribute(), Value);
    AbilitySystemComponent->ApplyModToAttribute(UMTD_ManaSet::GetManaAttribute(), EGameplayModOp::Type::Override, 0.f);
    
    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, BalanceDamageAttributeName, Level, Value);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_BalanceSet::GetDamageAttribute(), Value);
    
    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, BalanceResistAttributeName, Level, Value);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_BalanceSet::GetResistAttribute(), Value);

    MTDS_VERBOSE("Attributes have been initialized.");
}

void AMTD_BasePlayerCharacter::OnDeathStarted_Implementation(AActor *OwningActor)
{
    Super::OnDeathStarted_Implementation(OwningActor);
    
    DisableControllerInput();
    DisableMovement();
    DisableCollision();
}

void AMTD_BasePlayerCharacter::OnLevelUp(UMTD_LevelComponent *LevelComponent, float OldValue, float NewValue,
    AActor *InInstigator)
{
    if (!IsValid(PlayerData))
    {
        MTDS_WARN("Player data is invalid.");
        return;
    }

    if (!IsValid(PlayerData->AttributeTable))
    {
        MTDS_WARN("Attribute table is invalid.");
        return;
    }

    UAbilitySystemComponent *AbilitySystemComponent = GetAbilitySystemComponent();
    if (!IsValid(AbilitySystemComponent))
    {
        MTDS_WARN("Ability system component is invalid.");
        return;
    }

    float Value;
    
    // Set new max health value based on new level
    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, HealthAttributeName, NewValue, Value);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_HealthSet::GetMaxHealthAttribute(), Value);
    
    // Set new max mana value based on new level
    EVALUTE_ATTRIBUTE(PlayerData->AttributeTable, ManaAttributeName, NewValue, Value);
    AbilitySystemComponent->SetNumericAttributeBase(UMTD_ManaSet::GetMaxManaAttribute(), Value);
}

void AMTD_BasePlayerCharacter::OnGameTerminated_Implementation(EMTD_GameResult GameResult)
{
    Super::OnGameTerminated_Implementation(GameResult);
}

void AMTD_BasePlayerCharacter::ListenForLevelUp()
{
    if (!IsValid(PlayerData))
    {
        MTDS_WARN("Player data is invalid.");
        return;
    }
    
    AMTD_PlayerState *MtdPlayerState = GetMtdPlayerState();
    if (!IsValid(MtdPlayerState))
    {
        MTDS_WARN("MTD player state is invalid.");
        return;
    }
    
    // Set our hero classes on player state, so that is accessible even if this character is dead
    MtdPlayerState->SetHeroClasses(PlayerData->HeroClasses);
        
    UMTD_LevelComponent *LevelComponent = MtdPlayerState->GetLevelComponent();
    if (!IsValid(LevelComponent))
    {
        MTDS_WARN("Level component on player state [%s] is invalid.", *MtdPlayerState->GetName());
        return;
    }
    
    // Listen for level up event
    LevelComponent->OnPrimaryLevelAttributeChangedDelegate.BindUObject(this, &ThisClass::OnLevelUp);
}

void AMTD_BasePlayerCharacter::InitializeInput()
{
    auto PlayerController = GetController<APlayerController>();
    check(IsValid(PlayerController));

    auto EiSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    check(IsValid(EiSubsystem));

    if (!IsValid(PlayerData))
    {
        MTDS_WARN("Player data is invalid.");
        return;
    }

    FModifyContextOptions ContextOptions;
    ContextOptions.bIgnoreAllPressedKeysUntilRelease = false;

    // Add all mapping contexts
    int32 Index = 0;
    for (TObjectPtr<const UInputMappingContext> Context : PlayerData->InputContexts)
    {
        if (!Context)
        {
            MTDS_WARN("Mapping input context (%d) is invalid.", Index);
            Index++;
            continue;
        }
        
        EiSubsystem->AddMappingContext(Context, 0, ContextOptions);
        Index++;
    }
}

void AMTD_BasePlayerCharacter::DisableControllerInput()
{
    if (!IsValid(Controller))
    {
        return;
    }

    auto PlayerController = GetController<APlayerController>();
    check(IsValid(PlayerController));

    DisableInput(PlayerController);
}

void AMTD_BasePlayerCharacter::DisableMovement()
{
    UCharacterMovementComponent *CharacterMovementComponent = GetCharacterMovement();
    check(IsValid(CharacterMovementComponent));

    CharacterMovementComponent->DisableMovement();
}

void AMTD_BasePlayerCharacter::DisableCollision()
{
    UCapsuleComponent *CapsuleComp = GetCapsuleComponent();
    check(IsValid(CapsuleComp));

    CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    CapsuleComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
}
