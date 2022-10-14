#include "AbilitySystem/MTD_AbilitySystemComponent.h"

bool UMTD_AbilitySystemComponent::SetGameplayEffectDurationHandle(
	FActiveGameplayEffectHandle Handle, float NewDuration)
{
	if (!Handle.IsValid())
		return false;

	const FActiveGameplayEffect *ConstActiveGe =
		GetActiveGameplayEffect(Handle);
	if (!ConstActiveGe)
		return false;

	FActiveGameplayEffect *ActiveGe =
		const_cast<FActiveGameplayEffect*>(ConstActiveGe);
	if (NewDuration > 0.f)
	{
		ActiveGe->Spec.Duration = NewDuration;
	}
	else
	{
		ActiveGe->Spec.Duration = 0.01f;
	}

	ActiveGe->StartServerWorldTime = ActiveGameplayEffects.GetServerWorldTime();
	ActiveGe->CachedStartServerWorldTime = ActiveGe->StartServerWorldTime;
	ActiveGe->StartWorldTime = ActiveGameplayEffects.GetWorldTime();
	ActiveGameplayEffects.MarkItemDirty(*ActiveGe);
	ActiveGameplayEffects.CheckDuration(Handle);

	ActiveGe->EventSet.OnTimeChanged.Broadcast(
		ActiveGe->Handle, ActiveGe->StartWorldTime, ActiveGe->GetDuration());
	OnGameplayEffectDurationChange(*ActiveGe);

	return true;
}

bool UMTD_AbilitySystemComponent::IncreaseGameplayEffectLevelHandle(
	FActiveGameplayEffectHandle Handle, float IncreaseBy)
{
	if (!Handle.IsValid())
		return false;

	const FActiveGameplayEffect *ConstActiveGe =
		GetActiveGameplayEffect(Handle);
	if (!ConstActiveGe)
		return false;

	FActiveGameplayEffect *ActiveGe =
		const_cast<FActiveGameplayEffect*>(ConstActiveGe);
	
	const float OldLevel = ActiveGe->Spec.GetLevel();
	const float NewLevel = OldLevel + IncreaseBy;
	
	ActiveGe->Spec.SetLevel(NewLevel);
	
	return true;
}
