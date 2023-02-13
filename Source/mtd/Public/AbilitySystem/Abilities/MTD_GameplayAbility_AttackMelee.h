#pragma once

#include "AbilitySystem/Abilities/MTD_GameplayAbility_Attack.h"
#include "mtd.h"

#include "MTD_GameplayAbility_AttackMelee.generated.h"

/**
 * Gameplay ability to perform a melee attack logic, such as causing a character to lose health or to get knockbacked.
 *
 * Note: This is an abstract class because it lacks gameplay event listening, it should be done inside blueprints. 
 * Without it we don't know when to perform hit logic.
 */
UCLASS(Abstract)
class MTD_API UMTD_GameplayAbility_AttackMelee
    : public UMTD_GameplayAbility_Attack
{
    GENERATED_BODY()

protected:
    /**
     * Perform the hit logic, causing a character to lose health and get knockbacked.
     * @param   Payload: event data containing core information to perform a hit on a target.
     */
    UFUNCTION(BlueprintCallable, Category="MTD|Attack Melee Gameplay Ability")
    void OnHit(const FGameplayEventData &Payload);

private:
    /**
     * Create and setup a balance damage spec handle.
     * @param   KnockbackDirection: knockback direction a character has to knockback towards.
     * @param   EffectContext: gameplay effect context to create the balance damage spec with.
     * @return  Gameplay effect spec handle to damage a character's balance.
     */
    FGameplayEffectSpecHandle GetBalanceDamageSpecHandle(const FVector KnockbackDirection,
        const FGameplayEffectContextHandle &EffectContext) const;

    /**
     * Create and setup a damage spec handle.
     * @param   EffectContext: gameplay effect context to create the damage spec with.
     * @return  Gameplay effect spec handle to damage a character.
     */
    FGameplayEffectSpecHandle GetDamageSpecHandle(const FGameplayEffectContextHandle &EffectContext) const;

    /**
     * Compute what direction a target has to knockback towards.
     * @param   TargetActor: the target that may be knockbacked.
     * @return  Direction a target has to knockback towards.
     */
    FVector ComputeKnockbackDirection(const AActor *TargetActor) const;

protected:
    /** Gameplay Effect to use to apply damage. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Attack Melee Gameplay Ability")
    TSubclassOf<UGameplayEffect> GameplayEffectDamageInstantClass = nullptr;
    
    /** Gameplay Effect to use to apply balance damage. */
    UPROPERTY(EditDefaultsOnly, Category="MTD|Attack Melee Gameplay Ability")
    TSubclassOf<UGameplayEffect> GameplayEffectBalanceDamageInstantClass = nullptr;
};
