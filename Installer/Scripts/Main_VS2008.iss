; Input defines: ConfigurationName (Debug or Release), PlatformName (Win32, x64, etc.)

; Change from "Yes" to "No" on the next line if you don't compile an alpha build
#define alpha "Yes"

#if VER < 0x05010700
  #error Inno Setup version 5.1.7 or higher is needed for this script
#endif
#if PREPROCVER < 0x05020000
  #error PreProcessor version 5.2.0.0 or higher is needed for this script
#endif

#define internal_name GetStringFileInfo("..\PeerProject\" + ConfigurationName + " " + PlatformName + "\PeerProject.exe", INTERNAL_NAME);
#define name          internal_name
#define build         PlatformName + " " + ConfigurationName + " Build"
#define version       GetFileVersion("..\PeerProject\" + ConfigurationName + " " + PlatformName + "\PeerProject.exe")
#define Publisher     "PeerProject Development Team"
#define Description   internal_name + " Filesharing System"
#define date          GetDateTimeString('yyyy/mm/dd', '-', '')

#if ConfigurationName == "Debug"
  #define output_name	internal_name + "_" + version + "_" + date + "_" + ConfigurationName + "_" + PlatformName
#elif alpha == "Yes"
  #if PlatformName == "x64"
    #define output_name	internal_name + "_" + version + "_" + date + "_" + PlatformName
  #else
    #define output_name	internal_name + "_" + version + "_" + date
  #endif
#elif PlatformName == "x64"
  #define output_name	internal_name + "_" + version + "_" + PlatformName
#else
  #define output_name	internal_name + "_" + version
#endif

