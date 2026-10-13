using UnrealBuildTool;

public class SimulationWordEditor : ModuleRules
{
	public SimulationWordEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivateDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "UnrealEd", "InputCore", "SimulationWord"
		});
	}
}
