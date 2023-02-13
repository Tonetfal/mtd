#include "Character/MTD_PawnExtensionComponent.h"

#include "AbilitySystem/MTD_AbilityAnimationSet.h"
#include "AbilitySystem/MTD_AbilitySystemComponent.h"
#include "Character/MTD_CharacterCoreTypes.h"

UMTD_PawnExtensionComponent::UMTD_PawnExtensionComponent()
{
    // Nothing to tick for
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;
}

void UMTD_PawnExtensionComponent::OnRegister()
{
    Super::OnRegister();

    const auto Pawn = GetPawn<APawn>();
    if (!IsValid(Pawn))
    {
        MTDS_ERROR("Pawn extension component on [%s] can only be added to Pawn actors.", *GetNameSafe(GetOwner()));
        return;
    }

    TArray<UActorComponent *> PawnExtensionComponents;
    Pawn->GetComponents(StaticClass(), PawnExtensionComponents);
    const int32 Num = PawnExtensionComponents.Num();
    if (Num != 1)
    {
        MTDS_ERROR("Only one pawn extension component should exist on [%s].", *GetNameSafe(GetOwner()));
    }
}

void UMTD_PawnExtensionComponent::InitializePawnData(const UMTD_PawnData *InPawnData)
{
    if (!IsValid(InPawnData))
    {
        MTDS_WARN("Trying to initialize pawn data with an invalid pawn data.");
        return;
    }
    
    if (IsValid(PawnData))
    {
        MTDS_ERROR("Trying to initialize an already initialized pawn data [%s] with pawn data [%s].",
            *PawnData->GetName(), *InPawnData->GetName());
        return;
    }

    PawnData = InPawnData;
    CheckPawnReadyToInitialize();
}

void UMTD_PawnExtensionComponent::InitializeAbilitySystem(UMTD_AbilitySystemComponent *InAbilitySystemComponent,
    AActor *InOwnerActor)
{
    check(IsValid(InAbilitySystemComponent));
    check(IsValid(InOwnerActor));

    if (AbilitySystemComponent == InAbilitySystemComponent)
    {
        // ASC hasn't changed
        return;
    }

    if (AbilitySystemComponent)
    {
        // Clean up the old ASC
        UninitializeAbilitySystem();
    }

    auto PawnOwner = GetPawnChecked<APawn>();
    const AActor *ExistingAvatar = InAbilitySystemComponent->GetAvatarActor();

    // ASC can only have one avatar, hence if there is one, uninitialize his ASC
    if (((IsValid(ExistingAvatar)) && (ExistingAvatar != PawnOwner)))
    {
        UMTD_PawnExtensionComponent *OtherExtComp = FindPawnExtensionComponent(ExistingAvatar);
        if (IsValid(OtherExtComp))
        {
            OtherExtComp->UninitializeAbilitySystem();
        }
    }

    // Save the ASC
    AbilitySystemComponent = InAbilitySystemComponent;

    // Use passed actor as the ASC owner, and use our owner as the ASC actor
    AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, PawnOwner);

    // Notify about ASC initialization
    OnAbilitySystemInitialized.Broadcast();

    MTDS_VERBOSE("Ability system component [%s] on pawn [%s] owner [%s] has been initialized.",
        *GetNameSafe(InAbilitySystemComponent), *GetNameSafe(PawnOwner), *GetNameSafe(InOwnerActor));
}

void UMTD_PawnExtensionComponent::UninitializeAbilitySystem()
{
    // Avoid running the logic if ASC is nullptr
    if (!AbilitySystemComponent)
    {
        return;
    }

    // Uninitialize ASC only if we still are holding the ownership
    const AActor *Avatar = AbilitySystemComponent->GetAvatarActor();
    const AActor *Owner = GetOwner();
    if (Avatar == Owner)
    {
        // Clear main ASC data
        AbilitySystemComponent->CancelAbilities(nullptr, nullptr);
        AbilitySystemComponent->ClearAbilityInput();
        AbilitySystemComponent->RemoveAllGameplayCues();

        // Clear ASC actor info
        const AActor *AscOwner = AbilitySystemComponent->GetOwnerActor();
        if (AscOwner)
        {
            AbilitySystemComponent->SetAvatarActor(nullptr);
        }
        else
        {
            AbilitySystemComponent->ClearActorInfo();
        }

        // Notify about ASC uninitialization
        OnAbilitySystemUninitialized.Broadcast();
    }

    // Nullify stored ASC
    AbilitySystemComponent = nullptr;
}