[Setup]
AppComments={#Description}
AppId={#internal_name}
AppName={#name}
AppVersion={#version}
AppVerName={#internal_name} {#build} {#version}
AppMutex={#internal_name},Global\TorrentWizard
DefaultDirName={ini:{param:SETTINGS|},Locations,Path|{reg:HKLM\SOFTWARE\{#internal_name},|{pf}\{#internal_name}}}
DirExistsWarning=no
DefaultGroupName={#internal_name}
#if VER > 0x05030200
  DisableProgramGroupPage=auto
#endif
AllowNoIcons=yes
OutputDir=VS2008
OutputBaseFilename={#output_name}
SolidCompression=yes
Compression=lzma/max
InternalCompressLevel=max
PrivilegesRequired=poweruser
ShowLanguageDialog=yes
ShowUndisplayableLanguages=yes
LanguageDetectionMethod=locale
AppModifyPath="{app}\Uninstall\Setup.exe"
UninstallDisplayIcon={app}\Uninstall\Setup.exe
UninstallDisplayName={#internal_name} {#version}
UninstallFilesDir={app}\Uninstall
SetupIconFile=Installer\Res\Install.ico
WizardImageFile=Installer\Res\Sidebar.bmp
WizardSmallImageFile=Installer\Res\CornerLogo.bmp
ShowComponentSizes=no
ChangesAssociations=yes
ChangesEnvironment=yes
OutputManifestFile=Manifest_{#ConfigurationName}{#PlatformName}.txt
MinVersion=0,5.0
#if PlatformName == "x64"
  ArchitecturesAllowed=x64
  ArchitecturesInstallIn64BitMode=x64
#endif

; Set the SVN root as source dir (up 2 levels)
SourceDir=..\..

VersionInfoVersion={#version}
VersionInfoDescription={#Description}
AppPublisher={#Publisher}
AppCopyright=Copyright © {#Publisher}. All rights reserved.

; Links to website for software panel
AppPublisherURL=http://PeerProject.org
AppSupportURL=http://support.peerproject.org/
AppUpdatesURL=http://download.peerproject.org/

[Tasks]
Name: "quicklaunch"; Description: "{cm:CreateQuickLaunchIcon}"; OnlyBelowVersion: 6.1,6.01;
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}";
Name: "desktopiconwizard"; Description: "{cm:CreateDesktopIconWizard}"; Languages: en en_uk;
Name: "language"; Description: "{cm:tasks_languages}";
Name: "multiuser"; Description: "{cm:tasks_multisetup}";
Name: "upnp"; Description: "{cm:tasks_upnp}"; MinVersion: 0,5.01; Check: CanUserModifyServices;
;Name: "firewall"; Description: "{cm:tasks_firewall}"; MinVersion: 0,5.01sp2;
Name: "resetdiscoveryhostcache"; Description: "{cm:tasks_resetdiscoveryhostcache}"; Check: EnableDeleteOldSetup; Flags: unchecked; 
#if alpha == "No"
Name: "deleteoldsetup"; Description: "{cm:tasks_deleteoldsetup}"; Check: EnableDeleteOldSetup;
#endif

[Files]
; Main files
Source: "PeerProject\{#ConfigurationName} {#PlatformName}\PeerProject.exe"; 	DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
Source: "TorrentWizard\{#ConfigurationName} {#PlatformName}\TorrentWizard.exe";	DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
Source: "SkinInstaller\{#ConfigurationName} {#PlatformName}\SkinInstaller.exe";	DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
;Source: "SkinBuilder\{#ConfigurationName} {#PlatformName}\SkinBuilder.exe"; 	DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension

; Save/Restore scripts
Source: "Services\SaveSettings.bat"; DestDir: "{app}"; DestName: "SaveSettings.bat"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension skipifsourcedoesntexist
Source: "Services\RestoreSettings.bat"; DestDir: "{app}"; DestName: "RestoreSettings.bat"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension skipifsourcedoesntexist

; ZLib
;Source: "Services\zlib\{#ConfigurationName} {#PlatformName}\zlibwapi.dll"; DestDir: "{app}"; DestName: "zlibwapi.dll"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
;Source: "Services\zlib\{#ConfigurationName} {#PlatformName}\zlibwapi.dll"; DestDir: "{app}\Plugins"; DestName: "zlibwapi.dll"; Flags: overwritereadonly replacesameversion uninsremovereadonly sortfilesbyextension deleteafterinstall
#if PlatformName == "Win32"
Source: "Services\zlibwapi.dll"; DestDir: "{app}";         DestName: "zlibwapi.dll"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
Source: "Services\zlibwapi.dll"; DestDir: "{app}\Plugins"; DestName: "zlibwapi.dll"; Flags: overwritereadonly replacesameversion uninsremovereadonly sortfilesbyextension deleteafterinstall
#elif PlatformName == "x64"
Source: "Services\zlibwapi-x64.dll"; DestDir: "{app}";         DestName: "zlibwapi.dll"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
Source: "Services\zlibwapi-x64.dll"; DestDir: "{app}\Plugins"; DestName: "zlibwapi.dll"; Flags: overwritereadonly replacesameversion uninsremovereadonly sortfilesbyextension deleteafterinstall
#endif

; HashLib
Source: "HashLib\{#ConfigurationName} {#PlatformName}\HashLib.dll"; DestDir: "{app}"; DestName: "HashLib.dll"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension

; SQLite
Source: "Services\SQLite\{#ConfigurationName} {#PlatformName}\SQLite.dll"; DestDir: "{app}"; DestName: "SQLite.dll"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension

; GeoIP
Source: "Services\GeoIP\{#ConfigurationName} {#PlatformName}\GeoIP.dll"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
;Source: "Data\GeoIP.dat";  DestDir: "{app}\Data"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension

; Plugins
Source: "Plugins\DocumentReader\{#ConfigurationName} {#PlatformName}\DocumentReader.dll";   DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

Source: "Plugins\ImageViewer\{#ConfigurationName} {#PlatformName}\ImageViewer.dll";   DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

Source: "Services\LibGFL\{#PlatformName}\LibGFL290.dll"; DestDir: "{app}\Plugins";  Flags: overwritereadonly replacesameversion uninsremovereadonly sortfilesbyextension deleteafterinstall
Source: "Services\LibGFL\{#PlatformName}\LibGFL290.dll"; DestDir: "{app}";  Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
Source: "Plugins\GFLImageServices\{#ConfigurationName} {#PlatformName}\GFLImageServices.dll";  DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
Source: "Plugins\GFLLibraryBuilder\{#ConfigurationName} {#PlatformName}\GFLLibraryBuilder.dll";  DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension

Source: "Plugins\MediaImageServices\{#ConfigurationName} {#PlatformName}\MediaImageServices.dll";   DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver
Source: "Plugins\MediaLibraryBuilder\{#ConfigurationName} {#PlatformName}\MediaLibraryBuilder.dll";   DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

Source: "Plugins\RARBuilder\{#ConfigurationName} {#PlatformName}\RARBuilder.dll";   DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver
#if PlatformName == "Win32"
Source: "Plugins\RARBuilder\Unrar.dll"; DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
;Source: "Plugins\RARBuilder\Unrar.dll"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
#elif PlatformName == "x64"
Source: "Plugins\RARBuilder\Unrar64.dll"; DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
;Source: "Plugins\RARBuilder\Unrar64.dll"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
#endif

Source: "Plugins\7ZipBuilder\{#ConfigurationName} {#PlatformName}\7ZipBuilder.dll"; DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver
Source: "Plugins\7ZipBuilder\{#ConfigurationName} {#PlatformName}\7zxr.dll";  DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion uninsremovereadonly sortfilesbyextension
;Source: "Plugins\7ZipBuilder\{#ConfigurationName} {#PlatformName}\7zxr.dll"; DestDir: "{app}";         Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension deleteafterinstall

Source: "Plugins\ZIPBuilder\{#ConfigurationName} {#PlatformName}\ZIPBuilder.dll";  DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

Source: "Plugins\RatDVDPlugin\{#ConfigurationName} {#PlatformName}\RatDVDReader.dll";   DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

Source: "Plugins\SkinScan\{#ConfigurationName} {#PlatformName}\SkinScan.dll";  DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

Source: "Plugins\SWFPlugin\{#ConfigurationName} {#PlatformName}\SWFPlugin.dll";  DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

Source: "Plugins\VirusTotal\{#ConfigurationName} {#PlatformName}\VirusTotal.dll";  DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

Source: "Plugins\SearchExport\{#ConfigurationName} {#PlatformName}\SearchExport.dll"; DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

;Source: "Plugins\WindowsThumbnail\{#ConfigurationName} {#PlatformName}\WindowsThumbnail.exe";  DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension

Source: "Plugins\MediaPlayer\{#ConfigurationName} {#PlatformName}\MediaPlayer.dll"; DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension regserver

; Don't register WebHook.dll since it will setup PeerProject as download manager  (ToDo: fix WebHook.dll regserver!)
Source: "Plugins\WebHook\{#ConfigurationName} {#PlatformName}\WebHook*.dll"; DestDir: "{app}\Plugins"; Flags: noregerror overwritereadonly replacesameversion restartreplace uninsrestartdelete uninsremovereadonly sortfilesbyextension
;Source: "Plugins\WebHook.dll"; DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension


;--== Debug Databases ==--
#if ConfigurationName == "Debug"

Source: "PeerProject\{#ConfigurationName} {#PlatformName}\PeerProject.pdb"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
; ** This section can be uncommented to include the debug database files for all the plugins
;Source: "Plugins\*.pdb"; DestDir: "{app}\Plugins"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
; ** The next line can be uncommented to include geoip, skin & zlibwapi debug database files
;Source: "Services\*.pdb"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension

#if PlatformName == "x64"
Source: "Services\BugTrap\BugTrapU-x64.dll"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
Source: "Services\BugTrap\dbghelp-x64.dll"; DestDir: "{sys}"; DestName: "dbghelp.dll"; Flags: overwritereadonly restartreplace uninsneveruninstall sortfilesbyextension
#else
Source: "Services\BugTrap\BugTrapU.dll"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
Source: "Services\BugTrap\dbghelp.dll"; DestDir: "{sys}"; DestName: "dbghelp.dll"; Flags: overwritereadonly restartreplace uninsneveruninstall sortfilesbyextension
#endif

#endif


;--== Include Files ==--
; Main Data Files
Source: "Data\*.*"; DestDir: "{app}\Data"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn,*.bak,*.url,GPL*"

; Icons
Source: "Installer\Res\Uninstall.ico"; DestDir: "{app}\Uninstall"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension

; Schemas
Source: "Schemas\*"; DestDir: "{app}\Schemas"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn,*.bak"

; Skins
Source: "Skins\*"; DestDir: "{app}\Skins"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension recursesubdirs; Excludes: ".svn,*.bak"

; Templates
Source: "Templates\*"; DestDir: "{app}\Templates"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension recursesubdirs; Excludes: ".svn,*.bak"

; Languages
;Source: "Languages\*"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Tasks: "language"; Excludes: ".svn,*.bak,default-en.*"

; Visualizations
;Source: "Plugins\MediaVis\*"; DestDir: "{app}\Vis"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension recursesubdirs; Excludes: ".svn"

; Tools
;Source: "Repository\Bittorrent Tempfile Chopper.exe"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension
;Source: "Repository\Pilo Torrent Tempfile Splitter.exe"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension

; Licenses
; Source: "Installer\License\LICENSE-GeoIP.txt"; DestDir: "{app}"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension
Source: "Installer\License\License (GPL 3.0).html"; DestDir: "{app}"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension

;--== Copy files ==--
; Copy skins back from {userappdata}\PeerProject\Skins
Source: "{userappdata}\PeerProject\Skins\*"; DestDir: "{app}\Skins"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist recursesubdirs; AfterInstall: DeleteFolder('{userappdata}\PeerProject\Skins')

; Copy templates back from {userappdata}\PeerProject\Templates
Source: "{userappdata}\PeerProject\Templates\*"; DestDir: "{app}\Templates"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist recursesubdirs; AfterInstall: DeleteFolder('{userappdata}\PeerProject\Templates')

; Switch user data between locations
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Library1.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Library2.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\DownloadGroups.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\TigerTree.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Security.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\UploadQueues.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Searches.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Schedule.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Profile.xml"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Library1.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Library2.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\DownloadGroups.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\TigerTree.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Security.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\UploadQueues.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Searches.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Schedule.dat"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Profile.xml"; DestDir: "{userappdata}\PeerProject\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: multiuser

Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Library1.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Library2.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\DownloadGroups.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\TigerTree.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Security.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\UploadQueues.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Searches.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Schedule.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data\Profile.xml"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Library1.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Library2.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\DownloadGroups.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\TigerTree.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Security.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\UploadQueues.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Searches.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Schedule.dat"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser
Source: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data\Profile.xml"; DestDir: "{app}\Data"; Flags: ignoreversion uninsremovereadonly sortfilesbyextension external onlyifdoesntexist skipifsourcedoesntexist; Tasks: not multiuser

; Copy installer into download and uninstall dir
#if alpha == "No"
Source: "{srcexe}"; DestDir: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}}"; DestName: "{#output_name}.exe"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension external onlyifdoesntexist; Tasks: multiuser
Source: "{srcexe}"; DestDir: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}}"; DestName: "{#output_name}.exe"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension external onlyifdoesntexist; Tasks: not multiuser
#endif
Source: "{srcexe}"; DestDir: "{app}\Uninstall"; DestName: "setup.exe"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension external

; Copy repair installer
;Source: "Installer\Builds\repair.exe"; DestDir: "{app}\Uninstall"; Flags: overwritereadonly replacesameversion uninsremovereadonly sortfilesbyextension; Check: not WizardSilent

; Copy default security rules
Source: "Data\DefaultSecurity.dat"; DestDir: "{userappdata}\PeerProject\Data"; DestName: "Security.dat"; Flags: onlyifdoesntexist uninsremovereadonly sortfilesbyextension; Tasks: multiuser
Source: "Data\DefaultSecurity.dat"; DestDir: "{app}\Data"; DestName: "Security.dat"; Flags: onlyifdoesntexist uninsremovereadonly sortfilesbyextension; Tasks: not multiuser

[Icons]
; PeerProject Start Menu Shortcuts
Name: "{group}\{#internal_name}"; Filename: "{app}\PeerProject.exe"; WorkingDir: "{app}"; Comment: "{cm:reg_apptitle}"
Name: "{group}\TorrentWizard"; Filename: "{app}\TorrentWizard.exe"; WorkingDir: "{app}"; Comment: "PeerProject Torrent File Creator"
Name: "{group}\GUI Modes\{#internal_name} ({cm:icons_basicmode})"; Filename: "{app}\PeerProject.exe"; Parameters: "-basic"; WorkingDir: "{app}"; Comment: "{cm:reg_apptitle}"
Name: "{group}\GUI Modes\{#internal_name} ({cm:icons_tabbedmode})"; Filename: "{app}\PeerProject.exe"; Parameters: "-tabbed"; WorkingDir: "{app}"; Comment: "{cm:reg_apptitle}"
Name: "{group}\GUI Modes\{#internal_name} ({cm:icons_windowedmode})"; Filename: "{app}\PeerProject.exe"; Parameters: "-windowed"; WorkingDir: "{app}"; Comment: "{cm:reg_apptitle}"
Name: "{group}\GUI Modes\{#internal_name} ({cm:icons_launchtray})"; Filename: "{app}\PeerProject.exe"; Parameters: "-tray"; WorkingDir: "{app}"; Comment: "{cm:reg_apptitle}"
Name: "{commondesktop}\{#internal_name}"; Filename: "{app}\PeerProject.exe"; WorkingDir: "{app}"; Comment: "{cm:reg_apptitle}"; Tasks: desktopicon
Name: "{commondesktop}\TorrentWizard"; Filename: "{app}\TorrentWizard.exe"; WorkingDir: "{app}"; Comment: "{cm:reg_apptitle}"; Tasks: desktopiconwizard
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#internal_name}"; Filename: "{app}\PeerProject.exe"; WorkingDir: "{app}"; Comment: "{cm:reg_apptitle}"; Tasks: quicklaunch

; Other icons in user language
Name: "{group}\{cm:icons_license}"; Filename: "{app}\Uninstall\GPL License.rtf"; WorkingDir: "{app}\Uninstall"; Comment: "{cm:icons_license}"
Name: "{group}\{cm:icons_uninstall}"; Filename: "{uninstallexe}"; WorkingDir: "{app}\Uninstall"; Comment: "{cm:UninstallProgram,PeerProject}"; IconFilename: "{app}\Uninstall\uninstall.ico"
;Name: "{userprograms}\{groupname}\{cm:icons_downloads}"; Filename: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}}"; WorkingDir: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}}"; Comment: "{cm:icons_downloads}"; Tasks: multiuser; Check: not WizardNoIcons
;Name: "{group}\{cm:icons_downloads}"; Filename: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}}"; WorkingDir: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}}"; Comment: "{cm:icons_downloads}"; Tasks: not multiuser; Check: not WizardNoIcons

[Messages]
; Overwrite standard ISL entries  (Do not use localized messages)
BeveledLabel=PeerProject.org
SetupAppTitle=Setup | {#internal_name}

[Run]
; Register EXE servers
;Filename: "{app}\MediaImageServices.exe";  Parameters: "/RegServer"; WorkingDir: "{app}"
;Filename: "{app}\Plugins\WindowsThumbnail.exe";  Parameters: "/RegServer"; WorkingDir: "{app}"

; Run the skin installer at end of installation
Filename: "{app}\SkinInstaller.exe"; Parameters: "/installsilent"; WorkingDir: "{app}"; StatusMsg: "{cm:run_skinexe}"
; Run PeerProject at end of installation
Filename: "{app}\PeerProject.exe"; Description: "{cm:LaunchProgram,PeerProject}"; WorkingDir: "{app}"; Flags: postinstall skipifsilent nowait

[UninstallRun]
; Run the skin installer at start of uninstallation and make sure it only runs once
Filename: "{app}\SkinInstaller.exe"; Parameters: "/uninstallsilent"; WorkingDir: "{app}"; StatusMsg: "{cm:run_skinexe}"; RunOnceId: "uninstallskinexe"

[Registry]
Root: HKLM; Subkey: "SOFTWARE\PeerProject\PeerProject"; ValueType: dword; ValueName: "MultiUser"; ValueData: 1; Flags: deletevalue; Tasks: multiuser
Root: HKLM; Subkey: "SOFTWARE\PeerProject\PeerProject"; ValueType: dword; ValueName: "MultiUser"; ValueData: 0; Flags: deletevalue; Tasks: not multiuser

; Write installation path to registry
Root: HKLM; Subkey: "SOFTWARE\PeerProject"; ValueType: string; ValueName: ; ValueData: "{app}"; Flags: uninsdeletekey deletevalue
Root: HKCU; Subkey: "Software\PeerProject\PeerProject"; ValueType: string; ValueName: "Path" ; ValueData: "{app}"; Flags: uninsdeletekey deletevalue
Root: HKCU; Subkey: "Software\PeerProject\PeerProject"; ValueType: string; ValueName: "UserPath" ; ValueData: "{ini:{param:SETTINGS|},Locations,UserPath|{userappdata}\PeerProject}"; Flags: uninsdeletekey deletevalue ; Tasks: multiuser
Root: HKCU; Subkey: "Software\PeerProject\PeerProject"; ValueType: string; ValueName: "UserPath" ; ValueData: "{ini:{param:SETTINGS|},Locations,UserPath|{app}}"; Flags: uninsdeletekey deletevalue; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\PeerProject.exe"; ValueType: string; ValueName: ; ValueData: "{app}\PeerProject.exe"; Flags: uninsdeletekey deletevalue
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\PeerProject.exe"; ValueType: string; ValueName: "Path"; ValueData: "{app}"; Flags: uninsdeletekey deletevalue

; Install chat notify sound
Root: HKCU; Subkey: "AppEvents\EventLabels\Sound_IncomingChat"; ValueType: string; ValueName: ; ValueData: "{cm:reg_incomingchat}"; Flags: uninsdeletekey
Root: HKCU; Subkey: "AppEvents\Schemes\Apps\PeerProject"; ValueType: string; ValueName: ; ValueData: "{cm:reg_apptitle}"; Flags: uninsdeletekey
Root: HKCU; Subkey: "AppEvents\Schemes\Apps\PeerProject\Sound_IncomingChat\.current"; ValueType: string; ValueName: ; ValueData: "%SystemRoot%\Media\notify.wav"; Flags: uninsdeletekey
Root: HKCU; Subkey: "AppEvents\Schemes\Apps\PeerProject\Sound_IncomingChat\.default"; ValueType: string; ValueName: ; ValueData: "%SystemRoot%\Media\notify.wav"; Flags: uninsdeletekey

; Set UPNP as choosed during the setup
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Connection"; ValueType: dword; ValueName: "EnableUPnP"; ValueData: 1; Flags: deletevalue; Tasks: upnp
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Connection"; ValueType: dword; ValueName: "EnableUPnP"; ValueData: 0; Flags: deletevalue; Tasks: not upnp

; Set directory locations
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Downloads"; ValueType: string; ValueName: "CompletePath"; ValueData: "{ini:{param:SETTINGS|},Locations,CompletePath|{userdocs}\PeerProject Downloads}"; Flags: uninsdeletekey createvalueifdoesntexist; Tasks: multiuser
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Downloads"; ValueType: string; ValueName: "IncompletePath"; ValueData: "{ini:{param:SETTINGS|},Locations,IncompletePath|{localappdata}\PeerProject\Incomplete}"; Flags: uninsdeletekey createvalueifdoesntexist; Tasks: multiuser
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Downloads"; ValueType: string; ValueName: "TorrentPath"; ValueData: "{ini:{param:SETTINGS|},Locations,TorrentPath|{userappdata}\PeerProject\Torrents}"; Flags: uninsdeletekey createvalueifdoesntexist; Tasks: multiuser
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Downloads"; ValueType: string; ValueName: "CollectionPath"; ValueData: "{ini:{param:SETTINGS|},Locations,CollectionPath|{userappdata}\PeerProject\Collections}"; Flags: uninsdeletekey createvalueifdoesntexist; Tasks: multiuser

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Downloads"; ValueType: string; ValueName: "CompletePath"; ValueData: "{ini:{param:SETTINGS|},Locations,CompletePath|{app}\Downloads}"; Flags: uninsdeletekey createvalueifdoesntexist; Tasks: not multiuser
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Downloads"; ValueType: string; ValueName: "IncompletePath"; ValueData: "{ini:{param:SETTINGS|},Locations,IncompletePath|{app}\Incomplete}"; Flags: uninsdeletekey createvalueifdoesntexist; Tasks: not multiuser
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Downloads"; ValueType: string; ValueName: "TorrentPath"; ValueData: "{ini:{param:SETTINGS|},Locations,TorrentPath|{app}\Torrents}"; Flags: uninsdeletekey createvalueifdoesntexist; Tasks: not multiuser
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Downloads"; ValueType: string; ValueName: "CollectionPath"; ValueData: "{ini:{param:SETTINGS|},Locations,CollectionPath|{app}\Collections}"; Flags: uninsdeletekey createvalueifdoesntexist; Tasks: not multiuser

; ShareMonkey CID
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\WebServices"; ValueType: string; ValueName: "ShareMonkeyCid"; ValueData: "197506"; Flags: deletevalue uninsdeletekey

; Set permissions on registry keys
Root: HKLM; Subkey: "SOFTWARE\Classes\.co"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\.collection"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\.torrent"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\bittorrent"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\ed2k"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\gnet"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\gnutella"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\gnutella1"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\gnutella2"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\uhc"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\ukhl"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\magnet"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\mp2p"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\gwc"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\PeerProject"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\PeerProject.Collection"; Permissions: users-modify; Tasks: not multiuser
Root: HKLM; Subkey: "SOFTWARE\Classes\Applications\PeerProject.exe"; Permissions: users-modify; Tasks: not multiuser


; Delete keys at uninstall
Root: HKLM; Subkey: "SOFTWARE\PeerProject"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject"; Flags: dontcreatekey uninsdeletekey
Root: HKU; Subkey: ".DEFAULT\Software\PeerProject"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Run"; ValueName: "PeerProject"; Flags: dontcreatekey uninsdeletevalue

Root: HKLM; Subkey: "SOFTWARE\Classes\.psk"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\.sks"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\.co"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\.collection"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\.torrent"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\bittorrent"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\ed2k"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\gnet"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\gnutella"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\gnutella1"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\gnutella2"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\uhc"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\ukhl"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\magnet"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\mp2p"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\gwc"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\peerproject"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\PeerProject.*"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\Applications\PeerProject.exe"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Classes\Applications\SkinInstaller.exe"; Flags: dontcreatekey uninsdeletekey

Root: HKCU; Subkey: "SOFTWARE\Classes\.psk"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\.sks"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\.co"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\.collection"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\.emulecollection"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\.torrent"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\bittorrent"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\ed2k"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\gnet"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\gnutella"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\gnutella1"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\gnutella2"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\uhc"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\ukhl"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\magnet"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\mp2p"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\gwc"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\peerproject"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\PeerProject.*"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\Applications\PeerProject.exe"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "SOFTWARE\Classes\Applications\SkinInstaller.exe"; Flags: dontcreatekey uninsdeletekey

Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.psk"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.sks"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.torrent"; Flags: dontcreatekey uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\App Management\ARPCache\PeerProject_is1"; Flags: dontcreatekey uninsdeletekey
Root: HKCU; Subkey: "Software\Microsoft\Windows\ShellNoRoam\MUICache"; ValueName:"{app}\PeerProject.exe"; Flags: dontcreatekey uninsdeletevalue

Root: HKLM; Subkey: "SOFTWARE\Magnet"; Flags: dontcreatekey uninsdeletekey

; Delete NSIS entry on software panel
Root: HKLM; Subkey: "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PeerProject"; Flags: dontcreatekey deletekey

; Clear version check key
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\VersionCheck"; Flags: dontcreatekey deletekey
Root: HKLM; Subkey: "Software\PeerProject\PeerProject\VersionCheck"; Flags: dontcreatekey deletekey

; Create TorrentWizard default dir locations
Root: HKCU; Subkey: "Software\PeerProject\TorrentWizard\Folders"; ValueType: string; ValueName: "001.Path"; ValueData: "{userappdata}\PeerProject\Torrents"; Flags: createvalueifdoesntexist; Tasks: multiuser
Root: HKCU; Subkey: "Software\PeerProject\TorrentWizard\Folders"; ValueType: string; ValueName: "Last"; ValueData: "{userappdata}\PeerProject\Torrents"; Flags: createvalueifdoesntexist; Tasks: multiuser
Root: HKCU; Subkey: "Software\PeerProject\TorrentWizard\Folders"; ValueType: string; ValueName: "001.Path"; ValueData: "{app}\Torrents"; Flags: createvalueifdoesntexist; Tasks: not multiuser
Root: HKCU; Subkey: "Software\PeerProject\TorrentWizard\Folders"; ValueType: string; ValueName: "Last"; ValueData: "{app}\Torrents"; Flags: createvalueifdoesntexist; Tasks: not multiuser
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\BitTorrent"; ValueType: string; ValueName: "TorrentCreatorPath"; ValueData: "TorrentWizard.exe" ; Flags: createvalueifdoesntexist uninsdeletekey

; Disable extensions for plugins which make trouble
; Since it is image services plugin we need to add extensions required for the first run
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Plugins"; ValueType: string; ValueName: "{{C9314782-CB91-40B8-B375-F631FF30C1C8}"; ValueData: "|-.pdf||.bmp||.png||.jpg|"; Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Plugins"; Flags: dontcreatekey uninsdeletekey


[Dirs]
; Make complete, incomplete, torrent and collection dir
; Note: download dir will be created when installer is copied but we create also here to be sure
Name: "{ini:{param:SETTINGS|},Locations,UserPath|{reg:HKCU\Software\PeerProject\PeerProject,UserPath|{userappdata}\PeerProject}}\Data"; Flags: uninsalwaysuninstall; Tasks: multiuser
Name: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data"; Flags: uninsalwaysuninstall; Tasks: multiuser
Name: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}}"; Flags: uninsalwaysuninstall; Tasks: multiuser
Name: "{ini:{param:SETTINGS|},Locations,IncompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,IncompletePath|{localappdata}\PeerProject\Incomplete}}"; Flags: uninsalwaysuninstall; Tasks: multiuser
Name: "{ini:{param:SETTINGS|},Locations,TorrentPath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,TorrentPath|{userappdata}\PeerProject\Torrents}}"; Flags: uninsalwaysuninstall; Tasks: multiuser
Name: "{ini:{param:SETTINGS|},Locations,CollectionPath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CollectionPath|{userappdata}\PeerProject\Collections}}"; Flags: uninsalwaysuninstall; Tasks: multiuser

Name: "{ini:{param:SETTINGS|},Locations,Path|{reg:HKCU\Software\PeerProject\PeerProject,Path|{app}}}\Data"; Flags: uninsalwaysuninstall; Permissions: users-modify; Tasks: not multiuser
Name: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}}"; Flags: uninsalwaysuninstall; Permissions: users-modify; Tasks: not multiuser
Name: "{ini:{param:SETTINGS|},Locations,IncompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,IncompletePath|{app}\Incomplete}}"; Flags: uninsalwaysuninstall; Permissions: users-modify; Tasks: not multiuser
Name: "{ini:{param:SETTINGS|},Locations,TorrentPath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,TorrentPath|{app}\Torrents}}"; Flags: uninsalwaysuninstall; Permissions: users-modify; Tasks: not multiuser
Name: "{ini:{param:SETTINGS|},Locations,CollectionPath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CollectionPath|{app}\Collections}}"; Flags: uninsalwaysuninstall; Permissions: users-modify; Tasks: not multiuser

Name: "{app}\Skins"; Flags: uninsalwaysuninstall; Permissions: users-modify

[InstallDelete]
; Very basic malware removal
Type: files; Name: "{app}\PeerProject.exe"; Check: IsMalwareDetected
Type: files; Name: "{app}\vc2.dll"

; Clean up old files from PeerProject
Type: files; Name: "{app}\*.pdb"
Type: files; Name: "{app}\LibGFL*.dll"
Type: files; Name: "{app}\Plugins\*.pdb"
Type: files; Name: "{app}\*.dat"
Type: files; Name: "{app}\*.xml"
;Type: files; Name: "{app}\*.png"
;Type: files; Name: "{app}\*.bmp"
;Type: files; Name: "{app}\Data\*.url"
Type: filesandordirs; Name: "{userappdata}\PeerProject\Remote"
Type: filesandordirs; Name: "{userappdata}\PeerProject\Schemas"
Type: filesandordirs; Name: "{userappdata}\PeerProject\Skins"
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}\Thumbs.db"; Tasks: multiuser
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}\Thumbs.db"
Type: files; Name: "{userappdata}\PeerProject\Data\DefaultAvatar.png"

; Clean up old/unwanted PeerProject Shortcuts
Type: files; Name: "{userdesktop}\PeerProject.lnk"; Tasks: not desktopicon
Type: files; Name: "{commondesktop}\PeerProject.lnk"; Tasks: not desktopicon
Type: files; Name: "{userdesktop}\TorrentWizard.lnk"; Tasks: not desktopiconwizard
Type: files; Name: "{commondesktop}\TorrentWizard.lnk"; Tasks: not desktopiconwizard
;Type: files; Name: "{userdesktop}\Start PeerProject.lnk"; Check: NSISUsed
;Type: filesandordirs; Name: "{userprograms}\PeerProject"; Check: NSISUsed
;Type: filesandordirs; Name: "{commonprograms}\PeerProject"; Check: NSISUsed
Type: filesandordirs; Name: "{userprograms}\PeerProject"; Check: InnoSetupUsed
Type: filesandordirs; Name: "{commonprograms}\PeerProject"; Check: InnoSetupUsed
Type: files; Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\PeerProject.lnk"; Tasks: not quicklaunch
;Following two lines may delete data not created by PeerProject (Instead)
;Type: filesandordirs; Name: "{userprograms}\{reg:HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PeerProject_is1,Inno Setup: Icon Group|{groupname}}"; Check: InnoSetupUsed
;Type: filesandordirs; Name: "{commonprograms}\{reg:HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PeerProject_is1,Inno Setup: Icon Group|{groupname}}"; Check: InnoSetupUsed

; Delete extra components so installer can "uninstall" them
Type: filesandordirs; Name: "{app}\Remote"
Type: filesandordirs; Name: "{app}\Skins\Languages"; Tasks: not language

; Delete old PeerProject installers
#if alpha == "No"
Type: files; Name: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}}\PeerProject*.exe"; Tasks: deleteoldsetup and multiuser
Type: files; Name: "{ini:{param:SETTINGS|},Locations,CompletePath|{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}}\PeerProject*.exe"; Tasks: deleteoldsetup and not multiuser
#endif

; Delete Discovery.dat and HostCache.dat
Type: files; Name: "{app}\Data\Discovery.dat"; Tasks: resetdiscoveryhostcache
Type: files; Name: "{app}\Data\HostCache.dat"; Tasks: resetdiscoveryhostcache
Type: files; Name: "{userappdata}\PeerProject\Data\Discovery.dat"; Tasks: resetdiscoveryhostcache and multiuser
Type: files; Name: "{userappdata}\PeerProject\Data\HostCache.dat"; Tasks: resetdiscoveryhostcache and multiuser

[UninstallDelete]
; Clean up files created after installation
Type: filesandordirs; Name: "{app}\Data"
Type: filesandordirs; Name: "{userappdata}\PeerProject\Data"
Type: filesandordirs; Name: "{app}\Skins"
Type: filesandordirs; Name: "{app}\Templates"
Type: filesandordirs; Name: "{app}\Schemas"

Type: filesandordirs; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}\Metadata"; Tasks: multiuser
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}\Thumbs.db"; Tasks: multiuser
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}\SThumbs.dat"; Tasks: multiuser
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{userdocs}\PeerProject Downloads}\desktop.ini"; Tasks: multiuser
Type: filesandordirs; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,TorrentPath|{userappdata}\PeerProject\Torrents}\Metadata"; Tasks: multiuser
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,TorrentPath|{userappdata}\PeerProject\Torrents}\desktop.ini"; Tasks: multiuser
Type: filesandordirs; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CollectionPath|{userappdata}\PeerProject\Collections}\Metadata"; Tasks: multiuser
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CollectionPath|{userappdata}\PeerProject\Collections}\desktop.ini"; Tasks: multiuser

