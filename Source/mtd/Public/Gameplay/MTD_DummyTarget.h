#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Actor.h"
#include "mtd.h"

#include "MTD_DummyTarget.generated.h"

class UCapsuleComponent;
class UMTD_AbilitySystemComponent;

UCLASS()
class MTD_API AMTD_DummyTarget
    : public AActor
    , public IAbilitySystemInterface
    , public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    AMTD_DummyTarget();

    //~AActor Interface
    virtual void PostInitializeComponents() override;
    //~End of AActor Interface

    //~IAbilitySystemInterface Interface
    virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;
    //~End of IAbilitySystemInterface Interface
    
    //~IGenericTeamAgentInterface Interface
    virtual FGenericTeamId GetGenericTeamId() const override;
    //~End of IGenericTeamAgentInterface Interface

protected:
    UFUNCTION(BlueprintNativeEvent, DisplayName="OnHealthDecrease")
    void K2_OnHealthDecrease(float LostHealth);

private:
    void OnHealthChanged(const FOnAttributeChangeData &Attribute);

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MTD|Components", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;
};
