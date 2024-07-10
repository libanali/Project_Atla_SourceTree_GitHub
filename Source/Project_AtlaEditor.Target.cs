// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class Project_AtlaEditorTarget : TargetRules
{
	public Project_AtlaEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "Project_Atla", "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem" });


         
    }
}
