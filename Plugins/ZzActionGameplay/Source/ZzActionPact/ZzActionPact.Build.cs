using UnrealBuildTool;

public class ZzActionPact : ModuleRules
{
    public ZzActionPact(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "ZzAction", 
                "ZzActionNodes",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                
                "ZzAction",
                "MotionWarping",
                "ZzActionUtilities",
            }
        );
    }
}