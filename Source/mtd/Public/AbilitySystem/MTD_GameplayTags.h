#pragma once

#include "GameplayTagContainer.h"
#include "mtd.h"

class UGameplayTagsManager;

/**
 * Singleton container of native gameplay tags.
 * 
 * Note: Gameplay tags should NOT be used before InitializeNativeTags on this class is called.
 */
struct FMTD_GameplayTags
{
public:
    /**
     * Singleton instance getter.
     */
    static const FMTD_GameplayTags &Get();

    /**
     * Initialize all the native tags declared by this struct. Should be called only once after gameplay tags manager
     * has been created.
     */
    static void InitializeNativeTags();

    /**
     * Check whether the given container contains Gameplay.Hero.All or not.
     * @return  If true, given container contains Gameplay.Hero.All, false otherwise.
     */
    static bool IsForAllHeroClasses(const FGameplayTagContainer& InTags);

    /**
     * Add a gameplay tag to the native gameplay tags container.
     * @param   OutTag: added gameplay tag reference.
     * @param   TagName: name to give to the added gameplay tag.
     * @param   TagComment: comment to give to the added gameplay tag.
     */
    static void AddTag(FGameplayTag &OutTag, const ANSICHAR *TagName, const ANSICHAR *TagComment);

private:
    /**
     * Add all gameplay tags contained inside this class.
     */
    void AddAllTags();


private:
    /** Singleton instance of this class. */
    static FMTD_GameplayTags GameplayTags;
    
public:
    FGameplayTag Ability_ActivateFail_IsDead;
    FGameplayTag Ability_ActivateFail_Cooldown;
    FGameplayTag Ability_ActivateFail_Cost;
    FGameplayTag Ability_ActivateFail_TagsBlocked;
    FGameplayTag Ability_ActivateFail_TagsMissing;
    FGameplayTag Ability_ActivateFail_ActivationGroup;

    FGameplayTag Gameplay_Ability_Behavior_SurvivesDeath;
    
    FGameplayTag Gameplay_Hero_All;

    FGameplayTag Gameplay_Ability_Attack_Melee;
    FGameplayTag Gameplay_Ability_Attack_Ranged;
    FGameplayTag Gameplay_Ability_Death;
    FGameplayTag Gameplay_Ability_Knockback;

    FGameplayTag InputTag_Attack;
    FGameplayTag InputTag_AlternativeAttack;
    FGameplayTag InputTag_Move;
    FGameplayTag InputTag_Look_Mouse;
    FGameplayTag InputTag_Look_Stick;
    FGameplayTag InputTag_AutoRun;

    FGameplayTag Gameplay_Event_Death;
    FGameplayTag Gameplay_Event_LevelUp;
    FGameplayTag Gameplay_Event_Knockback;
    FGameplayTag Gameplay_Event_RangeHit;
    FGameplayTag Gameplay_Event_MeleeHit;
    FGameplayTag Gameplay_Event_Reset;
    FGameplayTag Gameplay_Event_RequestReset;

    FGameplayTag SetByCaller_Damage_Base;
    FGameplayTag SetByCaller_Damage_Additive;
    FGameplayTag SetByCaller_Damage_Multiplier;
    
    FGameplayTag SetByCaller_Balance_Damage;
    FGameplayTag SetByCaller_KnockbackDirection_X;
    FGameplayTag SetByCaller_KnockbackDirection_Y;
    FGameplayTag SetByCaller_KnockbackDirection_Z;

    FGameplayTag Status_Death;
};

inline const FMTD_GameplayTags &FMTD_GameplayTags::Get()
{
    return GameplayTags;
}