Type: filesandordirs; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}\Metadata"
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}\Thumbs.db"
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}\SThumbs.dat"
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CompletePath|{app}\Downloads}\desktop.ini"
Type: filesandordirs; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,TorrentPath|{app}\Torrents}\Metadata"
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,TorrentPath|{app}\Torrents}\desktop.ini"
Type: filesandordirs; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CollectionPath|{app}\Collections}\Metadata"
Type: files; Name: "{reg:HKCU\Software\PeerProject\PeerProject\Downloads,CollectionPath|{app}\Collections}\desktop.ini"

; Pull in languages and localized files
#include "Languages.iss"
; Pull in PeerProject settings to write to registry
#include "Settings.iss"

; Code sections need to be the last section in a script or the compiler will get confused
[Code]
type
  SERVICE_STATUS = record
    dwServiceType: cardinal;
    dwCurrentState: cardinal;
    dwControlsAccepted: cardinal;
    dwWin32ExitCode: cardinal;
    dwServiceSpecificExitCode: cardinal;
    dwCheckPoint: cardinal;
    dwWaitHint: cardinal;
  end;
  HANDLE = cardinal;
const
  WM_CLOSE = $0010;
  KeyLoc1 = 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PeerProject_is1';
  KeyLoc2 = 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PeerProject';
  KeyName = 'UninstallString';
  NET_FW_SCOPE_ALL = 0;
  NET_FW_IP_VERSION_ANY       = 2;
  SERVICE_QUERY_CONFIG        = $1;
  SERVICE_CHANGE_CONFIG       = $2;
  SERVICE_QUERY_STATUS        = $4;
  SERVICE_START               = $10;
  SERVICE_STOP                = $20;
  SERVICE_ALL_ACCESS          = $f01ff;
  SC_MANAGER_ALL_ACCESS       = $f003f;
  SERVICE_AUTO_START          = $2;
  SERVICE_DEMAND_START        = $3;
  SERVICE_RUNNING             = $4;
  SERVICE_NO_CHANGE           = $ffffffff;
