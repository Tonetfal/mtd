#pragma once

#include "Engine/DataAsset.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "mtd.h"

#include "MTD_AbilitySet.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_GameplayAbility;
class UGameplayEffect;


/**
 * FMTD_AbilitySet_GameplayAbility
 *
 *	Data used by the ability set to grant gameplay abilities.
 */
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
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


/**
 * FMTD_AbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct FMTD_AbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/**
 * FMTD_AbilitySet_AttributeSet
 *
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct FMTD_AbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet = nullptr;
};

/**
 * FMTD_AbilitySet_GrantedHandles
 *
 *	Data used to store handles to what has been granted by the ability set.
 */
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
	// Handles to the granted abilities
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


/**
 * UMTD_AbilitySet
 *
 *	Non-mutable data asset used to grant gameplay abilities and gameplay effects.
 */
UCLASS(BlueprintType, Const)
class UMTD_AbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMTD_AbilitySet();

	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	void GiveToAbilitySystem(
		UMTD_AbilitySystemComponent *MtdAsc,
		FMTD_AbilitySet_GrantedHandles *OutGrantedHandles,
		UObject *SourceObject = nullptr) const;

protected:
	// Gameplay abilities to grant when this ability set is granted
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities",
		meta=(TitleProperty=Ability))
	TArray<FMTD_AbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects",
		meta=(TitleProperty=GameplayEffect))
	TArray<FMTD_AbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets",
		meta=(TitleProperty=AttributeSet))
	TArray<FMTD_AbilitySet_AttributeSet> GrantedAttributes;
};
