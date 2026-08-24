using UnrealBuildTool;

public class ZzActionPactEditor : ModuleRules
{
    public ZzActionPactEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "ZzActionPact"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore", 
                "Projects",
                "MotionWarping",
                "UnrealEd",
                "ToolMenus",
                
                "ZzAction",
				"ZzActionEditor",
                "ZzActionNodes",
                "ZzActionPact",
				"ZzActionUtilitiesEditor", 
            }
        );
    }
}