var
  CurrentPath: string;
  Installed: Boolean;
  MalwareDetected: Boolean;
  FirewallFailed: string;
  HasUserPrivileges: Boolean;

// NT API functions for services
Function OpenSCManager(lpMachineName, lpDatabaseName: string; dwDesiredAccess: cardinal): HANDLE;
external 'OpenSCManagerA@advapi32.dll stdcall setuponly';

Function OpenService(hSCManager: HANDLE; lpServiceName: string; dwDesiredAccess: cardinal): HANDLE;
external 'OpenServiceA@advapi32.dll stdcall setuponly';

Function CloseServiceHandle(hSCObject: HANDLE): Boolean;
external 'CloseServiceHandle@advapi32.dll stdcall setuponly';

Function StartNTService(hService: HANDLE; dwNumServiceArgs: cardinal; lpServiceArgVectors: cardinal): Boolean;
external 'StartServiceA@advapi32.dll stdcall setuponly';

Function QueryServiceStatus(hService: HANDLE; var ServiceStatus: SERVICE_STATUS): Boolean;
external 'QueryServiceStatus@advapi32.dll stdcall setuponly';

Function ChangeServiceConfig(hService: HANDLE; dwServiceType, dwStartType, dwErrorControl: cardinal;
                             lpBinaryPathName, lpLoadOrderGroup: string; lpdwTagId: cardinal;
                             lpDependencies, lpServiceStartName, lpPassword, lpDisplayName: string): Boolean;
