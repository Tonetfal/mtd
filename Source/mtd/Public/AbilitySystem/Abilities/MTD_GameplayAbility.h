#pragma once

#include "Abilities/GameplayAbility.h"
#include "mtd.h"

#include "MTD_GameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EMTD_AbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

UCLASS(Abstract, HideCategories=Input, 
	meta=(ShortTooltip="The base gameplay ability class used by this project."))
class MTD_API UMTD_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMTD_GameplayAbility();

	virtual void OnGiveAbility(
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilitySpec &Spec) override;

	EMTD_AbilityActivationPolicy GetActivationPolicy() const
		{ return ActivationPolicy; }
	
	virtual const FGameplayTagContainer *GetCooldownTags() const override;
	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

private:
	void TryActivateAbilityOnSpawn(
		const FGameplayAbilityActorInfo *ActorInfo,
		const FGameplayAbilitySpec &Spec) const;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cooldown")
	FScalableFloat CooldownDuration;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="MTD|Activation",
		meta=(AllowPrivateAccess="true"))
	EMTD_AbilityActivationPolicy ActivationPolicy =
		EMTD_AbilityActivationPolicy::OnInputTriggered;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MTD|Cooldown",
		meta=(AllowPrivateAccess="true"))
	FGameplayTagContainer CooldownTags;

	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
};
