#pragma once

#include "mtd.h"
#include "MTD_PawnComponent.h"

#include "MTD_PawnExtensionComponent.generated.h"

class UMTD_AbilityAnimationSet;
class UMTD_AbilitySystemComponent;
class UMTD_PawnData;
struct FGameplayTag;
struct FMTD_AbilityAnimations;

UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_PawnExtensionComponent : public UMTD_PawnComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynamicMulticastSignature);

public:
    UMTD_PawnExtensionComponent();

    UFUNCTION(BlueprintPure, Category="MTD|Pawn")
    static UMTD_PawnExtensionComponent *FindPawnExtensionComponent(const AActor *Actor);

    void SetPawnData(const UMTD_PawnData *InPawnData);

    void InitializeAbilitySystem(UMTD_AbilitySystemComponent *InAsc, AActor *InOwnerActor);
    void UninitializeAbilitySystem();

    void HandleControllerChanged();
    void SetupPlayerInputComponent();

    bool CheckPawnReadyToInitialize();

    UFUNCTION(BlueprintCallable, Category="MTD|Pawn", meta=(ExpandBoolAsExecs="ReturnValue"))
    bool IsPawnReadyToInitialize() const;

    void OnPawnReadyToInitialize_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
    void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
    void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    FMTD_AbilityAnimations GetAbilityAnimMontages(FGameplayTag AbilityTag) const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    UAnimMontage *GetRandomAnimMontage(FGameplayTag AbilityTag) const;

protected:
    virtual void OnRegister() override;

public:
    template <class T>
    const T *GetPawnData() const;

    UFUNCTION(BlueprintPure, Category = "MTD|Pawn")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

protected:
    FSimpleMulticastDelegate OnPawnReadyToInitialize;

    UPROPERTY(BlueprintAssignable, meta=(DisplayName="On Pawn Ready To Initialize"))
    FDynamicMulticastSignature BP_OnPawnReadyToInitialize;

    FSimpleMulticastDelegate OnAbilitySystemInitialized;
    FSimpleMulticastDelegate OnAbilitySystemUninitialized;

private:
    UPROPERTY()
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MTD|Pawn", meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_PawnData> PawnData = nullptr;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Pawn", meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_AbilityAnimationSet> AnimationSet = nullptr;

    bool bPawnReadyToInitialize = false;
};

inline UMTD_PawnExtensionComponent *UMTD_PawnExtensionComponent::FindPawnExtensionComponent(const AActor *Actor)
{
    return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_PawnExtensionComponent>()) : (nullptr);
}

inline bool UMTD_PawnExtensionComponent::IsPawnReadyToInitialize() const
{
    return bPawnReadyToInitialize;
}

template <class T>
inline const T *UMTD_PawnExtensionComponent::GetPawnData() const
{
    return Cast<T>(PawnData);
}

inline UMTD_AbilitySystemComponent *UMTD_PawnExtensionComponent::GetMtdAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
