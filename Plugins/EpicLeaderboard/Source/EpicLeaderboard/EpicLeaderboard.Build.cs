// Copyright 2018 EpicLeaderboard.com, All Rights Reserved.
namespace UnrealBuildTool.Rules
{
	public class EpicLeaderboard : ModuleRules
	{
		public EpicLeaderboard(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PrivateIncludePaths.AddRange(new string[] {"EpicLeaderboard/Private", "EpicLeaderboard/Public" });
			PublicDependencyModuleNames.AddRange(new string[]{"Core","CoreUObject","Http","Json","JsonUtilities","Engine"});
		}
	}
}