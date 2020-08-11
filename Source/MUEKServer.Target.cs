// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MUEKServerTarget : TargetRules
{
	public MUEKServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
        bUsesSteam = false;
		ExtraModuleNames.Add("MUEK");
	}
}
