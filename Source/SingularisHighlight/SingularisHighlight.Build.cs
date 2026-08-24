using UnrealBuildTool;

public class SingularisHighlight : ModuleRules
{
	public SingularisHighlight(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(
			[
				"Core",
				"CoreUObject",
				"Engine"
			]
		);
	}
}