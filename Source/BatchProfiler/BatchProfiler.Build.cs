// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BatchProfiler : ModuleRules
{
	public BatchProfiler(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateIncludePaths.AddRange(
			new [] 
			{
				System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/EditorStyle/Public"
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"Core",
				"Blutility",
				"EditorScriptingUtilities",
				"UMG",
				"Niagara",
				"UnrealEd",
				"AssetRegistry",
				"AssetTools",
				"ContentBrowser",
				"InputCore",
				"Slate",
				"SlateCore",
				"Projects"
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new []
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore", 
				"EditorStyle"
				// ... add private dependencies that you statically link with here ...	
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