external 'ChangeServiceConfigA@advapi32.dll stdcall setuponly';

Function InnoSetupUsed(): boolean;
Begin
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, KeyLoc1);
End;

Function NSISUsed(): boolean;
Begin
    Result := RegKeyExists(HKEY_LOCAL_MACHINE, KeyLoc2);
End;

// check if the current install path exists
Function DoesPathExist(): boolean;
Begin
    if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\PeerProject','', CurrentPath) then
        Result := DirExists(CurrentPath)
    else
        Result := False;
End;

Function OpenServiceManager(): HANDLE;
begin
  Result := 0;
  if (InstallOnThisVersion('0,5.01', '0,0') = irInstall) then
    Result := OpenSCManager('', 'ServicesActive', SC_MANAGER_ALL_ACCESS);
end;

Function CanUserModifyServices(): Boolean;
var
 hSCManager: HANDLE;
begin
  hSCManager := 0;
  Result := false;
  HasUserPrivileges := false;
  if (InstallOnThisVersion('0,5.01', '0,0') = irInstall) then begin
    hSCManager := OpenSCManager('', 'ServicesActive', SC_MANAGER_ALL_ACCESS);
    if (hSCManager <> 0) then begin
      HasUserPrivileges := true;
      Result := true;
      CloseServiceHandle(hSCManager);
    end;
  end;
