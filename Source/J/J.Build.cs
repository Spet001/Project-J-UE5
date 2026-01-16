// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class J : ModuleRules
{
	public J(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { 
			"Slate", 
			"SlateCore" 
		});
		
		// Include paths para organização de pastas
		PublicIncludePaths.AddRange(new string[] {
			"J",
			"J/Core",
			"J/Characters",
			"J/Combat",
			"J/Encounters"
		});
	}
}
