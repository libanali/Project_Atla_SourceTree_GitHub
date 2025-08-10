// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Project_Atla : ModuleRules
{
    public Project_Atla(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Core dependencies for ALL platforms
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "InputDevice",
            "NavigationSystem",
            "UMG",
            "Slate",
            "SlateCore",
            "ApplicationCore",
            "Niagara",
            "MediaAssets",
            "Media",
            "OnlineSubsystem"  // Keep base OnlineSubsystem for all platforms
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "Engine" });

        // Steam-specific dependencies ONLY for PC platforms
        if (Target.Platform == UnrealTargetPlatform.Win64 ||
            Target.Platform == UnrealTargetPlatform.Mac ||
            Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicDependencyModuleNames.AddRange(new string[] {
                "OnlineSubsystemSteam",
                "Steamworks",
                "SteamController"
            });
        }

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}