end;

Function IsServiceInstalled(ServiceName: string): boolean;
var
 hSCManager: HANDLE;
 hService: HANDLE;
begin
  hSCManager := OpenServiceManager();
  Result := false;
  if (hSCManager <> 0) then begin
    hService := OpenService(hSCManager, ServiceName, SERVICE_QUERY_CONFIG);
    if (hService <> 0) then begin
      Result := true;
      CloseServiceHandle(hService);
    end;
    CloseServiceHandle(hSCManager);
  end;
end;

Function StartService(ServiceName: string): boolean;
var
  hSCManager: HANDLE;
  hService: HANDLE;
begin
  hSCManager := OpenServiceManager();
  Result := false;
  if (hSCManager <> 0) then begin
    hService := OpenService(hSCManager, ServiceName, SERVICE_START);
    if (hService <> 0) then begin
      Result := StartNTService(hService, 0, 0);
      CloseServiceHandle(hService);
    end;
    CloseServiceHandle(hSCManager);
  end;
end;

Function IsServiceRunning(ServiceName: string): boolean;
var
  hSCManager: HANDLE;
  hService: HANDLE;
  sStatus: SERVICE_STATUS;
begin
  hSCManager := OpenServiceManager();
  Result := false;
  if (hSCManager <> 0) then begin
    hService := OpenService(hSCManager, ServiceName, SERVICE_QUERY_STATUS);
    if (hService <> 0) then begin
      if (QueryServiceStatus(hService, sStatus)) then
        Result := (sStatus.dwCurrentState = SERVICE_RUNNING);
      CloseServiceHandle(hService);
    end;
    CloseServiceHandle(hSCManager);
 end;
