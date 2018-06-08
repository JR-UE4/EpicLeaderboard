// Copyright 2018 EpicLeaderboard.com, All Rights Reserved.
using UnrealBuildTool;
using System.Collections.Generic;

public class EpicLdbTemplateEditorTarget : TargetRules
{
	public EpicLdbTemplateEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "EpicLdbTemplate" } );
	}
}
