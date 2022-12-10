#include "Character/MTD_HeroComponent.h"

#include "AbilitySystem/MTD_AbilitySet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "AbilitySystem/MTD_GameplayTags.h"
#include "Character/MTD_BaseCharacter.h"
#include "Character/MTD_CharacterCoreTypes.h"
#include "Character/MTD_PawnData.h"
#include "Character/MTD_PawnExtensionComponent.h"
#include "GameFramework/Character.h"
#include "Input/MTD_InputComponent.h"
#include "Player/MTD_PlayerController.h"
#include "Player/MTD_PlayerState.h"

UMTD_HeroComponent::UMTD_HeroComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMTD_HeroComponent::AddAdditionalInputConfig(const UMTD_InputConfig *InputConfig)
{
    check(InputConfig);

    auto Owner = GetPawn<APawn>();
    if (!IsValid(Owner))
    {
        return;
    }

    auto InputComponent = Owner->FindComponentByClass<UInputComponent>();
    check(InputComponent);

    auto MtdInputComponent = CastChecked<UMTD_InputComponent>(InputComponent);

    MtdInputComponent->BindAbilityActions(
        InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);
}

void UMTD_HeroComponent::OnRegister()
{
    Super::OnRegister();

    const auto Owner = GetPawn<APawn>();
    if (!IsValid(Owner))
    {
        MTDS_ERROR("Owner [%s] must be a Pawn actor", *GetNameSafe(GetOwner()));
        return;
    }

    auto PawnExtComp = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Owner);
    if (!IsValid(PawnExtComp))
    {
        MTDS_ERROR("Owner [%s] must have MTD_PawnExtensionComponent", *GetNameSafe(Owner));
        return;
    }

    PawnExtComp->OnPawnReadyToInitialize_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnPawnReadyToInitialize));
}

void UMTD_HeroComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UMTD_HeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    const auto Pawn = GetPawn<APawn>();
    if (IsValid(Pawn))
    {
        auto ExtComp = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Pawn);
        if (IsValid(ExtComp))
        {
            ExtComp->UninitializeAbilitySystem();
        }
    }

    Super::EndPlay(EndPlayReason);
}

bool UMTD_HeroComponent::IsPawnComponentReadyToInitialize() const
{
    const auto Pawn = GetPawn<APawn>();

    // A pawn is required
    if (!IsValid(Pawn))
    {
        return false;
    }

    const auto Controller = GetController<AController>();

    const bool bHasControllerPairedWithPS =
        (Controller) && (Controller->PlayerState) && (Controller->PlayerState->GetOwner() == Controller);

    if (!bHasControllerPairedWithPS)
    {
        return false;
    }

    const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
    const bool bIsBot = Pawn->IsBotControlled();

    if (bIsLocallyControlled && !bIsBot)
    {
        // The input component is required when locally controlled
        if (!Pawn->InputComponent)
        {
            return false;
        }
    }

    return true;
}

void UMTD_HeroComponent::OnPawnReadyToInitialize()
{
    // Don't initialize twice
    if (!ensure(!bPawnHasInitialized))
    {
        return;
    }

    const auto Pawn = GetPawn<APawn>();
    if (!IsValid(Pawn))
    {
        return;
    }

    const auto MtdPs = Pawn->GetPlayerStateChecked<AMTD_PlayerState>();
    check(MtdPs);

    UMTD_AbilitySystemComponent *MtdAsc = MtdPs->GetMtdAbilitySystemComponent();

    PawnData = nullptr;
    const auto PawnExtComp = UMTD_PawnExtensionComponent::FindPawnExtensionComponent(Pawn);
    if (IsValid(PawnExtComp))
    {
        PawnData = PawnExtComp->GetPawnData<UMTD_PawnData>();
        PlayerData = PawnExtComp->GetPlayerData<UMTD_PlayerData>();

        PawnExtComp->InitializeAbilitySystem(MtdAsc, MtdPs);
    }

    if (IsValid(PawnData))
    {
        for (const UMTD_AbilitySet *AbilitySet : PawnData->AbilitySets)
        {
            AbilitySet->GiveToAbilitySystem(MtdAsc, nullptr, GetOwner());
        }
    }
    else
    {
        MTDS_WARN("Pawn Data asset on owner [%s] is invalid!", *Pawn->GetName());
    }

    const auto MtdPc = Pawn->GetController<AMTD_PlayerController>();
    if ((IsValid(MtdPc)) && (IsValid(Pawn->InputComponent)))
    {
        InitializePlayerInput(Pawn->InputComponent);
    }

    bPawnHasInitialized = true;
}

void UMTD_HeroComponent::InitializePlayerInput(UInputComponent *InputComponent)
{
    check(InputComponent);
    check(PlayerData);

    auto MtdInputComponent = CastChecked<UMTD_InputComponent>(InputComponent);

    const FMTD_GameplayTags GameplayTags = FMTD_GameplayTags::Get();
    UMTD_InputConfig *InputConfig = PlayerData->InputConfig;

    if (!IsValid(InputConfig))
    {
        MTDS_WARN("Input Config on Player Data [%s] is invalid", *PawnData->GetName());
        return;
    }

    MtdInputComponent->BindAbilityActions(
        InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased);

    auto BindNativeAction =
        [&, this](FGameplayTag GameplayTag, auto Delegate)
    {
        MtdInputComponent->BindNativeAction(InputConfig, GameplayTag, ETriggerEvent::Triggered, this, Delegate);
    };

    BindNativeAction(GameplayTags.InputTag_Move, &ThisClass::Input_Move);
    BindNativeAction(GameplayTags.InputTag_Look_Mouse, &ThisClass::Input_LookMouse);
}

void UMTD_HeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
    UMTD_AbilitySystemComponent *MtdAsc = GetMtdAbilitySystemComponent();
    if (!IsValid(MtdAsc))
    {
        return;
    }

    MtdAsc->OnAbilityInputTagPressed(InputTag);
}

void UMTD_HeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
    UMTD_AbilitySystemComponent *MtdAsc = GetMtdAbilitySystemComponent();
    if (!IsValid(MtdAsc))
    {
        return;
    }

    MtdAsc->OnAbilityInputTagReleased(InputTag);
}

void UMTD_HeroComponent::Input_Move(const FInputActionValue &InputActionValue)
{
    if (!K2_CanMove())
    {
        return;
    }

    auto Character = GetPawn<ACharacter>();
    check(Character);

    const FVector2D Dir = InputActionValue.Get<FVector2D>();

    if (Dir.X != 0.f)
    {
        Character->AddMovementInput(Character->GetActorRightVector(), Dir.X);
    }

    if (Dir.Y != 0.f)
    {
        Character->AddMovementInput(Character->GetActorForwardVector(), Dir.Y);
    }
}

void UMTD_HeroComponent::Input_LookMouse(const FInputActionValue &InputActionValue)
{
    if (!K2_CanLook())
    {
        return;
    }

    auto Character = GetPawn<ACharacter>();
    check(Character);

    const FVector2D Dir = InputActionValue.Get<FVector2D>();

    // TODO: Store XY turn ratio

    if (Dir.X != 0.f)
    {
        Character->AddControllerYawInput(Dir.X);
    }

    if (Dir.Y != 0.f)
    {
        Character->AddControllerPitchInput(Dir.Y);
    }
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
