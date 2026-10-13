// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SimulationWord : ModuleRules
{
	public SimulationWord(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"AIModule", 
			"UMG", 
			"Slate", 
			"SlateCore", 
			"GameplayTags",
			"DeveloperSettings"
		});
	}
}
