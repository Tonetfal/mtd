#include "AbilitySystem/MTD_GameplayTags.h"

#include "GameplayTagsManager.h"

// Create the singleton instance
FMTD_GameplayTags FMTD_GameplayTags::GameplayTags;

void FMTD_GameplayTags::InitializeNativeTags()
{
    GameplayTags.AddAllTags();

    // Notify manager that we are done adding native tags
    UGameplayTagsManager &Manager = UGameplayTagsManager::Get();
    Manager.DoneAddingNativeTags();
}

void FMTD_GameplayTags::AddTag(FGameplayTag &OutTag, const ANSICHAR *TagName, const ANSICHAR *TagComment)
{
    OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

bool FMTD_GameplayTags::IsForAllHeroClasses(const FGameplayTagContainer &InTags)
{
    return InTags.HasTagExact(GameplayTags.Gameplay_Hero_All);
}

void FMTD_GameplayTags::AddAllTags()
{
    AddTag(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
    AddTag(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
    AddTag(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
    AddTag(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
    AddTag(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
    AddTag(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");

    AddTag(Gameplay_Ability_Behavior_SurvivesDeath, "Gameplay.Ability.Behavior.SurvivesDeath", "An ability with this type tag should not be canceled due to death.");
    
    AddTag(Gameplay_Hero_All, "Gameplay.Hero.All", "Tag meaning that an entity can use anything, or be used by anyone.");

    AddTag(Gameplay_Ability_Attack_Melee, "Gameplay.Ability.Attack.Melee", "Melee attack ability.");
    AddTag(Gameplay_Ability_Attack_Ranged, "Gameplay.Ability.Attack.Ranged", "Ranged attack ability.");
    AddTag(Gameplay_Ability_Death, "Gameplay.Ability.Death", "Death ability.");
    AddTag(Gameplay_Ability_Knockback, "Gameplay.Ability.Knockback", "Knockback ability.");

    AddTag(InputTag_Attack, "InputTag.Attack", "Attack input.");
    AddTag(InputTag_AlternativeAttack, "InputTag.AlternativeAttack", "Alternative attack input.");
    AddTag(InputTag_Move, "InputTag.Move", "Move input.");
    AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Look (mouse) input.");
    AddTag(InputTag_Look_Stick, "InputTag.Look.Stick", "Look (stick) input.");
    AddTag(InputTag_AutoRun, "InputTag.AutoRun", "Auto-run input.");

    AddTag(Gameplay_Event_Death, "Gameplay.Event.Death", "Event that fires on death.");
    AddTag(Gameplay_Event_LevelUp, "Gameplay.Event.LevelUp", "Event that fires on level up.");
    AddTag(Gameplay_Event_Knockback, "Gameplay.Event.Knockback", "Event that fires on knockback.");
    AddTag(Gameplay_Event_RangeHit, "Gameplay.Event.RangeHit", "Event that fires on ranged weapon hit.");
    AddTag(Gameplay_Event_MeleeHit, "Gameplay.Event.MeleeHit", "Event that fires on melee weapon hit.");
    AddTag(Gameplay_Event_Reset, "Gameplay.Event.Reset", "Event that fires once a player reset is executed.");
    AddTag(Gameplay_Event_RequestReset, "Gameplay.Event.RequestReset", "Event to request a player's pawn to be instantly replaced with a new one at a valid spawn location.");

    AddTag(SetByCaller_Damage_Base, "SetByCaller.Damage.Base", "SetByCaller tag used by damage gameplay effects.");
    AddTag(SetByCaller_Damage_Additive, "SetByCaller.Damage.Additive", "SetByCaller tag used by damage gameplay effects.");
    AddTag(SetByCaller_Damage_Multiplier, "SetByCaller.Damage.Multiplier", "SetByCaller tag used by damage gameplay effects.");
        
    AddTag(SetByCaller_Balance_Damage, "SetByCaller.Balance.Damage", "SetByCaller tag used by damage gameplay effects.");
    AddTag(SetByCaller_KnockbackDirection_X, "SetByCaller.KnockbackDirection.X", "SetByCaller tag used by damage gameplay effects.");
    AddTag(SetByCaller_KnockbackDirection_Y, "SetByCaller.KnockbackDirection.Y", "SetByCaller tag used by damage gameplay effects.");
    AddTag(SetByCaller_KnockbackDirection_Z, "SetByCaller.KnockbackDirection.Z", "SetByCaller tag used by damage gameplay effects.");

    AddTag(Status_Death, "Status.Death", "Target has the death status.");
}