end;

Function ChangeServiceStartup(ServiceName: string; dwStartType: cardinal): boolean;
var
  hSCManager: HANDLE;
  hService: HANDLE;
begin
  hSCManager := OpenServiceManager();
  Result := false;
  if (hSCManager <> 0) then begin
    hService := OpenService(hSCManager, ServiceName, SERVICE_CHANGE_CONFIG);
    if (hService <> 0) then begin
       Result := ChangeServiceConfig(hService, SERVICE_NO_CHANGE, dwStartType, SERVICE_NO_CHANGE, '','',0,'','','','');
       CloseServiceHandle(hService);
    end;
    CloseServiceHandle(hSCManager);
  end;
end;

Function NextButtonClick(CurPageID: integer): Boolean;
var
  Wnd: HWND;
  Shutdownmessage: string;
begin
  Result := True;
  if (CurPageID = wpWelcome) then begin
    Wnd := FindWindowByClassName('PeerProjectMainWnd');
    if Wnd <> 0 then begin
      Shutdownmessage := ExpandConstant('{cm:dialog_shutdown,PeerProject}');
      if MsgBox(Shutdownmessage, mbConfirmation, MB_OKCANCEL) = IDOK then begin
        SendMessage(Wnd, WM_CLOSE, 0, 0);
        while Wnd <> 0 do begin
          Sleep(100);
          Wnd := FindWindowByClassName('PeerProjectMainWnd');
        end;
      end else Result := False;
    end;
  end;
end;

Function MalwareCheck(MalwareFile: string): Boolean;
Begin
  Result := False;
  if FileExists( MalwareFile ) then Begin
    if MsgBox(ExpandConstant( '{cm:dialog_malwaredetected,' + MalwareFile + '}' ), mbConfirmation, MB_YESNO) = IDYES then begin
      Result := True;
    End;
    MalwareDetected := True;
  End;
End;

Function InitializeSetup: Boolean;
Begin
  Result := True;
  Installed := (RegValueExists(HKEY_LOCAL_MACHINE, KeyLoc1, KeyName) or RegValueExists(HKEY_LOCAL_MACHINE, KeyLoc2, KeyName)) and DoesPathExist();
  MalwareDetected := False;

  // Malware check
  Result := NOT MalwareCheck( ExpandConstant('{win}\vgraph.dll') );
  if Result then Begin Result := NOT MalwareCheck( ExpandConstant('{win}\Shareaza*') ); End;
  if Result then Begin Result := NOT MalwareCheck( ExpandConstant('{sys}\Shareaza*') ); End;
  if Result then Begin Result := NOT MalwareCheck( ExpandConstant('{win}\PeerProject*') ); End;
  if Result then Begin Result := NOT MalwareCheck( ExpandConstant('{sys}\PeerProject*') ); End;
  if Result then Begin Result := NOT MalwareCheck( ExpandConstant('{pf}\PeerProject\vc2.dll') ); End;
End;

Function IsMalwareDetected: Boolean;
Begin
  Result := MalwareDetected;
End;

Function EnableDeleteOldSetup: Boolean;
Begin
  Result := Installed;
End;

Function WeOwnTorrentAssoc: boolean;
var
  CommandString: string;
  Position: Integer;
Begin
  Result := False;
  if RegQueryStringValue(HKEY_CLASSES_ROOT, 'bittorrent\shell\open\command','', CommandString) then
    Begin
      Position := Pos('PeerProject.exe', LowerCase(CommandString));
      Result := (Position > 0);
    End
End;

Function ShouldSkipPage(PageID: Integer): Boolean;
Begin
  Result := False;
  if PageID = wpSelectDir then Result := Installed;
End;

Procedure DeleteFolder(Param: String);
var
  Foldername: string;
Begin
  Foldername := ExpandConstant(Param);
  DelTree(Foldername, True, True, True);
End;

Procedure DeleteFile(Param: String);
var
  Filename: string;
Begin
  Filename := ExpandConstant(Param);
  DelayDeleteFile(Filename,3);
End;

// We don't allow to modify the setting of MultiUser if was already selected.
Procedure CurPageChanged(const CurrentPage: integer);
var
  i : integer;
  MultiUserValue: DWORD;
Begin
  if CurrentPage = wpSelectTasks then begin
    i := WizardForm.TasksList.Items.IndexOf(ExpandConstant('{cm:tasks_multisetup}'));
    if i <> -1 then begin
      if RegQueryDWordValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\PeerProject\PeerProject', 'MultiUser', MultiUserValue) then begin
        if MultiUserValue = 1 then
          Wizardform.TasksList.Checked[i] := true
        else
          Wizardform.TasksList.Checked[i] := false;

        WizardForm.TasksList.ItemEnabled[i] := false;
      End;
    End;
  End;
End;

Procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  InstallFolder: string;
  FirewallManager: Variant;
  FirewallProfile: Variant;
  Wnd: HWND;
Begin
  if CurUninstallStep = usUninstall then begin
    if InstallOnThisVersion('0,5.01sp2','0,0') = irInstall then begin
      try
        InstallFolder := ExpandConstant('{app}\PeerProject.exe');
        FirewallManager := CreateOleObject('HNetCfg.FwMgr');
        FirewallProfile := FirewallManager.LocalPolicy.CurrentProfile;
        FirewallProfile.AuthorizedApplications.Remove(InstallFolder);
      except
      End;
    End;
    Wnd := FindWindowByClassName('PeerProjectMainWnd');
    if Wnd <> 0 then begin
      SendMessage(Wnd, WM_CLOSE, 0, 0);
      while Wnd <> 0 do
        begin
          Sleep(100);
          Wnd := FindWindowByClassName('PeerProjectMainWnd');
        End;
    End;
    if WeOwnTorrentAssoc then begin
      RegDeleteKeyIncludingSubkeys(HKEY_CLASSES_ROOT,'.torrent');
      RegDeleteKeyIncludingSubkeys(HKEY_CLASSES_ROOT,'bittorrent');
    End;
  End;
End;

Function IsLanguageRTL(LangCode: String): String;
Begin
  if ( (LangCode = 'heb') or (LangCode = 'ar') ) then
    Result := '1'
  else
    Result := '0';
End;

Function GetRelFilePath(LangCode: String): String;
Begin
  StringChangeEx(LangCode, '_', '-', True);

  if ( LangCode = 'en-uk' ) then
    Result := 'Languages\default-alt.xml'
  else if ( LangCode = 'pt' ) then
    Result := 'Languages\default-pt-br.xml'
  else if ( LangCode = 'af' ) then
    Result := 'Languages\default-za.xml'
  else
    Result := 'Languages\default-' + LangCode + '.xml';
End;

Function ResetLanguages: boolean;
var
  Names: TArrayOfString;
  I: Integer;
  S: String;
  Value: String;
