// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Project_Atla : ModuleRules
{
	public Project_Atla(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "InputDevice", "NavigationSystem", "UMG", "Slate", "SlateCore", "ApplicationCore", "Niagara", "MediaAssets", "Media", "OnlineSubsystem", "OnlineSubsystemSteam", "Steamworks"});

		PrivateDependencyModuleNames.AddRange(new string[] { "Engine" });

        PublicDependencyModuleNames.AddRange(new string[] { "SteamController" });


        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
