#include "AbilitySystem/MTD_GameplayTags.h"

#include "GameplayTagsManager.h"

FMTD_GameplayTags FMTD_GameplayTags::GameplayTags;

void FMTD_GameplayTags::InitializeNativeTags()
{
    UGameplayTagsManager &Manager = UGameplayTagsManager::Get();

    GameplayTags.AddAllTags(Manager);

    // Notify manager that we are done adding native tags
    Manager.DoneAddingNativeTags();
}

void FMTD_GameplayTags::AddAllTags(UGameplayTagsManager &Manager)
{
    AddTag(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead",
        "Ability failed to activate because its owner is dead.");
    AddTag(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown",
        "Ability failed to activate because it is on cool down.");
    AddTag(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost",
        "Ability failed to activate because it did not pass the cost checks.");
    AddTag(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked",
        "Ability failed to activate because tags are blocking it.");
    AddTag(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing",
        "Ability failed to activate because tags are missing.");
    AddTag(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup",
        "Ability failed to activate because of its activation group.");

    AddTag(Gameplay_Ability_Behavior_SurvivesDeath, "Gameplay.Ability.Behavior.SurvivesDeath",
        "An ability with this type tag should not be canceled due to death.");
    
    AddTag(Gameplay_Hero_All, "Gameplay.Hero.All",
        "Tag meaning that an entity can use anything, or be used by anyone.");

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
    AddTag(Gameplay_Event_Knockback, "Gameplay.Event.Knockback", "Event that fires on knockback.");
    AddTag(Gameplay_Event_RangeHit, "Gameplay.Event.RangeHit", "Event that fires on ranged weapon hit.");
    AddTag(Gameplay_Event_MeleeHit, "Gameplay.Event.MeleeHit", "Event that fires on melee weapon hit.");
    AddTag(Gameplay_Event_Reset, "Gameplay.Event.Reset", "Event that fires once a player reset is executed.");
    AddTag(Gameplay_Event_RequestReset, "Gameplay.Event.RequestReset",
        "Event to request a player's pawn to be instantly replaced with a new one at a valid spawn location.");

    AddTag(SetByCaller_Damage_Base, "SetByCaller.Damage.Base", "SetByCaller tag used by damage gameplay effects.");
    AddTag(SetByCaller_Damage_Additive, "SetByCaller.Damage.Additive",
        "SetByCaller tag used by damage gameplay effects.");
    AddTag(SetByCaller_Damage_Multiplier, "SetByCaller.Damage.Multiplier",
        "SetByCaller tag used by damage gameplay effects.");

    AddTag(Status_Death, "Status.Death", "Target has the death status.");
    AddTag(Status_Death_Dying, "Status.Death.Dying", "Target has begun the death process.");
    AddTag(Status_Death_Dead, "Status.Death.Dead", "Target has finished the death process.");

    AddMovementModeTag(Movement_Mode_Walking, "Movement.Mode.Walking", MOVE_Walking);
    AddMovementModeTag(Movement_Mode_NavWalking, "Movement.Mode.NavWalking", MOVE_NavWalking);
    AddMovementModeTag(Movement_Mode_Falling, "Movement.Mode.Falling", MOVE_Falling);
    AddMovementModeTag(Movement_Mode_Swimming, "Movement.Mode.Swimming", MOVE_Swimming);
    AddMovementModeTag(Movement_Mode_Flying, "Movement.Mode.Flying", MOVE_Flying);
    AddMovementModeTag(Movement_Mode_Custom, "Movement.Mode.Custom", MOVE_Custom);
}

void FMTD_GameplayTags::AddTag(FGameplayTag &OutTag, const ANSICHAR *TagName, const ANSICHAR *TagComment)
{
    OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}

void FMTD_GameplayTags::AddMovementModeTag(FGameplayTag &OutTag, const ANSICHAR *TagName, uint8 MovementMode)
{
    AddTag(OutTag, TagName, "Character movement mode tag.");
    GameplayTags.MovementModeTagMap.Add(MovementMode, OutTag);
}

void FMTD_GameplayTags::AddCustomMovementModeTag(FGameplayTag &OutTag, const ANSICHAR *TagName,
    uint8 CustomMovementMode)
{
    AddTag(OutTag, TagName, "Character custom movement mode tag.");
    GameplayTags.CustomMovementModeTagMap.Add(CustomMovementMode, OutTag);
}

FGameplayTag FMTD_GameplayTags::FindTagByString(FString TagString, bool bMatchPartialString)
{
    const UGameplayTagsManager &Manager = UGameplayTagsManager::Get();
    FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

    if (!Tag.IsValid() && bMatchPartialString)
    {
        FGameplayTagContainer AllTags;
        Manager.RequestAllGameplayTags(AllTags, true);

        for (const FGameplayTag TestTag :AllTags)
        {
            if (TestTag.ToString().Contains(TagString))
            {
                MTD_LOG("Could not find exact match for tag [%s] but found partial match on tag [%s].",
                    *TagString, *TestTag.ToString());
                Tag = TestTag;
                break;
            }
        }
    }

    return Tag;
}
