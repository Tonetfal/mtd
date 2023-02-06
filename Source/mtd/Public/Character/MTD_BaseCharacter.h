#pragma once

#include "AbilitySystemInterface.h"
#include "Character/MTD_GameResultInterface.h"
#include "GameFramework/Character.h"
#include "mtd.h"

#include "MTD_BaseCharacter.generated.h"

class AMTD_PlayerState;
class UMTD_AbilitySystemComponent;
class UMTD_BalanceComponent;
class UMTD_CombatComponent;
class UMTD_HealthComponent;
class UMTD_HeroComponent;
class UMTD_ManaComponent;
class UMTD_PawnExtensionComponent;

UCLASS()
class MTD_API AMTD_BaseCharacter :
    public ACharacter, 
    public IAbilitySystemInterface,
    public IMTD_GameResultInterface
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
    
    virtual void Tick(float DeltaSeconds) override;
    //~End of AActor interface

    //~APawn interface
    virtual void NotifyControllerChanged() override;
    //~End of APawn interface

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Character")
    float GetMovementDirectionAngle() const;

protected:
    virtual void OnAbilitySystemInitialized();
    virtual void OnAbilitySystemUninitialized();
    virtual void DestroyDueToDeath();
    virtual void Uninit();

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Character")
    void OnDeathStarted(AActor *OwningActor);
    virtual void OnDeathStarted_Implementation(AActor *OwningActor);

    UFUNCTION(BlueprintNativeEvent, Category="MTD|Character")
    void OnDeathFinished(AActor *OwningActor);
    virtual void OnDeathFinished_Implementation(AActor *OwningActor);

    //~ACharacter interface
    virtual void SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) override;
    //~End of ACharacter interface

    virtual void InitializeAttributes();

    virtual void FellOutOfWorld(const UDamageType &DamageType) override;

public:
    UMTD_HealthComponent *GetHealthComponent() const;
    UMTD_ManaComponent *GetManaComponent() const;
    UMTD_BalanceComponent *GetBalanceComponent() const;
    UMTD_CombatComponent *GetCombatComponent() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    AMTD_PlayerState *GetMtdPlayerState() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Character")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End IAbilitySystemInterface interface

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_PawnExtensionComponent> PawnExtentionComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HeroComponent> HeroComponent = nullptr;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_HealthComponent> HealthComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_ManaComponent> ManaComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_BalanceComponent> BalanceComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_CombatComponent> CombatComponent = nullptr;

};

inline UMTD_HealthComponent *AMTD_BaseCharacter::GetHealthComponent() const
{
    return HealthComponent;
}

inline UMTD_ManaComponent *AMTD_BaseCharacter::GetManaComponent() const
{
    return ManaComponent;
}

inline UMTD_BalanceComponent *AMTD_BaseCharacter::GetBalanceComponent() const
{
    return BalanceComponent;
}

inline UMTD_CombatComponent *AMTD_BaseCharacter::GetCombatComponent() const
{
    return CombatComponent;
}
