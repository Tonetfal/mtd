#pragma once

#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_AbilitySet.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_GameplayAbility;
class UMTD_GameplayEffect;
class UMTD_AttributeSet;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FMTD_AbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMTD_GameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, meta=(Category="InputTag"))
	FGameplayTag InputTag;
};

USTRUCT(BlueprintType)
struct FMTD_AbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMTD_GameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct FMTD_AbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMTD_AttributeSet> AttributeSet = nullptr;
};

USTRUCT(BlueprintType)
struct FMTD_AbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle &Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle &Handle);
	void AddAttributeSet(UAttributeSet *Set);

	void TakeFromAbilitySystem(UMTD_AbilitySystemComponent *MtdAsc);

protected:
	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets.
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


/**
 *	Non-mutable data asset used to grant gameplay abilities, gameplay effects
 *	and attribute sets.
 */
UCLASS(BlueprintType, Const)
class UMTD_AbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was
	// granted.
	void GiveToAbilitySystem(
		UMTD_AbilitySystemComponent *MtdAsc,
		FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
		UObject *SourceObject = nullptr) const;

	UFUNCTION(BlueprintCallable, Category="MTD|Ability Set",
		meta=(DisplayName="GiveToAbilitySystem"))
	void K2_GiveToAbilitySystem(
		UMTD_AbilitySystemComponent *MtdAsc,
		FMTD_AbilitySet_GrantedHandles &OutGrantedHandles,
		UObject *SourceObject = nullptr);

private:
	void GrantAbilities(
		UMTD_AbilitySystemComponent *MtdAsc,
		FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
		UObject *SourceObject) const;
	
	void GrantEffects(
		UMTD_AbilitySystemComponent *MtdAsc,
		FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
		UObject *SourceObject) const;
	
	void GrantAttributes(
		UMTD_AbilitySystemComponent *MtdAsc,
		FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
		UObject *SourceObject) const;

protected:
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities",
		meta=(TitleProperty=Ability))
	TArray<FMTD_AbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects",
		meta=(TitleProperty=GameplayEffect))
	TArray<FMTD_AbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets",
		meta=(TitleProperty=AttributeSet))
	TArray<FMTD_AbilitySet_AttributeSet> GrantedAttributes;
};
