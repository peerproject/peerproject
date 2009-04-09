; This sub-script lists all PeerProject settings that have to be written at installation time

[Registry]
; Turn on SkinVista skin
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Skins"; ValueType: dword; ValueName: "SkinVista\SkinVista.xml"; ValueData: "{ini:{param:SETTINGS|},Skins,SkinVista|1}"; Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Skins"; ValueType: dword; ValueName: "SkinVista\SkinVista_{language}.xml"; ValueData: "{ini:{param:SETTINGS|},Skins,SkinVista|1}"; Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\MediaPlayer\"; ValueType: dword; ValueName: "ListSize"; ValueData: 139 ;				Flags: createvalueifdoesntexist uninsdeletekey

; Set Menubar
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Summary"; ValueType: dword; ValueName: "Bars";  ValueData: 7 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar0"; ValueType: dword; ValueName: "BarID";  ValueData: $0000E801 ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bars";  ValueData: 9 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#0"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#1"; ValueData: $0000E813 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#2"; ValueData: $0000E811 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#3"; ValueData: $0000E810 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#4"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#5"; ValueData: $0000E812 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#6"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#7"; ValueData: $0000E800 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#8"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E810 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "Docking"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockLeftPos"; ValueData: $00000230 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockRightPos"; ValueData: $00000632 ; 	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockTopPos"; ValueData: $FFFFFFFE ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockBottomPos"; ValueData: $0000001A ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUFloatStyle"; ValueData: $00002000 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUFloatXPos"; ValueData: $00000033 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUFloatYPos"; ValueData: $00000224 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "XPos"; ValueData: $00000230 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "YPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E812 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "Docking"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockLeftPos"; ValueData: $FFFFFFFE ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockRightPos"; ValueData: $00000400 ; 	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockTopPos"; ValueData: $00000018 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockBottomPos"; ValueData: $00000032 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUFloatStyle"; ValueData: $00002000 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUFloatXPos"; ValueData: $0000006C ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUFloatYPos"; ValueData: $00000089 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "XPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "YPos"; ValueData: $00000018 ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E811 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "Docking"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockLeftPos"; ValueData: $0000007C ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockRightPos"; ValueData: $000000232 ; 	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockTopPos"; ValueData: $FFFFFFFE ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockBottomPos"; ValueData: $0000001A ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUFloatStyle"; ValueData: $00002000 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUFloatXPos"; ValueData: $0000004F ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUFloatYPos"; ValueData: $00000190 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "XPos"; ValueData: $0000007C ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "YPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar5"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E800 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar5"; ValueType: dword; ValueName: "Visible"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar5"; ValueType: dword; ValueName: "Docking"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar5"; ValueType: dword; ValueName: "MRUDockID"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E813 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "Docking"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "MRUDockID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "MRUDockLeftPos"; ValueData: $FFFFFFFE ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "MRUDockRightPos"; ValueData: $0000007E ; 	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "MRUDockTopPos"; ValueData: $FFFFFFFE ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "MRUDockBottomPos"; ValueData: $0000001A ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "MRUFloatStyle"; ValueData: $00002000 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "MRUFloatXPos"; ValueData: $0000003D ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "MRUFloatYPos"; ValueData: $000000DB ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "XPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "YPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Bars";  ValueData: 3 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E81F ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Bar#0"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Bar#1"; ValueData: $0000E813 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Bar#2"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Horz"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Floating"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "XPos"; ValueData: $0000004A ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "YPos"; ValueData: $00000116 ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Bars";  ValueData: 3 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E81F ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Bar#0"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Bar#1"; ValueData: $0000E811 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Bar#2"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Horz"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Floating"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "XPos"; ValueData: $00000057 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "YPos"; ValueData: $000001AA ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "Bars";  ValueData: 3 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E81F ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "Bar#0"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "Bar#1"; ValueData: $0000E810 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "Bar#2"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "Horz"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "Floating"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "XPos"; ValueData: $0000003B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar9"; ValueType: dword; ValueName: "YPos"; ValueData: $0000023E ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "Bars";  ValueData: 3 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E81F ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "Bar#0"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "Bar#1"; ValueData: $0000E812 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "Bar#2"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "Horz"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "Floating"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "XPos"; ValueData: $00000050 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar10"; ValueType: dword; ValueName: "YPos"; ValueData: $000002CB ;		Flags: createvalueifdoesntexist uninsdeletekey


; Disable extensions for plugins which make trouble
; Since it is image services plugin we need to add extensions required for the first run
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Plugins"; ValueType: string; ValueName: "{{FF5FCD00-2C20-49D8-84F6-888D2E2C95DA}"; ValueData: "|-.pdf||.bmp||.png||.jpg|"; Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\MediaPlayer"; ValueType: dword; ValueName: "EnablePlay"; ValueData: 0 ;				Flags: createvalueifdoesntexist uninsdeletekey
; Root: HKCU; Subkey: "Software\PeerProject\PeerProject\MediaPlayer"; ValueType: dword; ValueName: "EnableEnqueue"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey

; Set Wizard Defaults
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Settings"; ValueType: dword; ValueName: "GUIMode"; ValueData: 1 ; 				Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\eDonkey";  ValueType: dword; ValueName: "EnableAlways"; ValueData: 1 ;				Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Gnutella1"; ValueType: dword; ValueName: "EnableAlways"; ValueData: 1 ;				Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\BitTorrent"; ValueType: dword; ValueName: "AdvancedInterface"; ValueData: 1 ;			Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\BitTorrent"; ValueType: string; ValueName: "TorrentCreatorPath"; ValueData: "TorrentWizard.exe" ; Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\BitTorrent"; ValueType: string; ValueName: "DefaultTracker"; ValueData: "http://tracker.thepiratebay.org/announce" ; Flags: createvalueifdoesntexist uninsdeletekey