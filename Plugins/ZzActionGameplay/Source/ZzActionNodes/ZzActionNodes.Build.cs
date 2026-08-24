using UnrealBuildTool;

public class ZzActionNodes : ModuleRules
{
    public ZzActionNodes(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "ZzAction", "Niagara", 
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "MotionWarping",
                "Niagara", 
                "ZzActionUtilities"
            }
        );
    }
}