using UnrealBuildTool;

public class ZzActionNodesEditor : ModuleRules
{
    public ZzActionNodesEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
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
                "ZzActionNodes",
                "ZzAction",
                "ZzActionEditor", 
                "EditorStyle", 
                "UnrealEd"
            }
        );
    }
}