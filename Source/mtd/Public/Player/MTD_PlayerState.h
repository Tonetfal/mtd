#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerState.h"
#include "mtd.h"

#include "MTD_PlayerState.generated.h"

class AMTD_BasePlayerCharacter;
class AMTD_PlayerController;
class UGameplayAbility;
class UMTD_AbilitySystemComponent;
class UMTD_EquipmentManagerComponent;
class UMTD_InventoryManagerComponent;
class UMTD_LevelComponent;

UCLASS()
class MTD_API AMTD_PlayerState
    : public APlayerState
    , public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(
        FOnHeroClassesSetSignature,
        const FGameplayTagContainer & /* HeroClasses */);

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

    UMTD_EquipmentManagerComponent *GetEquipmentManagerComponent() const;
    UMTD_InventoryManagerComponent *GetInventoryManagerComponent() const;
    UMTD_LevelComponent *GetLevelComponent() const;

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

public:
    FOnHeroClassesSetSignature OnHeroClassesSetDelegate;

protected:
    UPROPERTY(VisibleAnywhere, Category="MTD|Components")
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_EquipmentManagerComponent> EquipmentManagerComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_InventoryManagerComponent> InventoryManagerComponent = nullptr;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_LevelComponent> LevelComponent = nullptr;

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

inline UMTD_LevelComponent *AMTD_PlayerState::GetLevelComponent() const
{
    return LevelComponent;
}

inline void AMTD_PlayerState::SetHeroClasses(const FGameplayTagContainer &InHeroClasses)
{
    HeroClasses = InHeroClasses;
    OnHeroClassesSetDelegate.Broadcast(HeroClasses);
}

inline const FGameplayTagContainer &AMTD_PlayerState::GetHeroClasses() const
{
    return HeroClasses;
}

inline void AMTD_PlayerState::ClearHeroClasses()
{
    HeroClasses = FGameplayTagContainer::EmptyContainer;
}
