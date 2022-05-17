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
using EpicGames.Core;
using System.Collections.Generic;
using System.IO;

public class DebrisCloudTarget : TargetRules
{
    // To build, you need to drop a copy of MaxQ here:
    // Source/MaxQ
    //
    // MaxQ GitHub
    // https://github.com/Gamergenic1/MaxQ

    public const string RelativePathToCSpiceToolkit = "Source\\MaxQ\\CSpice_Library\\cspice\\";
    public const string RelativePathToCSpiceLib = "Source\\MaxQ\\CSpice_Library\\lib\\Win64\\cspice.lib"; 
    
    public DebrisCloudTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "DebrisCloud" } );

        ExtraModuleNames.AddRange(new string[] { "Spice" });

        BuildCSpiceLib(this);
    }

    static public void BuildCSpiceLib(TargetRules targetRules)
    {
        string pathToCSpiceLib = CSpice_Library.CSpiceLibPath(new ReadOnlyTargetRules(targetRules));

        if (!File.Exists(pathToCSpiceLib))
        {
            // Note :  If the step fails, since it's a prebuild step, these rules will not be rebuilt.
            // So, don't cause a failure here, if you're iterating on these rules.
            // Also, changes to the invocation won't be seen until the following build!
            System.Console.WriteLine("Rebuilding cspice toolkit lib");
            targetRules.PreBuildSteps.Add("$(ProjectDir)\\" + RelativePathToCSpiceToolkit + "makeall_ue.bat \"$(ProjectDir)\\" + RelativePathToCSpiceToolkit + "\"");
        }
        else
        {
            System.Console.WriteLine("cspice toolkit lib is up to date");
        }
    }
}
