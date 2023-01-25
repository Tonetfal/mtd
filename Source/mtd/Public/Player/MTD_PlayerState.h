#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "mtd.h"

#include "MTD_PlayerState.generated.h"

class AMTD_BasePlayerCharacter;
class UMTD_InventoryManagerComponent;
class UGameplayAbility;
class AMTD_PlayerController;
class UMTD_AbilitySystemComponent;
class UMTD_EquipmentManagerComponent;

UCLASS()
class MTD_API AMTD_PlayerState
    : public APlayerState
    , public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AMTD_PlayerState();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Player State")
    AMTD_PlayerController *GetMtdPlayerController() const;
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Player State")
    AMTD_BasePlayerCharacter *GetMtdPlayerCharacter() const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Player State")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const;

    //~IAbilitySystemInterface Interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End of IAbilitySystemInterface Interface

    UFUNCTION(BlueprintCallable, BlueprintPure, Category="MTD|Player State")
    UMTD_EquipmentManagerComponent *GetEquipmentManagerComponent() const;
    UMTD_InventoryManagerComponent *GetInventoryManagerComponent() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Ability")
    virtual void GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, int32 InputCode);

    UFUNCTION(BlueprintCallable, Category="MTD|Ability")
    virtual void ActivateAbility(int32 InputCode);

    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitializeComponents() override;
    //~End of AActor interface

    void SetHeroClasses(const FGameplayTagContainer &InHeroClasses);
    const FGameplayTagContainer &GetHeroClasses() const;
    void ClearHeroClasses();

protected:
    UPROPERTY(VisibleAnywhere, Category="MTD|Components")
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_EquipmentManagerComponent> EquipmentManagerComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_InventoryManagerComponent> InventoryManagerComponent = nullptr;

    UPROPERTY()
    FGameplayTagContainer HeroClasses;
};

inline UMTD_AbilitySystemComponent *AMTD_PlayerState::GetMtdAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

inline UMTD_EquipmentManagerComponent *AMTD_PlayerState::GetEquipmentManagerComponent() const
{
    return EquipmentManagerComponent;
}

inline UMTD_InventoryManagerComponent *AMTD_PlayerState::GetInventoryManagerComponent() const
{
    return InventoryManagerComponent;
}

inline void AMTD_PlayerState::SetHeroClasses(const FGameplayTagContainer &InHeroClasses)
{
    HeroClasses = InHeroClasses;
}

inline const FGameplayTagContainer &AMTD_PlayerState::GetHeroClasses() const
{
    return HeroClasses;
}

inline void AMTD_PlayerState::ClearHeroClasses()
{
    HeroClasses = FGameplayTagContainer::EmptyContainer;
}
