#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "mtd.h"
#include "MTD_GameResultInterface.h"
#include "AbilitySystem/MTD_AbilityAnimationSet.h"

#include "MTD_BaseCharacter.generated.h"

class UMTD_HeroComponent;
class UMTD_HealthComponent;
class UMTD_ManaComponent;
class UMTD_PawnExtensionComponent;
class UMTD_AbilitySystemComponent;
class AMTD_PlayerState;

UCLASS()
class MTD_API AMTD_BaseCharacter : public ACharacter, public IAbilitySystemInterface, public IMTD_GameResultInterface
    // public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    AMTD_BaseCharacter();

    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitProperties() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Reset() override;
    //~End of AActor interface

    //~APawn interface
    virtual void NotifyControllerChanged() override;
    //~End of APawn interface

    FMTD_AbilityAnimations GetAbilityAnimMontages(FGameplayTag AbilityTag) const;

protected:
    virtual void OnAbilitySystemInitialized();
    virtual void OnAbilitySystemUninitialized();
    virtual void DestroyDueToDeath();
    virtual void Uninit();

    UFUNCTION(BlueprintNativeEvent, Category="Character")
    void OnDeathStarted(AActor *OwningActor);
    virtual void OnDeathStarted_Implementation(AActor *OwningActor);

    UFUNCTION(BlueprintNativeEvent, Category="Character")
    void OnDeathFinished(AActor *OwningActor);
    virtual void OnDeathFinished_Implementation(AActor *OwningActor);

    virtual void DisableControllerInput();
    virtual void DisableMovement();
    virtual void DisableCollision();

    //~APlayer interface
    virtual void SetupPlayerInputComponent(
        UInputComponent *PlayerInputComponent) override;
    //~End of APlayer interface

    virtual void FellOutOfWorld(const UDamageType &DamageType) override;

public:
    UMTD_HealthComponent *GetHealthComponent() const
    {
        return HealthComponent;
    }

    UMTD_ManaComponent *GetManaComponent() const
    {
        return ManaComponent;
    }

    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    AMTD_PlayerState *GetMtdPlayerState() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PawnExtensionComponent> PawnExtentionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroComponent> HeroComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_ManaComponent> ManaComponent = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MTD|Ability System",
        meta=(AllowPrivateAccess="true"))
    TObjectPtr<const UMTD_AbilityAnimationSet> AnimationSet = nullptr;
};