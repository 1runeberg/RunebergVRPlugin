using UnrealBuildTool;
 
public class RunebergVRPlugin : ModuleRules
{
    public RunebergVRPlugin(TargetInfo Target)
    {
		PublicIncludePaths.AddRange(new string[] { "RunebergVRPlugin/Public" });
				
		PrivateIncludePaths.AddRange(new string[] {"RunebergVRPlugin/Private", });
		
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "SteamVR", "SteamVRController", "HeadMountedDisplay"});
 
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore"});
 
        DynamicallyLoadedModuleNames.AddRange(new string[] { "RunebergVRPlugin" });
    }
}