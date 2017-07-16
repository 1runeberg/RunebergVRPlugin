using UnrealBuildTool;
 
public class RunebergVRPlugin : ModuleRules
{
    public RunebergVRPlugin(ReadOnlyTargetRules Target)  : base(Target)
    {
		PublicIncludePaths.AddRange(new string[] { "RunebergVRPlugin/Public" });
				
		PrivateIncludePaths.AddRange(new string[] {"RunebergVRPlugin/Private", });
		
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay"});
 
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});
 
        DynamicallyLoadedModuleNames.AddRange(new string[] { "RunebergVRPlugin" });
    }
}