// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BatchProfiler : ModuleRules
{
	public BatchProfiler(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new []
			{
				"Core"
				, "CoreUObject"
				, "Engine"
				, "InputCore"
			}
		);
		
		if (Target.bBuildEditor == true)
		{
			PublicDependencyModuleNames.AddRange(new string[]
			{
				"Slate"
				, "SlateCore"
				, "UnrealEd"
				, "MainFrame"
			});
		}
	}
}
