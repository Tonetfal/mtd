using UnrealBuildTool;

public class mtd : ModuleRules
{
	public mtd(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"NavigationSystem",
			"AIModule",
			"GameplayTasks",
			"GameplayAbilities", 
			"GameplayTags",
			"ModularGameplay",
			// "ModularGameplayActors",
			"NetCore",
			"EnhancedInput",
            "CommonUI",
            "CommonUIEditor",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
