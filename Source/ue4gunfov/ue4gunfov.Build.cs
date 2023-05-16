// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ue4gunfov : ModuleRules
{
	public ue4gunfov(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