void UMTD_PawnExtensionComponent::HandleControllerChanged()
{
    if (IsValid(AbilitySystemComponent))
    {
        const AActor *Avatar = AbilitySystemComponent->GetAvatarActor();
        const AActor *Owner = GetPawnChecked<APawn>();
        if (Avatar == Owner)
        {
            const AActor *AscOwner = AbilitySystemComponent->GetOwnerActor();
            ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AscOwner);
            
            if (!IsValid(AscOwner))
            {
                UninitializeAbilitySystem();
            }
            else
            {
                AbilitySystemComponent->RefreshAbilityActorInfo();
            }
        }
    }

    CheckPawnReadyToInitialize();
}

void UMTD_PawnExtensionComponent::SetupPlayerInputComponent()
{
    CheckPawnReadyToInitialize();
}

bool UMTD_PawnExtensionComponent::CheckPawnReadyToInitialize()
{
    if (bPawnReadyToInitialize)
    {
        return true;
    }

    // Check whether all components are ready to initialize
    const auto Pawn = GetPawnChecked<APawn>();
    TArray<UActorComponent *> InteractableComponents = Pawn->GetComponentsByInterface(
        UMTD_ReadyInterface::StaticClass());
    for (UActorComponent *InteractableComponent : InteractableComponents)
    {
        const auto Ready = CastChecked<IMTD_ReadyInterface>(InteractableComponent);
        if (!Ready->IsPawnComponentReadyToInitialize())
        {
            // A component is not ready to initialize, hence we're not ready yet
            return false;
        }
    }

    // Pawn is ready to initialize
    bPawnReadyToInitialize = true;
    OnPawnReadyToInitialize.Broadcast();
    BP_OnPawnReadyToInitialize.Broadcast();

    return true;
}

void UMTD_PawnExtensionComponent::OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
    // Avoid registering if already did
    if (!OnPawnReadyToInitialize.IsBoundToObject(Delegate.GetUObject()))
    {
        // Register the delegate
        OnPawnReadyToInitialize.Add(Delegate);
    }

    if (bPawnReadyToInitialize)
    {
        // Call the delegate if pawn is already ready to initialize
        Delegate.Execute();
    }
}

void UMTD_PawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(
    FSimpleMulticastDelegate::FDelegate Delegate)
{
    // Avoid registering if already did
    if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
    {
        // Register the delegate
        OnAbilitySystemInitialized.Add(Delegate);
    }

    if (AbilitySystemComponent)
    {
        // Call the delegate if ability system component is already ready to initialize
        Delegate.Execute();
    }
}

void UMTD_PawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
    // Avoid registering if already did
    if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
    {
        // Register the delegate
        OnAbilitySystemUninitialized.Add(Delegate);
    }
}

void UMTD_PawnExtensionComponent::GetAbilityAnimMontages(const FGameplayTag &AbilityTag,
    FMTD_AbilityAnimations &OutAbilityAnimations) const
{
    if (!IsValid(AnimationSet))
    {
        MTDS_WARN("Animation set is invalid.");
        return;
    }
    
    AnimationSet->GetAbilityAnimMontages(AbilityTag, OutAbilityAnimations);
}

const UAnimMontage *UMTD_PawnExtensionComponent::GetRandomAnimMontage(const FGameplayTag &AbilityTag) const
{
    // Get the animations
    FMTD_AbilityAnimations AbilityAnimations;
    GetAbilityAnimMontages(AbilityTag, AbilityAnimations);
    TArray<TObjectPtr<UAnimMontage>> Animations = AbilityAnimations.Animations;

    // Check if number is positive
    const int32 Num = Animations.Num();
    if (Num == 0)
    {
        return nullptr;
    }

    // Randomize the index
    const int32 Index = FMath::RandRange(0, (Num - 1));

    // Get the randomized animation montage
    const UAnimMontage *Animation = Animations[Index];
    return Animation;
}
