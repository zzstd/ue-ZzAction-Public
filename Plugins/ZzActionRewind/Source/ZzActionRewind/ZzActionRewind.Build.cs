using UnrealBuildTool;

public class ZzActionRewind : ModuleRules
{
    public ZzActionRewind(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "ZzAction",
                "ZzRewindRuntime",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "InputCore",
            }
        );
        
        /*if (Target.Type == TargetRules.TargetType.Editor)
        {
	        PrivateDependencyModuleNames.AddRange(
		        new string[]
		        {
			        "ZzActionEditor"
		        }
	        );
        }*/
    }
}