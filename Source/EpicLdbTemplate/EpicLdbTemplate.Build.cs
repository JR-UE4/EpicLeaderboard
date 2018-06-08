// Copyright 2018 EpicLeaderboard.com, All Rights Reserved.
using UnrealBuildTool;

public class EpicLdbTemplate : ModuleRules
{
	public EpicLdbTemplate(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });
		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
