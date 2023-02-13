#include "Character/MTD_HeroComponent.h"

#include "AbilitySystem/MTD_AbilitySet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_BasePlayerCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "GameFramework/Character.h"
#include "Input/MTD_InputComponent.h"
#include "Player/MTD_PlayerController.h"
#include "Player/MTD_PlayerState.h"

UMTD_HeroComponent::UMTD_HeroComponent()
{
    // Nothing to tick for
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_HeroComponent::OnRegister()
{
    Super::OnRegister();

    const auto Owner = GetPawn<APawn>();
    if (!IsValid(Owner))
    {
        MTDS_ERROR("Owner [%s] must be a pawn actor.", *GetNameSafe(GetOwner()));
        return;
    }

    auto PawnExtComp = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Owner);
    if (!IsValid(PawnExtComp))
    {
        MTDS_ERROR("Owner [%s] must have pawn extension component.", *GetNameSafe(Owner));
        return;
    }

    // Initialize ability system component as soon as the pawn is ready to initialize
    PawnExtComp->OnPawnReadyToInitialize_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnPawnReadyToInitialize));
}

void UMTD_HeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    const auto Pawn = GetPawn<APawn>();
    if (IsValid(Pawn))
    {
        auto ExtComp = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Pawn);
        if (IsValid(ExtComp))
        {
            // If hero component has ended to play, but the pawn didn't, then uninitialize his ability system
            ExtComp->UninitializeAbilitySystem();
        }
    }

    Super::EndPlay(EndPlayReason);
}

void UMTD_HeroComponent::AddAdditionalInputConfig(const UMTD_InputConfig *InputConfig)
{
    if (!IsValid(InputConfig))
    {
        MTDS_WARN("Input config is invalid.");
        return;
    }

    auto Pawn = GetPawn<APawn>();
    if (!IsValid(Pawn))
    {
        MTDS_WARN("Owner is invalid.");
        return;
    }
    
    const bool bIsBot = Pawn->IsBotControlled();
    if (bIsBot)
    {
        MTDS_WARN("An additional input config cannot be added on a bot.");
        return;
    }
    
    const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
    if (!bIsLocallyControlled)
    {
        MTDS_WARN("An additional input config cannot be added on a non-locally controlled player.");
        return;
    }
    
    auto InputComponent = Pawn->InputComponent;
    if (!IsValid(InputComponent))
    {
        MTDS_WARN("Input component on owner [%s] is invalid.", *Pawn->GetName());
        return;
    }
    
    auto MtdInputComponent = CastChecked<UMTD_InputComponent>(InputComponent);

    // Bind all the given inputs
    MtdInputComponent->BindAbilityActions(
        InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);
}

bool UMTD_HeroComponent::IsPawnComponentReadyToInitialize() const
{
    const auto Pawn = GetPawn<APawn>();
    if (!IsValid(Pawn))
    {
        MTDS_WARN("Owner is invalid.");
        return false;
    }

    const auto Controller = GetController<AController>();
    if (!((IsValid(Controller)) || (IsValid(Controller->PlayerState))))
    {
        return false;
    }
    
    const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
    if (!bIsLocallyControlled)
    {
        return true;
    }
    
    const bool bIsBot = Pawn->IsBotControlled();
    if (!bIsBot)
    {
        // Input component is required when locally controlled
        if (!IsValid(Pawn->InputComponent))
        {
            return false;
        }
    }

    return true;
}

UMTD_AbilitySystemComponent *UMTD_HeroComponent::GetMtdAbilitySystemComponent() const
{
    const auto Pawn = GetPawn<APawn>();
    if (!IsValid(Pawn))
    {
        return nullptr;
    }

    const auto ExtPawnComp = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Pawn);
    if (!IsValid(ExtPawnComp))
    {
        return nullptr;
    }

    return ExtPawnComp->GetMtdAbilitySystemComponent();
}