begin
  if RegGetValueNames(HKEY_CURRENT_USER, 'Software\PeerProject\PeerProject\Skins', Names) then
  begin
    S := '';
    Value := LowerCase(GetRelFilePath(ExpandConstant('{language}')));
    for I := 0 to GetArrayLength(Names)-1 do
    begin
      S := LowerCase(Names[I]);
      if Pos('languages', S) <> 0 then
        if Value <> S then
          RegWriteDWordValue(HKEY_CURRENT_USER, 'Software\PeerProject\PeerProject\Skins', S, 0);
    end;
    RegWriteDWordValue(HKEY_CURRENT_USER, 'Software\PeerProject\PeerProject\Skins', Value, 1);
    Value := IsLanguageRTL(ExpandConstant('{language}'));
    RegWriteDWordValue(HKEY_CURRENT_USER, 'Software\PeerProject\PeerProject\Settings', 'LanguageRTL', StrToInt(Value));
    RegWriteStringValue(HKEY_CURRENT_USER, 'Software\PeerProject\PeerProject\Settings', 'Language', ExpandConstant('{language}'));
    // Set default values for other users
    RegWriteDWordValue(HKEY_LOCAL_MACHINE, 'Software\PeerProject\PeerProject', 'DefaultLanguageRTL', StrToInt(Value));
    RegWriteStringValue(HKEY_LOCAL_MACHINE, 'Software\PeerProject\PeerProject', 'DefaultLanguage', ExpandConstant('{language}'));
  end;
  Result := True;
end;

Procedure CurStepChanged(CurStep: TSetupStep);
var
  InstallFolder: string;
  FirewallObject: Variant;
  FirewallManager: Variant;
  FirewallProfile: Variant;
  Reset: boolean;
  Success: boolean;
  Path: string;
Begin
  if CurStep=ssPostInstall then begin
    if IsTaskSelected('firewall') then begin
      if WizardSilent = True then begin
        try
          FirewallObject := CreateOleObject('HNetCfg.FwAuthorizedApplication');
          InstallFolder := ExpandConstant('{app}\PeerProject.exe');
          FirewallObject.ProcessImageFileName := InstallFolder;
          FirewallObject.Name := 'PeerProject';
          FirewallObject.Scope := NET_FW_SCOPE_ALL;
          FirewallObject.IpVersion := NET_FW_IP_VERSION_ANY;
          FirewallObject.Enabled := True;
          FirewallManager := CreateOleObject('HNetCfg.FwMgr');
          FirewallProfile := FirewallManager.LocalPolicy.CurrentProfile;
          FirewallProfile.AuthorizedApplications.Add(FirewallObject);
        except
        End;
      End else begin
        FirewallFailed := ExpandConstant('{cm:dialog_firewall}')
        try
          FirewallObject := CreateOleObject('HNetCfg.FwAuthorizedApplication');
          InstallFolder := ExpandConstant('{app}\PeerProject.exe');
          FirewallObject.ProcessImageFileName := InstallFolder;
          FirewallObject.Name := 'PeerProject';
          FirewallObject.Scope := NET_FW_SCOPE_ALL;
          FirewallObject.IpVersion := NET_FW_IP_VERSION_ANY;
          FirewallObject.Enabled := True;
          FirewallManager := CreateOleObject('HNetCfg.FwMgr');
          FirewallProfile := FirewallManager.LocalPolicy.CurrentProfile;
          FirewallProfile.AuthorizedApplications.Add(FirewallObject);
        except
          MsgBox(FirewallFailed, mbInformation, MB_OK);
        End;
      End;
    End;
    if IsTaskSelected('upnp') then begin
      if (HasUserPrivileges) then begin
        Success := false;
        if (IsServiceInstalled('SSDPSRV') and IsServiceInstalled('upnphost')) then begin
          if (not IsServiceRunning('SSDPSRV')) then begin
            // change the startup type to manual if it was disabled;
            // we don't need to start it since UPnP Device Host service depends on it;
            // assuming that user didn't modify the dependencies manually.
            // Note: probably, we could elevate user rights with AdjustTokenPrivileges(?)
            Success := ChangeServiceStartup('SSDPSRV', SERVICE_DEMAND_START);
          end else
            Success := true;
          if (Success) then begin
            // We succeeded to change the startup type, so we will change another service
            Success := ChangeServiceStartup('upnphost', SERVICE_AUTO_START);
            if (Success and not IsServiceRunning('upnphost')) then
              StartService('upnphost');
          end;
        end;
      end;
    end;
  End;
  if CurStep=ssInstall then begin
    if not IsTaskSelected('firewall') then begin
      if InstallOnThisVersion('0,5.01sp2','0,0') = irInstall then begin
        try
          InstallFolder := ExpandConstant('{app}\PeerProject.exe');
          FirewallManager := CreateOleObject('HNetCfg.FwMgr');
          FirewallProfile := FirewallManager.LocalPolicy.CurrentProfile;
          FirewallProfile.AuthorizedApplications.Remove(InstallFolder);
        except
        End;
      End;
    End;
  End;

  // Check if the needed paths exist otherwise delete it from the registry (They will be recreated later in the installation process)
  if CurStep=ssInstall then begin
    if RegQueryStringValue(HKEY_CURRENT_USER, 'SOFTWARE\PeerProject\PeerProject\Downloads', 'CompletePath', Path) and (not DirExists(Path)) then begin
      if not RegDeleteValue(HKEY_CURRENT_USER, 'SOFTWARE\PeerProject\PeerProject\Downloads', 'CompletePath') then begin
        MsgBox(ExpandConstant('{cm:PathNotExist,complete}'), mbError, MB_OK);
      End;
    End;
    if RegQueryStringValue(HKEY_CURRENT_USER, 'SOFTWARE\PeerProject\PeerProject\Downloads', 'IncompletePath', Path) and (not DirExists(Path)) then begin
      if not RegDeleteValue(HKEY_CURRENT_USER, 'SOFTWARE\PeerProject\PeerProject\Downloads', 'IncompletePath') then begin
        MsgBox(ExpandConstant('{cm:PathNotExist,incomplete}'), mbError, MB_OK);
      End;
    End;
    if RegQueryStringValue(HKEY_CURRENT_USER, 'SOFTWARE\PeerProject\PeerProject\Downloads', 'CollectionPath', Path) and (not DirExists(Path)) then begin
      if not RegDeleteValue(HKEY_CURRENT_USER, 'SOFTWARE\PeerProject\PeerProject\Downloads', 'CollectionPath') then begin
        MsgBox(ExpandConstant('{cm:PathNotExist,collection}'), mbError, MB_OK);
      End;
    End;
    if RegQueryStringValue(HKEY_CURRENT_USER, 'SOFTWARE\PeerProject\PeerProject\Downloads', 'TorrentPath', Path) and (not DirExists(Path)) then begin
      if not RegDeleteValue(HKEY_CURRENT_USER, 'SOFTWARE\PeerProject\PeerProject\Downloads', 'TorrentPath') then begin
        MsgBox(ExpandConstant('{cm:PathNotExist,torrent}'), mbError, MB_OK);
      End;
    End;
  End;

  if CurStep=ssDone then Reset := ResetLanguages;
End;

{ Pull in custom wizard pages }
#include "Pages.iss"

#expr SaveToFile("Scripts\Preprocessed.iss")
