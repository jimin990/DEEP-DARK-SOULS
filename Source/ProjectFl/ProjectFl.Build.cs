// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectFl : ModuleRules
{
	public ProjectFl(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ProjectFl",
			"ProjectFl/Variant_Platforming",
			"ProjectFl/Variant_Platforming/Animation",
			"ProjectFl/Variant_Combat",
			"ProjectFl/Variant_Combat/AI",
			"ProjectFl/Variant_Combat/Animation",
			"ProjectFl/Variant_Combat/Gameplay",
			"ProjectFl/Variant_Combat/Interfaces",
			"ProjectFl/Variant_Combat/UI",
			"ProjectFl/Variant_SideScrolling",
			"ProjectFl/Variant_SideScrolling/AI",
			"ProjectFl/Variant_SideScrolling/Gameplay",
			"ProjectFl/Variant_SideScrolling/Interfaces",
			"ProjectFl/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
