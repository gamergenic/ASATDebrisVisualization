# ASATDebrisVisualization
Example UnrealEngine5 space-track.com REST api http client.

Tutorial Link:  
[Call a SatCat REST API from Unreal Engine](https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/)


### Example application on GitHub

If you simply clone the repository and attempt to build it, you'll get a build error (By Design).

```
C:\git\gamedevtricks\ASATDebrisVisualization\DebrisCloud\Source\DebrisCloud.Target.cs(43,34): error CS0103: The name 'CSpice_Library' does not exist in the current context
ERROR: Expecting to find a type to be declared in a target rules named 'DebrisCloudTarget'.  This type must derive from the 'TargetRules' type defined by Unreal Build Tool.
```

This happens because MaxQ is not committed to the repository.  One easy solution is to clone the MaxQ repository separately, and then add a symbolic link that maps the appropriate source subdirectory to the example application.

[MaxQ GitHub page](https://github.com/Gamergenic1/MaxQ/)  

Command prompt commands that create the appropriate directory, clone the required projects, and create the necessary symbolic link (on Windows):

```
mkdir c:\git\gamedevtricks\
cd /D c:\git\gamedevtricks\

rem - clone example project from github
git clone https://github.com/gamergenic/ASATDebrisVisualization.git

rem - clone the MaxQ dependency
git clone https://github.com/Gamergenic1/MaxQ.git

rem - clean out the MaxQ directory so it can be replaced with a symbolic link
rd /S /Q ASATDebrisVisualization\DebrisCloud\Source\MaxQ

rem - create a directory junction in the sample project for MaxQ
mklink /J ASATDebrisVisualization\DebrisCloud\Source\MaxQ MaxQ\Source\MaxQ

Junction created for ASATDebrisVisualization\DebrisCloud\Source\MaxQ <<===>> MaxQ\Source\MaxQ
```

#### Generate Visual Studio project files

Once you have the complete set of source code in place, generate the visual studio project files:

<img alt="Use the Explorer context menu to generate Visual Studio project files from the .uproject" src="https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/generate-project-files.jpg" />

Now you can build the project's Unreal Engine Editor from the VS .sln.

#### Build the Unreal Engine Editor

Hit Control+Shift+B to build the editor, and you should have a successful build.

 ```
 1>Building DebrisCloudEditor...
...
1>Total execution time: 50.60 seconds
========== Build: 1 succeeded, 0 failed, 0 up-to-date, 1 skipped ==========
```

You can now launch the Project's Unreal Engine editor via the F5 key, which starts a debug session with the Editor Application.

#### Running the app with test data

Once the Editor is launched you can click the 'Play' button, and see what happens.

If all goes well, the Unreal Engine Output window will indicate an http request was sent, and a response received:
```
LogTemp: Space-Track request: https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/fengyun-1c-remaining.json; content:TEST_CONTENT
LogTemp: Space-Track request: https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/cosmos-1408-remaining.json; content:TEST_CONTENT
LogTemp: Space-Track response: [
    {
        "CCSDS_OMM_VERS": "2.0",
        "COMMENT": "

```

And, you should be treated to a 3D display of earth and the debris fields from two Anti-Satellite weapons tests.

[2007 Chinese anti-satellite missile test: Fengyun 1C](https://en.wikipedia.org/wiki/2007_Chinese_anti-satellite_missile_test)  
[2021 Russian anti-satellite missile test: Kosmos 1408](https://en.wikipedia.org/wiki/Kosmos_1408)  

The visualization you're viewing, though, is utilizing "stale" test data as opposed to live data from a SatCat.

#### Space-Track.org

Live data is available from Space-Track.org, a SatCat.  Note, however, Space-Track.org is a service developed to serve satellite operators, who make use of its telemetry to avoid collisions with debris, etc.  It's not for us nosey tinkering gamedevs, so tread lightly.

> Space-Track.org is a site that shares space situational awareness services with satellite operators, academia, and other interested entities.

The user agreement is here:  
https://www.space-track.org/documentation#user_agree


> **User Agreement**  
> Please read the following terms and conditions of the User Agreement carefully! This website permits access to U.S. Government space situational awareness information to approved users only. To obtain access, all users must abide by the following terms and conditions:  
> The User agrees not to transfer any data or technical information received from this website, or other U.S. Government source, including the analysis of data, to any other entity without prior express approval. See, 10 USC 2274(c)(2).  
  ...

> With great power comes great responsibility...
The API Query Builder tool allows users a great amount of power and flexibility in creating queries. Your space-track account may be suspended if you violate the usage policy by querying data too often or by running queries that negatively impact the performance of the website. Repeat offenders may have their account suspended permanently.

---

**info**  
Using the service as a target for a non-trivial volume of end users is against the User Agreement and will get you in trouble with Uncle Sam.  To that end, I've provided two sample files you can test your code against:  
```https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/fengyun-1c-remaining.json```  
```https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/cosmos-1408-remaining.json```  
These are the test files the GitHub app uses by default.

---

The JSON response data format you'll receive from Space-Track.org.

```JSON
[
    {
        "CCSDS_OMM_VERS": "2.0",
        "COMMENT": "GENERATED VIA SPACE-TRACK.ORG API",
        "CREATION_DATE": "2004-08-16T23:24:14",
        "ORIGINATOR": "18 SPCS",
        "OBJECT_NAME": "USA 19",
        "OBJECT_ID": "1986-069A",
        "CENTER_NAME": "EARTH",
        "REF_FRAME": "TEME",
        "TIME_SYSTEM": "UTC",
        "MEAN_ELEMENT_THEORY": "SGP4",
        "EPOCH": "1986-09-25T09:25:10.253855",
        "MEAN_MOTION": "16.08291950",
        "ECCENTRICITY": "0.01118730",
        "INCLINATION": "39.0531",
        "RA_OF_ASC_NODE": "276.4305",
        "ARG_OF_PERICENTER": "171.4053",
        "MEAN_ANOMALY": "189.5525",
        "EPHEMERIS_TYPE": "0",
        "CLASSIFICATION_TYPE": "U",
        "NORAD_CAT_ID": "16937",
        "ELEMENT_SET_NO": "999",
        "REV_AT_EPOCH": "306",
        "BSTAR": "0.00269490000000",
        "MEAN_MOTION_DOT": "0.06329737",
        "MEAN_MOTION_DDOT": "0.0000267130000",
        "SEMIMAJOR_AXIS": "6629.674",
        "PERIOD": "89.535",
        "APOAPSIS": "325.707",
        "PERIAPSIS": "177.371",
        "OBJECT_TYPE": "PAYLOAD",
        "RCS_SIZE": null,
        "COUNTRY_CODE": null,
        "LAUNCH_DATE": null,
        "SITE": null,
        "DECAY_DATE": "1986-09-28",
        "FILE": "34511",
        "GP_ID": "15040479",
        "TLE_LINE0": "0 USA 19",
        "TLE_LINE1": "1 16937U 86069A   86268.39247979  .06329737 +26713-4 +26949-2 0  9997",
        "TLE_LINE2": "2 16937 039.0531 276.4305 0111873 171.4053 189.5525 16.08291950003063"
    }
]
```

#### Connecting to Space-Track.org for live data

To actually see live data and query the actual Space-Track.org servers, you'll need to make a couple of changes within the Unreal Engine Editor.


You'll need to edit the map/level.  Specifically, edit BOTH debris field actors (BP_DebrisActor-*):
* Uncheck "Use Test URL"
* Set "Space Track User" and "Space Track Password" to your login credentials at Space-Track.com

<img alt="Add your username/password to BP_DebrisActor, uncheck 'User Test Url'" src="https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/debris-actor-details.png" />

Run the app again and check the Editor's Output Log.  You will see that you're now querying and visualizing up-to-the-minute data from Space-Track.org.

##### One more time: don't spam Space-Track.org! 

Space-Track.org asks API users to be responsible and adhere usage caps.  If you were actually building a visualization app for many users, you'd need to query the Space-Track.org server from your own server and cache the data.  If you cache the data your servers can then handle as many users as needed without adding additional load to the Space-Track.org servers.  Be careful not to violate the terms of use, of course, which prohibit arbitrarily redistributing data from the SatCat.




[![p-to-the-minute 3D orbital map, showing debris from two anti-satellite tests, obtained via http request to space-track.org](https://gamedevtricks.com/post/call-satcat-rest-api-from-ue-http/ue-debris-map.jpg)](https://youtu.be/LL31I2sQfR4)