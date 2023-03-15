using UnrealBuildTool;

public class mtd : ModuleRules
{
	public mtd(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"AIModule",
			"Core", 
			"CoreUObject", 
			"DeveloperSettings",
			"Engine", 
			"InputCore",
			"NavigationSystem",
			"NetCore",
            
			"GameplayTasks",
			"GameplayAbilities", 
			"GameplayTags",
            
			"ModularGameplay",
            
            "CommonUI",
            "CommonUIEditor",
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
        {  
            "EnhancedInput",
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
