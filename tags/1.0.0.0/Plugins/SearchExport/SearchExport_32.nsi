!define TITLE		"PeerProject Search Export Plugin"
!define VERSION		"1.0.0.0"
!define COMPANY		"PeerProject"
!define FILENAME	"Win32\SearchExport_Setup32_${VERSION}.exe"
!define COPYRIGHT	"Copyright 2009 Nikolay Raspopov"
!define UNINST		"SearchExport_Uninst.exe"

Name "${TITLE}"

VIProductVersion "${VERSION}"
VIAddVersionKey ProductName "${TITLE}"
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey OriginalFilename "${FILENAME}"
VIAddVersionKey FileDescription "${TITLE}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey LegalCopyright "${COPYRIGHT}"

CRCCheck On
XPStyle On
BrandingText "PeerProject Development Team"
SetOverwrite On
OutFile "${FILENAME}"
InstallDir "$PROGRAMFILES32\PeerProject"
InstallDirRegKey HKCU "Software\PeerProject\PeerProject" "Path"
ShowInstDetails show
ShowUninstDetails show
RequestExecutionLevel admin
SetCompressor /SOLID lzma

Var STARTMENU_FOLDER

!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_HEADERIMAGE
; !define MUI_COMPONENTSPAGE_NODESC
; !define MUI_FINISHPAGE_NOAUTOCLOSE
; !define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_ICON "..\..\setup\misc\install.ico"
!define MUI_UNICON "..\..\setup\misc\uninstall.ico"
!define MUI_HEADERIMAGE_BITMAP "..\..\setup\misc\corner_150x57.bmp"
!define MUI_HEADERIMAGE_BITMAP_NOSTRETCH
!define MUI_HEADERIMAGE_UNBITMAP "..\..\setup\misc\corner_150x57.bmp"
!define MUI_HEADERIMAGE_UNBITMAP_NOSTRETCH
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\..\setup\misc\sidebar.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "..\..\setup\misc\sidebar.bmp"
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "PeerProject"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\PeerProject"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\setup\license\default.rtf"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Function .onInit
	SetShellVarContext all
	SetRegView 32

	; Disable second run
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${TITLE}") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	MessageBox MB_ICONSTOP|MB_OK "Setup already running."
	Quit

FunctionEnd

Function un.onInit
	SetShellVarContext all
	SetRegView 32

	; Disable second run
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${TITLE}") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	MessageBox MB_ICONSTOP|MB_OK "Setup already running."
	Quit

FunctionEnd

Section "Install"
	SetOutPath $INSTDIR

	; Close PeerProject before installation
	DetailPrint "Checking for PeerProject..."
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "Global\PeerProject") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	MessageBox MB_ICONSTOP|MB_OK "Please close PeerProject and run setup again."
	Quit

	; Install plugin
	File /r "Templates"
	File "Release Win32\SearchExport.dll"
	RegDLL "$INSTDIR\SearchExport.dll"

	; Install Uninstaller
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
	CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
	CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall ${TITLE}.lnk" "$INSTDIR\${UNINST}" "" "$INSTDIR\${UNINST}" 0
	!insertmacro MUI_STARTMENU_WRITE_END
	WriteRegStr   HKCU "Software\PeerProject\PeerProject" "Path" $INSTDIR
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}" "DisplayName"     "${TITLE}"
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}" "UninstallString" "$INSTDIR\${UNINST}"
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}" "InstallLocation" "$INSTDIR"
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}" "DisplayIcon"     "$INSTDIR\SearchExport.dll,0"
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}" "DisplayVersion"  "${VERSION}"
	WriteRegStr   HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}" "Publisher"       "${COMPANY}"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}" "NoModify"         1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}" "NoRepair"         1
	WriteUninstaller "${UNINST}"
SectionEnd

Section "Uninstall"
	SetOutPath $TEMP

	; Close PeerProject before uninstallation
	DetailPrint "Checking for PeerProject..."
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "Global\PeerProject") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +3
	MessageBox MB_ICONSTOP|MB_OK "Please close PeerProject and run setup again."
	Quit

	; Uninstall plugin
	UnRegDLL "$INSTDIR\SearchExport.dll"
	Delete   "$INSTDIR\SearchExport.dll"
	RmDir /r "$INSTDIR\Templates"

	; Uninstall uninstaller
	!insertmacro MUI_STARTMENU_GETFOLDER Application $STARTMENU_FOLDER
	Delete "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall ${TITLE}.lnk"
	RmDir  "$SMPROGRAMS\$STARTMENU_FOLDER"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${TITLE}"
	Delete "$INSTDIR\${UNINST}"
	RmDir  "$INSTDIR"
SectionEnd

!appendfile "SearchExport.trg" "[${__TIMESTAMP__}] ${TITLE} ${VERSION} ${FILENAME}$\n"