void UMTD_HeroComponent::OnPawnReadyToInitialize()
{
    // Avoid initializing twice
    if (!ensure(!bPawnHasInitialized))
    {
        return;
    }

    const auto Pawn = GetPawn<APawn>();
    if (!IsValid(Pawn))
    {
        MTDS_WARN("Owner is invalid.");
        return;
    }

    const auto MtdPlayerState = Pawn->GetPlayerStateChecked<AMTD_PlayerState>();
    UMTD_AbilitySystemComponent *MtdAbilitySystemComponent = MtdPlayerState->GetMtdAbilitySystemComponent();
    check(IsValid(MtdAbilitySystemComponent));

    const auto PawnExtComp = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Pawn);
    if (!IsValid(PawnExtComp))
    {
        MTDS_WARN("Owner [%s] is missing pawn extension component.", *Pawn->GetName());
    }
    else
    {
        // Store pawn data for further use
        PawnData = PawnExtComp->GetPawnData<UMTD_PawnData>();

        // Initialize the ability system component
        PawnExtComp->InitializeAbilitySystem(MtdAbilitySystemComponent, MtdPlayerState);
    }

    // Dispatch pawn data
    if (!IsValid(PawnData))
    {
        MTDS_WARN("Pawn data asset on owner [%s] is invalid.", *Pawn->GetName());
    }
    else
    {
        // Dispatch all ability sets from pawn data
        for (const UMTD_AbilitySet *AbilitySet : PawnData->AbilitySets)
        {
            AbilitySet->GiveToAbilitySystem(MtdAbilitySystemComponent, nullptr, Pawn);
        }
    }
    
    const auto Player = Cast<AMTD_BasePlayerCharacter>(Pawn);
    if (IsValid(Player))
    {
        PlayerData = Player->GetPlayerData();
        if (!IsValid(PlayerData))
        {
            MTDS_WARN("Player data on player [%s] is invalid.", *Player->GetName());
        }
        else
        {
            // Dispatch inputs
            const auto MtdPlayerController = Pawn->GetController<AMTD_PlayerController>();
            if ((IsValid(MtdPlayerController)) && (IsValid(Pawn->InputComponent)))
            {
                InitializePlayerInput(Pawn->InputComponent);
            }
        }
    }

    // Remember that initialization has already succeeded
    bPawnHasInitialized = true;
}

void UMTD_HeroComponent::InitializePlayerInput(UInputComponent *InputComponent)
{
    check(IsValid(InputComponent));
    check(IsValid(PlayerData));

    const UMTD_InputConfig *InputConfig = PlayerData->InputConfig;
    if (!IsValid(InputConfig))
    {
        MTDS_WARN("Input config on player data [%s] is invalid.", *PlayerData->GetName());
        return;
    }

    const FMTD_GameplayTags GameplayTags = FMTD_GameplayTags::Get();
    auto MtdInputComponent = CastChecked<UMTD_InputComponent>(InputComponent);

    // Dispatch input config
    MtdInputComponent->BindAbilityActions(
        InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);

    // Shorthand native action bindings
    auto BindNativeAction = [&, this] (const FGameplayTag &GameplayTag, auto Delegate)
        {
            MtdInputComponent->BindNativeAction(InputConfig, GameplayTag, ETriggerEvent::Triggered, this, Delegate);
        };

    // Bind all actions that are meant to be hardcoded
    BindNativeAction(GameplayTags.InputTag_Move, &ThisClass::Input_Move);
    BindNativeAction(GameplayTags.InputTag_Look_Mouse, &ThisClass::Input_LookMouse);
}

void UMTD_HeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
    UMTD_AbilitySystemComponent *MtdAbilitySystemComponent = GetMtdAbilitySystemComponent();
    if (!IsValid(MtdAbilitySystemComponent))
    {
        return;
    }

    // Notify ability system component about input press
    MtdAbilitySystemComponent->OnAbilityInputTagPressed(InputTag);
}

void UMTD_HeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
    UMTD_AbilitySystemComponent *MtdAbilitySystemComponent = GetMtdAbilitySystemComponent();
    if (!IsValid(MtdAbilitySystemComponent))
    {
        return;
    }

    // Notify ability system component about input release
    MtdAbilitySystemComponent->OnAbilityInputTagReleased(InputTag);
}

void UMTD_HeroComponent::Input_Move(const FInputActionValue &InputActionValue)
{
    if (!K2_CanMove())
    {
        return;
    }

    auto Character = GetPawnChecked<ACharacter>();
    const auto Direction = InputActionValue.Get<FVector2D>();

    // Horizontal movement
    if (Direction.X != 0.f)
    {
        Character->AddMovementInput(Character->GetActorRightVector(), Direction.X);
    }

    // Vertical movement
    if (Direction.Y != 0.f)
    {
        Character->AddMovementInput(Character->GetActorForwardVector(), Direction.Y);
    }
}

void UMTD_HeroComponent::Input_LookMouse(const FInputActionValue &InputActionValue)
{
    if (!K2_CanLook())
    {
        return;
    }

    auto Character = GetPawnChecked<ACharacter>();
    const auto Direction = InputActionValue.Get<FVector2D>();

    // TODO: Store XY turn ratio

    // Horizontal rotation
    if (Direction.X != 0.f)
    {
        Character->AddControllerYawInput(Direction.X);
    }

    // Vertical rotation
    if (Direction.Y != 0.f)
    {
        Character->AddControllerPitchInput(Direction.Y);
    }
}
