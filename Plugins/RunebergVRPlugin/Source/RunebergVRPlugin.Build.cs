using UnrealBuildTool;
 
public class RunebergVRPlugin : ModuleRules
{
    public RunebergVRPlugin(ReadOnlyTargetRules Target)  : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "RunebergVRPlugin/Source/Public" });
				
		PrivateIncludePaths.AddRange(new string[] {"RunebergVRPlugin/Source/Private"});
		
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem"});
 
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});
 
        DynamicallyLoadedModuleNames.AddRange(new string[] { "RunebergVRPlugin" });
    }
}