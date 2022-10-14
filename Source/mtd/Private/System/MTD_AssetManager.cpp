#include "System/MTD_AssetManager.h"

#include "AbilitySystem/MTD_GameplayTags.h"
#include "AbilitySystemGlobals.h"

UMTD_AssetManager::UMTD_AssetManager()
{
}

UMTD_AssetManager &UMTD_AssetManager::Get()
{
	check(GEngine);

	auto Singleton = Cast<UMTD_AssetManager>(GEngine->AssetManager);
	if (Singleton)
	{
		return *Singleton;
	}

	MTD_FATAL("Invalid AssetManagerClassName in DefaultEngine.ini. "
		"It must be set to MTD_AssetManager!");

	// Fatal error above prevents this from being called
	return *NewObject<UMTD_AssetManager>();
}

void UMTD_AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	InitializeAbilitySystem();
}

void UMTD_AssetManager::InitializeAbilitySystem()
{
	FMTD_GameplayTags::InitializeNativeTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
