#pragma once

#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "mtd.h"

#include "MTD_PlayerState.generated.h"

class UGameplayAbility;
class AMTD_PlayerController;
class UMTD_AbilitySystemComponent;

UCLASS()
class MTD_API AMTD_PlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AMTD_PlayerState();

    UFUNCTION(BlueprintCallable, Category = "MTD|PlayerState")
    AMTD_PlayerController *GetMtdPlayerController() const;

    UFUNCTION(BlueprintCallable, Category="MTD|PlayerState")
    UMTD_AbilitySystemComponent *GetMtdAbilitySystemComponent() const
    {
        return AbilitySystemComponent;
    }

    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

    UFUNCTION(BlueprintCallable, Category="MTD|Ability")
    virtual void GrantAbility(TSubclassOf<UGameplayAbility> AbilityClass, int32 Level, int32 InputCode);

    UFUNCTION(BlueprintCallable, Category="MTD|Ability")
    virtual void ActivateAbility(int32 InputCode);

    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitializeComponents() override;
    //~End of AActor interface

protected:
    UPROPERTY(VisibleAnywhere, Category="MTD|Components")
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;
};
