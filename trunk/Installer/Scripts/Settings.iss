; This sub-script lists extra PeerProject settings that need to be written at installation time

[Registry]
; Enable SkinSeven Theme
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Skins"; ValueType: dword; ValueName: "SkinVista\SkinSeven.xml"; ValueData: "{ini:{param:SETTINGS|},Skins,SkinSeven|1}"; Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\MediaPlayer"; ValueType: dword; ValueName: "ListSize"; ValueData: 139 ;				Flags: createvalueifdoesntexist uninsdeletekey

; Set SkinVista Menubar
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Summary"; ValueType: dword; ValueName: "Bars";  ValueData: 7 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar0"; ValueType: dword; ValueName: "BarID";  ValueData: $0000E801 ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bars";  ValueData: 9 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#0"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#1"; ValueData: $0000E813 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#2"; ValueData: $0000E811 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#3"; ValueData: $0000E810 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#4"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#5"; ValueData: $0000E800 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#6"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#7"; ValueData: $0000E812 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar1"; ValueType: dword; ValueName: "Bar#8"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E810 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "Docking"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockLeftPos"; ValueData: $00000240 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockRightPos"; ValueData: $00000600 ; 	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockTopPos"; ValueData: $FFFFFFFE ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUDockBottomPos"; ValueData: $0000001A ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "MRUFloatStyle"; ValueData: $00002000 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "XPos"; ValueData: $0000022C ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar2"; ValueType: dword; ValueName: "YPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E812 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "Docking"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockLeftPos"; ValueData: $00000000 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockRightPos"; ValueData: $00000400 ; 	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockTopPos"; ValueData: $00000018 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUDockBottomPos"; ValueData: $00000032 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "MRUFloatStyle"; ValueData: $00002000 ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "XPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar3"; ValueType: dword; ValueName: "YPos"; ValueData: $00000018 ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E811 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "Docking"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockID"; ValueData: $0000E81B ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockLeftPos"; ValueData: $0000006C ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockRightPos"; ValueData: $00000022E ; 	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockTopPos"; ValueData: $FFFFFFFE ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUDockBottomPos"; ValueData: $0000001A ;	Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar4"; ValueType: dword; ValueName: "MRUFloatStyle"; ValueData: $00002000 ;	Flags: createvalueifdoesntexist uninsdeletekey
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
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "XPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar6"; ValueType: dword; ValueName: "YPos"; ValueData: $FFFFFFFE ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Bars";  ValueData: 3 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E813 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Bar#0"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Bar#1"; ValueData: $0000E813 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Bar#2"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Horz"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "Floating"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "XPos"; ValueData: $00000057 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar7"; ValueType: dword; ValueName: "YPos"; ValueData: $00000018 ;		Flags: createvalueifdoesntexist uninsdeletekey

Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Bars";  ValueData: 3 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "BarID"; ValueData: $0000E81F ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Bar#0"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Bar#1"; ValueData: $0000E813 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Bar#2"; ValueData: 0 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Horz"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "Floating"; ValueData: 1 ;			Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "XPos"; ValueData: $00000057 ;		Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Toolbars\CoolBar-Bar8"; ValueType: dword; ValueName: "YPos"; ValueData: $00000019 ;		Flags: createvalueifdoesntexist uninsdeletekey

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


; Disable extensions for plugins which make trouble (?)
; Since it is image services plugin we need to add extensions required for the first run
; Root: HKCU; Subkey: "Software\PeerProject\PeerProject\Plugins"; ValueType: string; ValueName: "{C9314782-CB91-40B8-B375-F631FF30C1C8}"; ValueData: "|-.pdf||.bmp||.png||.jpg|"; Flags: createvalueifdoesntexist uninsdeletekey

; Set Defaults
Root: HKCU; Subkey: "Software\PeerProject\PeerProject\BitTorrent"; ValueType: string; ValueName: "DefaultTracker"; ValueData: "http://tracker.openbittorrent.com:80/announce" ;  Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\TorrentWizard\Trackers"; ValueType: string; ValueName: "001.URL"; ValueData: "http://tracker.openbittorrent.com:80/announce" ;  Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\TorrentWizard\Trackers"; ValueType: string; ValueName: "002.URL"; ValueData: "http://tracker.publicbt.com:80/announce" ;  Flags: createvalueifdoesntexist uninsdeletekey
Root: HKCU; Subkey: "Software\PeerProject\TorrentWizard\Trackers"; ValueType: dword;  ValueName: "Count"; ValueData: 2 ;  Flags: createvalueifdoesntexist uninsdeletekey
