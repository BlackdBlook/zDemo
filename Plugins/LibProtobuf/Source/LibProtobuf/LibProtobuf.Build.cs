// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class LibProtobuf : ModuleRules
{
	public LibProtobuf(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			Console.WriteLine(Path.Combine(ModuleDirectory, "lib","libprotobuf.lib"));
			PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
			PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "lib"));
			PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib","libprotobuf.lib"));
		}


		ShadowVariableWarningLevel = WarningLevel.Off;
		bEnableExceptions = true;
	}
}
