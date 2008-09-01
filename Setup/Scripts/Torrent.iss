; Select file source root
#ifexist "..\..\VS2005\ReleaseWin32\TorrentWizard.exe"
  #define root "VS2005\ReleaseWin32"
  #define version GetFileVersion("..\..\VS2005\ReleaseWin32\TorrentWizard.exe")
#endif

#ifndef root
  #error You must compile TorrentWizard before compile the setup
#endif

[Setup]
AppName=TorrentWizard
AppVerName=TorrentWizard {#version}
AppPublisher=PeerProject Development Team
DefaultDirName={pf}\TorrentWizard
DefaultGroupName=TorrentWizard
DisableProgramGroupPage=yes
Compression=lzma/max
InternalCompressLevel=max
SolidCompression=yes
OutputBaseFilename=TorrentWizard_{#version}
OutputDir=Setup\Builds
VersionInfoCompany=PeerProject Development Team
VersionInfoDescription=TorrentWizard
VersionInfoVersion={#version}
AppId=TorrentWizard
AppVersion={#version}
DirExistsWarning=no
PrivilegesRequired=poweruser
LanguageDetectionMethod=locale
ShowLanguageDialog=auto
UninstallDisplayIcon={app}\TorrentWizard.exe
UninstallDisplayName={cm:NameAndVersion,TorrentWizard,{#version}}
UninstallFilesDir={app}\Uninstall
LicenseFile=setup\license\default.rtf
SetupIconFile=TorrentWizard\Res\TorrentWizard.ico
WizardImageFile=Setup\Res\WizardImages\Sidebar.bmp
WizardSmallImageFile=Setup\Res\WizardImages\CornerLogo.bmp

; Set the SVN root as source dir (up 2 levels)
SourceDir=..\..

; links to website for software panel
AppPublisherURL=http://TorrentWizard.PeerProject.org
AppSupportURL=http://support.peerproject.org
AppUpdatesURL=http://download.peerproject.org

[Files]
; Install unicows.dll on Win 9X
Source: "Setup\Builds\unicows.dll"; DestDir: "{app}"; Flags: overwritereadonly replacesameversion restartreplace uninsremovereadonly sortfilesbyextension; MinVersion: 4.0,0

; Main file
Source: "{#root}\TorrentWizard.exe"; DestDir: "{app}"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension

; Uninstall icon for software panel
Source: "TorrentWizard\Res\Uninstall.ico"; DestDir: "{app}\Uninstall"; Flags: ignoreversion sortfilesbyextension

[Icons]
Name: "{group}\Torrent Wizard"; Filename: "{app}\TorrentWizard.exe"; WorkingDir: "{app}"; Comment: "PeerProject Torrent Wizard"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"; WorkingDir: "{app}\Uninstall"; Comment: "{cm:UninstallProgram,TorrentWizard}"; IconFilename: "{app}\Uninstall\uninstall.ico"

[Registry]
Root: HKLM; Subkey: "Software\PeerProject\PeerProject\BitTorrent"; ValueType: string; ValueName: "TorrentCreatorPath"; ValueData: "{app}\TorrentWizard.exe"; Flags: deletevalue uninsdeletevalue
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\BitTorrent"; ValueType: string; ValueName: "TorrentCreatorPath"; ValueData: "{app}\TorrentWizard.exe"; Flags: deletevalue uninsdeletevalue
Root: HKCU; Subkey: "Software\PeerProject\TorrentWizard\"; Flags: dontcreatekey uninsdeletekey

[InstallDelete]
Type: files; Name: "{userprograms}\TorrentWizard.*"
Type: filesandordirs; Name: "{group}"

