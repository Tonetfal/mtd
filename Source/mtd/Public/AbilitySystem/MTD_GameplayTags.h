#pragma once

#include "GameplayTagContainer.h"
#include "mtd.h"

class UGameplayTagsManager;

/**
 *
 *	Singleton containing native gameplay tags.
 */
struct FMTD_GameplayTags
{
public:
	static const FMTD_GameplayTags &Get()
		{ return GameplayTags; }

	static void InitializeNativeTags();

	static FGameplayTag FindTagByString(
		FString TagString, bool bMatchPartialString = false);

public:
	FGameplayTag Ability_ActivateFail_IsDead;
	FGameplayTag Ability_ActivateFail_Cooldown;
	FGameplayTag Ability_ActivateFail_Cost;
	FGameplayTag Ability_ActivateFail_TagsBlocked;
	FGameplayTag Ability_ActivateFail_TagsMissing;
	FGameplayTag Ability_ActivateFail_ActivationGroup;

	FGameplayTag Gameplay_Ability_Behavior_SurvivesDeath;
	
	FGameplayTag Gameplay_Ability_Attack_Melee;
	FGameplayTag Gameplay_Ability_Attack_Ranged;
	FGameplayTag Gameplay_Ability_Death;

	FGameplayTag InputTag_Attack;
	FGameplayTag InputTag_AlternativeAttack;
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_Look_Stick;
	FGameplayTag InputTag_AutoRun;

	FGameplayTag Gameplay_Event_Death;
	FGameplayTag Gameplay_Event_Reset;
	FGameplayTag Gameplay_Event_RequestReset;

	FGameplayTag SetByCaller_Damage_Base;
	FGameplayTag SetByCaller_Damage_Additive;
	FGameplayTag SetByCaller_Damage_Multiplier;

	FGameplayTag Status_Death;
	FGameplayTag Status_Death_Dying;
	FGameplayTag Status_Death_Dead;

	FGameplayTag Movement_Mode_Walking;
	FGameplayTag Movement_Mode_NavWalking;
	FGameplayTag Movement_Mode_Falling;
	FGameplayTag Movement_Mode_Swimming;
	FGameplayTag Movement_Mode_Flying;
	FGameplayTag Movement_Mode_Custom;

	TMap<uint8, FGameplayTag> MovementModeTagMap;
	TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

public:
	static void AddTag(FGameplayTag &OutTag, const ANSICHAR *TagName,
		const ANSICHAR *TagComment);

protected:
	void AddAllTags(UGameplayTagsManager &Manager);
	void AddMovementModeTag(
		FGameplayTag &OutTag, const ANSICHAR *TagName, uint8 MovementMode);
	void AddCustomMovementModeTag(FGameplayTag &OutTag, const ANSICHAR *TagName,
		uint8 CustomMovementMode);

private:
	static FMTD_GameplayTags GameplayTags;
};
