#pragma once

#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_AbilitySet.generated.h"

class UGameplayEffect;
class UMTD_AbilitySystemComponent;
class UMTD_AttributeSet;
class UMTD_GameplayAbility;
class UMTD_GameplayEffect;

/**
 * Definition of a single gameplay ability.
 */
USTRUCT(BlueprintType)
struct FMTD_AbilitySet_GameplayAbility
{
    GENERATED_BODY()

public:
    /** Gameplay ability to grant. */
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UMTD_GameplayAbility> Ability = nullptr;

    /** Level of ability to grant. */
    UPROPERTY(EditDefaultsOnly)
    int32 AbilityLevel = 1;

    /** Tag used to process input for the ability. */
    UPROPERTY(EditDefaultsOnly, meta=(Category="InputTag"))
    FGameplayTag InputTag;
};

/**
 * Definition of a single gameplay effect.
 */
USTRUCT(BlueprintType)
struct FMTD_AbilitySet_GameplayEffect
{
    GENERATED_BODY()

public:
    /** Gameplay effect to grant. */
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UMTD_GameplayEffect> GameplayEffect = nullptr;

    /** Level of gameplay effect to grant. */
    UPROPERTY(EditDefaultsOnly)
    float EffectLevel = 1.0f;
};

/**
 * Definition of a single attribute set.
 */
USTRUCT(BlueprintType)
struct FMTD_AbilitySet_AttributeSet
{
    GENERATED_BODY()

public:
    /** Gameplay effect to grant. */
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UMTD_AttributeSet> AttributeSet = nullptr;
};

/**
 * Container of handles relative to all granted abilities, gameplay effects, and attribute sets.
 */
USTRUCT(BlueprintType)
struct FMTD_AbilitySet_GrantedHandles
{
    GENERATED_BODY()

public:
    /**
     * Add an ability spec handle to  relative set.
     * @param   Handle: ability spec handle to add.
     */
    void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle &Handle);
    
    /**
     * Add an attribute set to  relative set.
     * @param   AttributeSet: attribute set to add.
     */
    void AddAttributeSet(UAttributeSet *AttributeSet);
    
    /**
     * Add an effect handle to  relative set.
     * @param   Handle: effect handle to add.
     */
    void AddGameplayEffectHandle(const FActiveGameplayEffectHandle &Handle);

    /**
     * Take all the containing abilities, gameplay effects, and attribute sets from a given ability system component.
     * @param   AbilitySystemComponent: ability system component to take from.
     */
    void TakeFromAbilitySystem(UAbilitySystemComponent *AbilitySystemComponent);

protected:
    /** Handles to the granted abilities. */
    UPROPERTY(BlueprintReadOnly)
    TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

    /** Handles to the granted gameplay effects. */
    UPROPERTY(BlueprintReadOnly)
    TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;
    
    /** Pointers to the granted attribute sets. */
    UPROPERTY(BlueprintReadOnly)
    TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 *	Non-mutable data asset used to grant gameplay abilities, gameplay effects, and attribute sets.
 */
UCLASS(BlueprintType, Const)
class UMTD_AbilitySet
    : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    /**
     * Grant the ability set to the specified ability system component. The returned handles can be used later to take
     * away anything that was granted.
     * @param   MtdAbilitySystemComponent: ability system component to grant the set to.
     * @param   OutGrantedHandles: container of handles and pointers to the granted data.
     * @param   SourceObject: object to assosiate some granted data with.
     */
    void GiveToAbilitySystem(
        UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
        FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
        UObject *SourceObject = nullptr) const;

    /**
     * Grant the ability set to the specified ability system component. The returned handles can be used later to take
     * away anything that was granted.
     * @param   MtdAbilitySystemComponent: ability system component to grant the set to.
     * @param   OutGrantedHandles: container of handles and pointers to the granted data.
     * @param   SourceObject: object to assosiate some granted data with.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Ability Set", meta=(DisplayName="GiveToAbilitySystem"))
    void K2_GiveToAbilitySystem(
        UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
        FMTD_AbilitySet_GrantedHandles &OutGrantedHandles,
        UObject *SourceObject = nullptr);

private:
    /**
     * Grant all gameplay abilities to the specified ability system component.
     * @param   MtdAbilitySystemComponent: ability system component to grant the abilities to.
     * @param   OutGrantedHandles: container of granted ability handles.
     * @param   SourceObject: object to assosiate the ability source object with.
     */
    void GrantAbilities(
        UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
        FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
        UObject *SourceObject) const;

    /**
     * Grant all attributes sets to the specified ability system component.
     * @param   MtdAbilitySystemComponent: ability system component to grant the attribute sets to.
     * @param   OutGrantedHandles: container of granted ability set pointers.
     * @param   SourceObject: not used.
     */
    void GrantAttributes(
        UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
        FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
        UObject *SourceObject) const;

    /**
     * Grant all gameplay effects to the specified ability system component.
     * @param   MtdAbilitySystemComponent: ability system component to grant the effects to.
     * @param   OutGrantedHandles: container of granted effect handles.
     * @param   SourceObject: not used.
     */
    void GrantEffects(
        UMTD_AbilitySystemComponent *MtdAbilitySystemComponent,
        FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
        UObject *SourceObject) const;

protected:
    /** Gameplay abilities to grant when this ability set is granted. */
    UPROPERTY(EditDefaultsOnly, Category="Gameplay Abilities", meta=(TitleProperty="Ability"))
    TArray<FMTD_AbilitySet_GameplayAbility> GrantedGameplayAbilities;

    /** Gameplay effects to grant when this ability set is granted. */
    UPROPERTY(EditDefaultsOnly, Category="Gameplay Effects", meta=(TitleProperty="GameplayEffect"))
    TArray<FMTD_AbilitySet_GameplayEffect> GrantedGameplayEffects;

    /** Attrenute sets to grant when this ability set is granted. */
    UPROPERTY(EditDefaultsOnly, Category="Attribute Sets", meta=(TitleProperty="AttributeSet"))
    TArray<FMTD_AbilitySet_AttributeSet> GrantedAttributes;
};
