// Copyright 2018 EpicLeaderboard.com, All Rights Reserved.
using UnrealBuildTool;
using System.Collections.Generic;

public class EpicLdbTemplateTarget : TargetRules
{
	public EpicLdbTemplateTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "EpicLdbTemplate" } );
	}
}
