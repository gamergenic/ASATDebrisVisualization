/* Copyright (C) 2022 Chuck Noble <chuck@gamergenic.com>
 * This work is free.  You can redistribute it and /or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar.  See http://www.wtfpl.net/ for more details.
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details.
 */


using UnrealBuildTool;
using System.Collections.Generic;

public class DebrisCloudEditorTarget : TargetRules
{
	public DebrisCloudEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "DebrisCloud" } );

        ExtraModuleNames.AddRange(new string[] { "Spice", "SpiceEditor", "SpiceUncooked" });

        bUseFastPDBLinking = false;
        bPublicSymbolsByDefault = true;  // <- Forced to true on Windows anyways
        WindowsPlatform.bStripUnreferencedSymbols = false;

        DebrisCloudTarget.BuildCSpiceLib(this);
    }
}
