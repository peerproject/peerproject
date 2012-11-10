; This sub-script defines all languages to be compiled
; WARNING: Do not change this file's encoding

[Languages]
; Use compiler's built in ISL file to patch up holes in ISL collection and specify localized license files
; Note: first language specified is default > English

Name: "en"; MessagesFile: "compiler:Default.isl"; LicenseFile: "Installer/License/Default.rtf"
Name: "en_uk"; MessagesFile: "Installer\Languages\English-UK.isl"; LicenseFile: "Installer/License/Default.rtf"
Name: "ar"; MessagesFile: "Installer\Languages\Arabic.isl"; LicenseFile: "Installer/License/Arabic.rtf"
Name: "af"; MessagesFile: "Installer\Languages\Afrikaans.isl"; LicenseFile: "Installer/License/Afrikaans.rtf"
Name: "ca"; MessagesFile: "Installer\Languages\Catalan.isl"; LicenseFile: "Installer/License/Catalan.rtf"
Name: "cz"; MessagesFile: "Installer\Languages\Czech.isl"; LicenseFile: "Installer/License/Czech.rtf"
Name: "de"; MessagesFile: "Installer\Languages\German.isl"; LicenseFile: "Installer/License/German.rtf"
Name: "ee"; MessagesFile: "Installer\Languages\Estonian.isl"; LicenseFile: "Installer/License/Estonian.rtf"
Name: "es"; MessagesFile: "Installer\Languages\Spanish.isl"; LicenseFile: "Installer/License/Spanish.rtf"
Name: "es_mx"; MessagesFile: "Installer\Languages\Spanish-Mexican.isl"; LicenseFile: "Installer/License/Spanish.rtf"
Name: "fi"; MessagesFile: "Installer\Languages\Finnish.isl"; LicenseFile: "Installer/License/Finnish.rtf"
Name: "fr"; MessagesFile: "Installer\Languages\French.isl"; LicenseFile: "Installer/License/French.rtf"
Name: "gr"; MessagesFile: "Installer\Languages\Greek.isl"; LicenseFile: "Installer/License/Greek.rtf"
Name: "heb"; MessagesFile: "Installer\Languages\Hebrew.isl"; LicenseFile: "Installer/License/Hebrew.rtf"
Name: "hu"; MessagesFile: "Installer\Languages\Hungarian.isl"; LicenseFile: "Installer/License/Hungarian.rtf"
Name: "it"; MessagesFile: "Installer\Languages\Italian.isl"; LicenseFile: "Installer/License/Italian.rtf"
Name: "lt"; MessagesFile: "Installer\Languages\Lithuanian.isl"; LicenseFile: "Installer/License/Lithuanian.rtf"
Name: "nl"; MessagesFile: "Installer\Languages\Dutch.isl"; LicenseFile: "Installer/License/Dutch.rtf"
Name: "no"; MessagesFile: "Installer\Languages\Norwegian.isl"; LicenseFile: "Installer/License/Norwegian.rtf"
Name: "pl"; MessagesFile: "Installer\Languages\Polish.isl"; LicenseFile: "Installer/License/Polish.rtf"
Name: "pt"; MessagesFile: "Installer\Languages\Portuguese.isl"; LicenseFile: "Installer/License/Portuguese.rtf"
Name: "pt_br"; MessagesFile: "Installer\Languages\Portuguese-Brazil.isl"; LicenseFile: "Installer/License/Portuguese.rtf"
Name: "ru"; MessagesFile: "Installer\Languages\Russian.isl"; LicenseFile: "Installer/License/Russian.rtf"
Name: "sl"; MessagesFile: "Installer\Languages\Slovenian.isl"; LicenseFile: "Installer/License/Default.rtf"
Name: "sq"; MessagesFile: "Installer\Languages\Albanian.isl"; LicenseFile: "Installer/License/Albanian.rtf"
Name: "sr"; MessagesFile: "Installer\Languages\Serbian.isl"; LicenseFile: "Installer/License/Serbian.rtf"
Name: "sv"; MessagesFile: "Installer\Languages\Swedish.isl"; LicenseFile: "Installer/License/Swedish.rtf"
Name: "tr"; MessagesFile: "Installer\Languages\Turkish.isl"; LicenseFile: "Installer/License/Turkish.rtf"
Name: "zhs"; MessagesFile: "Installer\Languages\Chinese-Simple.isl"; LicenseFile: "Installer/License/Chinese-Simple.rtf"
Name: "zht"; MessagesFile: "Installer\Languages\Chinese-Traditional.isl"; LicenseFile: "Installer/License/Chinese-Traditional.rtf"
Name: "ja"; MessagesFile: "Installer\Languages\Japanese.isl"; LicenseFile: "Installer/License/Japanese.rtf"
Name: "ko"; MessagesFile: "Installer\Languages\Korean.isl"; LicenseFile: "Installer/License/Korean.rtf"

[Files]
; Install all languages if selected
Source: "Languages\default-*.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Tasks: language; Excludes: "default-en.xml"
Source: "Languages\default-*.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Tasks: language

; Install just localized language otherwise
; Brittish-English
Source: "Languages\default-alt.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: en_uk; Tasks: not language
Source: "Languages\default-alt.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: en_uk; Tasks: not language
; Arabic
Source: "Languages\default-ar.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ar; Tasks: not language
Source: "Languages\default-ar.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ar; Tasks: not language
; Catalan
Source: "Languages\default-ca.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ca; Tasks: not language
Source: "Languages\default-ca.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ca; Tasks: not language
; Czech
Source: "Languages\default-cz.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: cz; Tasks: not language
Source: "Languages\default-cz.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: cz; Tasks: not language
; German
Source: "Languages\default-de.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: de; Tasks: not language
Source: "Languages\default-de.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: de; Tasks: not language
; Estonian
Source: "Languages\default-ee.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ee; Tasks: not language
Source: "Languages\default-ee.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ee; Tasks: not language
; Spanish std
Source: "Languages\default-es.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es; Tasks: not language
Source: "Languages\default-es.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es; Tasks: not language
; Spanish Mexican
Source: "Languages\default-es-mx.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es_mx; Tasks: not language
Source: "Languages\default-es-mx.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es_mx; Tasks: not language
; French
Source: "Languages\default-fr.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fr; Tasks: not language
Source: "Languages\default-fr.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fr; Tasks: not language
; Finnish
Source: "Languages\default-fi.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fi; Tasks: not language
Source: "Languages\default-fi.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fi; Tasks: not language
; Greek
Source: "Languages\default-gr.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: gr; Tasks: not language
Source: "Languages\default-gr.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: gr; Tasks: not language
; Hebrew
Source: "Languages\default-heb.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: heb; Tasks: not language
Source: "Languages\default-heb.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: heb; Tasks: not language
; Hungarian
Source: "Languages\default-hu.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: hu; Tasks: not language
Source: "Languages\default-hu.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: hu; Tasks: not language
; Italian
Source: "Languages\default-it.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: it; Tasks: not language
Source: "Languages\default-it.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: it; Tasks: not language
; Lithuanian
Source: "Languages\default-lt.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: lt; Tasks: not language
Source: "Languages\default-lt.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: lt; Tasks: not language
; Dutch
Source: "Languages\default-nl.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: nl; Tasks: not language
Source: "Languages\default-nl.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: nl; Tasks: not language
; Norwegian
Source: "Languages\default-no.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: no; Tasks: not language
Source: "Languages\default-no.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: no; Tasks: not language
; Polish
Source: "Languages\default-pl.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pl; Tasks: not language
Source: "Languages\default-pl.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pl; Tasks: not language
; Portuguese std
Source: "Languages\default-pt-br.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt; Tasks: not language
Source: "Languages\default-pt-br.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt; Tasks: not language
; Portuguese Braz
Source: "Languages\default-pt-br.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt_br; Tasks: not language
Source: "Languages\default-pt-br.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt_br; Tasks: not language
; Russian
Source: "Languages\default-ru.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ru; Tasks: not language
Source: "Languages\default-ru.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ru; Tasks: not language
; Slovenian
Source: "Languages\default-sl.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sl; Tasks: not language
Source: "Languages\default-sl.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sl; Tasks: not language
; Albanian
Source: "Languages\default-sq.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sq; Tasks: not language
Source: "Languages\default-sq.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sq; Tasks: not language
; Serbian
Source: "Languages\default-sr.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sr; Tasks: not language
Source: "Languages\default-sr.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sr; Tasks: not language
; Swedish
Source: "Languages\default-sv.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sv; Tasks: not language
Source: "Languages\default-sv.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sv; Tasks: not language
; Turkish
Source: "Languages\default-tr.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: tr; Tasks: not language
Source: "Languages\default-tr.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: tr; Tasks: not language
; Afrikaans
Source: "Languages\default-za.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: af; Tasks: not language
Source: "Languages\default-za.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: af; Tasks: not language
; Japanese
Source: "Languages\default-ja.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ja; Tasks: not language
Source: "Languages\default-ja.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ja; Tasks: not language
; Korean
Source: "Languages\default-ko.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ja; Tasks: not language
Source: "Languages\default-ko.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ja; Tasks: not language
; Chinese Simp
Source: "Languages\default-zhs.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zhs; Tasks: not language
Source: "Languages\default-zhs.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zhs; Tasks: not language
; Chinese Trad
Source: "Languages\default-zht.xml"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zht; Tasks: not language
Source: "Languages\default-zht.ico"; DestDir: "{app}\Skins\Languages"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zht; Tasks: not language


; Common remote files
Source: "Remote\Common\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension recursesubdirs; Excludes: ".svn"
Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"

; Install localized remote otherwise
; English-UK
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: en_uk
; Czech
Source: "Remote\cz\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: cz
; German
Source: "Remote\de\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: de
; Spanish std
Source: "Remote\es\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: es
; Spanish Mexican
Source: "Remote\es\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: es_mx
; French
Source: "Remote\fr\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: fr
; Greek
Source: "Remote\gr\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: gr
; Hungarian
Source: "Remote\hu\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: hu
; Italian
Source: "Remote\it\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: it
; Lithuanian
Source: "Remote\lt\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: lt
; Dutch
Source: "Remote\nl\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: nl
; Norwegian
Source: "Remote\no\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: no
; Portuguese std
Source: "Remote\pt-br\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: pt
; Portuguese Braz
Source: "Remote\pt-br\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: pt_br
; Russian
Source: "Remote\ru\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: ru
; Swedish
Source: "Remote\sv\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: sv
; Albanian
Source: "Remote\sq\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: sq
; Serbian
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: sr
; Turkish
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: tr
; Slovenian
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: sl
; Catalan
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: ca
; Polish
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: pl
; Hebrew
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: heb
; Korean
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: ko
; Japanese
Source: "Remote\ja\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: ja
; Chinese Simp
Source: "Remote\chs\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: zhs
; Chinese Trad
Source: "Remote\chs\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: zht


; Common License file
Source: "Installer\License\Default.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension

; Install localized license
; English-UK
;Source: "Installer\License\Default.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: en_uk
; Dutch
Source: "Installer\License\Dutch.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: nl
; Lithuanian
Source: "Installer\License\Lithuanian.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: lt
; German
Source: "Installer\License\German.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: de
; Italian
Source: "Installer\License\Italian.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: it
; Norwegian
Source: "Installer\License\Norwegian.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: no
; Afrikaans
Source: "Installer\License\Afrikaans.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: af
; Portuguese std
Source: "Installer\License\Portuguese.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt
; Portuguese Braz
Source: "Installer\License\Portuguese.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt_br
; French
Source: "Installer\License\French.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fr
; Spanish std
Source: "Installer\License\Spanish.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es
; Spanish Mexican
Source: "Installer\License\Spanish.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es_mx
; Russian
Source: "Installer\License\Russian.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ru
; Greek
Source: "Installer\License\Greek.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: gr
; Hungarian
Source: "Installer\License\Hungarian.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: hu
; Swedish
Source: "Installer\License\Swedish.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sv
; Finnish
Source: "Installer\License\Finnish.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fi
; Hebrew
Source: "Installer\License\Hebrew.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: heb
; Polish
Source: "Installer\License\Polish.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pl
; Czech
Source: "Installer\License\Czech.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: cz
; Serbian
Source: "Installer\License\Serbian.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sr
; Turkish
Source: "Installer\License\Turkish.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: tr
; Japanese
Source: "Installer\License\Japanese.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ja
; Arabic
Source: "Installer\License\Arabic.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ar
; Estonian
Source: "Installer\License\Estonian.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ee
; Chinese Simp
Source: "Installer\License\Chinese-Simple.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zhs
; Chinese Trad
Source: "Installer\License\Chinese-Traditional.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zht
; Slovenian
Source: "Installer\License\Default.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sl
; Catalan
Source: "Installer\License\Catalan.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ca
; Catalan
Source: "Installer\License\Albanian.rtf"; DestDir: "{app}\Uninstall"; DestName: "AGPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sq


; Install default filter
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension

; Install localized filter
; Brittish-English
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: en_uk
; Dutch
Source: "Installer\Filter\Dutch.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: nl
; Lithuanian
Source: "Installer\Filter\Lithuanian.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: lt
; German
Source: "Installer\Filter\German.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: de
; Italian
Source: "Installer\Filter\Italian.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: it
; Norwegian
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: no
; Afrikaans
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: af
; Portuguese std
Source: "Installer\Filter\Portuguese-br.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt
; Portuguese Braz
Source: "Installer\Filter\Portuguese-br.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt_br
; French
Source: "Installer\Filter\French.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fr
; Spanish std
Source: "Installer\Filter\Spanish-Mexican.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es
; Spanish Mexican
Source: "Installer\Filter\Spanish-Mexican.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es_mx
; Russian
Source: "Installer\Filter\Russian.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ru
; Greek
Source: "Installer\Filter\Greek.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: gr
; Hungarian
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: hu
; Swedish
Source: "Installer\Filter\Swedish.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sv
; Finnish
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fi
; Hebrew
Source: "Installer\Filter\Hebrew.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: heb
; Polish
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pl
; Czech
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: cz
; Serbian
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sr
; Arabic
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ar
; Estonian
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ee
; Slovenian
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sl
; Albanian
Source: "Installer\Filter\Albanian.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sq
; Turkish
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: tr
; Japanese
Source: "Installer\Filter\AdultFilter.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ja
; Catalan
Source: "Installer\Filter\Catalan.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ca
; Chinese Simp
Source: "Installer\Filter\Chinese-Simpl.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zhs
; Chinese Trad
Source: "Installer\Filter\Chinese-Simpl.dat"; DestDir: "{app}\Data"; DestName: "AdultFilter.dat"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zht


[Messages]
; This section overrides existing phrases specified in Inno's Default.isl file

en.SelectLanguageLabel=%nSelect default language:
en.WelcomeLabel1=%nWelcome to the%n[name] Setup Wizard
en.WelcomeLabel2=This will install [name/ver]%nP2P filesharing on your computer.%n%nIt is recommended to close applications before continuing.
en.FinishedHeadingLabel=%nCompleting the [name]%nInstaller Wizard
en.FinishedLabel=Setup has finished installing [name] on your computer.%nIt can be launched from the installed shortcut icons.
en.LicenseLabel3=Please read the following license summary.%nYou should understand the basic terms of this License before continuing installation.


[CustomMessages]
; This section specifies phrases and words not specified in the ISL files
; Avoid customizing the ISL files since they will change with each version of Inno Setup.

; English Default
components_plugins=Plugins
components_skins=Skins
tasks_allusers=All users
tasks_selectusers=Install %1 for:
tasks_currentuser=%1 only
tasks_languages=Install multi-language support
tasks_multisetup=Setup Windows multi-user data
;tasks_upnp=Enable Plug'n'Play port-forwarding
tasks_firewall=Add an exception to the Windows Firewall
tasks_webhook=Enable web browser integration
tasks_resetdiscoveryhostcache=Reset network sources  (Discovery/Hostcache)
tasks_deleteoldsetup=Delete old installers
run_skinexe=Running skin installation...
reg_incomingchat=Incoming chat message
reg_apptitle=PeerProject P2P Filesharing
icons_license=AGPL License
icons_uninstall=Uninstall
icons_downloads=Downloads
icons_basicmode=Basic Mode
icons_tabbedmode=Tabbed Mode
icons_windowedmode=Windowed Mode
icons_launchtray=Tray Launch
;icons_noskin=No Skins
CreateQuickLaunchIcon=Show PeerProject &quick launch icon
CreateDesktopIcon=Show PeerProject &desktop icon
CreateDesktopIconWizard=Show TorrentWizard &desktop icon
PathNotExist=Error, the path of the %1 folder doesn't exist
dialog_shutdown=%1 is currently running. Would you like %1 to be shutdown so the installation can continue?
dialog_firewall=Setup failed to add PeerProject to the Windows Firewall.%nPlease add PeerProject to the exception list manually.
dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject.  Do you wish to exit now?
page_safetywarning_title=Safety Awareness
page_safetywarning_subtitle=Do you have an AntiVirus program running?
page_safetywarning_text=Filesharing is a home server with browser to find others like you.%n%nWhen downloading, always have an updated virus scanner to protect yourself from malicious files.  Be aware that anti-p2p companies will return fake results to searches, but can easily be banned.%n%nWhen uploading, recognize Permissive and Restrictive material.%nSharing unlicensed copyright work may be liable in some countries.%nThe best policy is to boycott big corporate copyright-abusers entirely:%nSimply don't promote those who won't appreciate it, many others do.%n
; Don't copy these last 2 messages, they are just links.
page_safetywarning_link=http://security.peerproject.org
page_safetywarning_destination=http://security.peerproject.org

; English Alt Spellings
;en_uk.

; Afrikaans
af.components_plugins=Inpropprogramme
af.components_skins=Omslagte
af.tasks_languages=Tale
af.tasks_allusers=Alle gebruikers van hierdie rekenaar
af.tasks_selectusers=Installeer %1 vir die volgende gebruikers:
af.tasks_currentuser=Vir %1 alleenlik
af.tasks_multisetup=Skakel ondersteuning vir veelvuldige gebruikers aan
;af.tasks_upnp=Enable Plug'n'Play port-forwarding
af.tasks_firewall=Voeg 'n uitsondering by die Windows Netskans
af.tasks_webhook=Aktiveer webblaaier integrasie
af.tasks_resetdiscoveryhostcache=Herstel netwerk bronne  (Ontdekking / Gasheer kas)
af.tasks_deleteoldsetup=Skrap ou opstellersl�ers
af.run_skinexe=Hardloop omslagte installasie...
af.reg_incomingchat=Inkomende Geselsie-boodskap
af.reg_apptitle=PeerProject P2P Filesharing
af.icons_license=AGPL Lisensie-ooreenkoms
af.icons_uninstall=De�nstalleer
af.icons_downloads=Aflaaie
af.icons_basicmode=Basic Modus
af.icons_tabbedmode=Tabelmodus
af.icons_windowedmode=Venstermodus
af.dialog_shutdown=%1 is op die oomblik besig om te loop. Wil jy graag %1 sluit sodat die installasie kan voortgaan?
af.dialog_firewall=Die Opsteller kon nie PeerProject by die Windows netskans uitsonderings voeg nie.%nVoeg PeerProject asseblief met die hand op die uitsonderingslys.
af.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
af.page_safetywarning_text=Maak altyd seker dat jy 'n opgedateerde anti-virus program ge�nstalleer het wanneer jy die internet gebruik, om jou rekenaar te beskerm teen virusse, wurms, en ander ongewenste programme. Jy kan 'n lys van goeie anti-virus programme en ander sekuriteitswenke oor hoe om jou rekenaar te beskerm verkry deur die volgende skakel te volg:
af.page_safetywarning_title=Virus Waarskuwing
af.page_safetywarning_subtitle=Het jy 'n Anti-Virus program ge�nstalleer?
af.CreateDesktopIcon=Vertoon 'n &werkskerm ikoon
af.CreateQuickLaunchIcon=Vertoon 'n &Quick Launch ikoon
; Arabic
ar.components_plugins=������� ��������
ar.components_skins=��������
ar.tasks_languages=������
ar.tasks_allusers=���� ����������
ar.tasks_selectusers=�� ��� %1 �����:
ar.tasks_currentuser=%1 ���
ar.tasks_multisetup=����� ����� ���� ��������
;ar.tasks_upnp=Enable Plug'n'Play port-forwarding
ar.tasks_firewall=����� ������� ��� ���� �������� ������
;ar.tasks_webhook=����� ����� ������� ��� ���� ��������
ar.tasks_resetdiscoveryhostcache=����� ����� ����� ������ (������ / ������)
ar.tasks_deleteoldsetup=��� ����� ������� �������
ar.run_skinexe=...��� ����� ����� ������
ar.reg_incomingchat=����� ������ �����
ar.reg_apptitle=���-��� �������� ����� ������ ��������
ar.icons_license=AGPL ������
ar.icons_uninstall=����� �������
ar.icons_downloads=���������
ar.icons_basicmode=���� ����
ar.icons_tabbedmode=���� �����
ar.icons_windowedmode=���� �� �����
ar.dialog_shutdown=%1 ���� ����� . �� ���� ����� %1 ������ ������� �
ar.dialog_firewall=��� ������� �� ����� ���-��� ��� ������ ����� �������� %n������ ����� ���-��� ��� ����� ����������� �����
ar.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
ar.page_safetywarning_text=����� ������� �������� � ��� �� ����� �� ���� ���� ������ ������� ���� . ����� ������ ��� ����� ��������� � ����� ����� ���� ������ �������� �� ��� ������:
ar.page_safetywarning_title=����� �� ���������
ar.page_safetywarning_subtitle=�� ���� ������ ��������� �
ar.CreateDesktopIcon=����� &������ ��� ������
ar.CreateQuickLaunchIcon=����� &������ ������� ������
; Catalan
ca.components_plugins=Agregats (plug-in)
ca.components_skins=Pells (skins)
ca.tasks_languages=Idiomes
ca.tasks_allusers=Tots els usuaris
ca.tasks_selectusers=Instal�lar %1 per a:
ca.tasks_currentuser=%1 �nicament
ca.tasks_multisetup=Habilita el suport multi-usuari
;ca.tasks_upnp=Habilita el descobriment de dispositius Plug'n'Play
ca.tasks_firewall=Afegeix una excepci� al tallafocs del Windows
ca.tasks_webhook=Permetre la integraci� del navegador web
ca.tasks_resetdiscoveryhostcache=Reestableix els serveis de descobriment i cax� de servidors
ca.tasks_deleteoldsetup=Esborra instal�lacions antigues
ca.run_skinexe=Executant la instal�laci� de la pell (skin)...
ca.reg_incomingchat=Missatge de xat entrant
ca.reg_apptitle=PeerProject: compartici� d'arxius d'�ltima generaci�
ca.icons_license=AGPL Llic�ncia
ca.icons_uninstall=Desinstal�laci�
ca.icons_downloads=Desc�rregues
ca.icons_basicmode=Mode basico
ca.icons_tabbedmode=Mode en pestanyes
ca.icons_windowedmode=Mode de finestres
ca.dialog_shutdown=%1 est� sent executat. Dessitja que %1 siga aturat per que la instal�laci� puga continuar?
ca.dialog_firewall=La instal�laci� ha fallat mentre s'afegia una exepci� al tallafocs del Windows.%nSi us plau, afegeix PeerProject al llistat d'excepcions manualment.
ca.dialog_malwaredetected=PeerProject ha detectat un arxiu malici�s (malware) a %1. Si us plau, elimine'l amb un antivirus abans d'instal�lar. Vol sortir de la instal�laci�?
ca.page_safetywarning_text=Mentre utilitzes Internet, has d'assegurar-te que tens un antivirus actualitzat per a protegir-te de troians, cucs, virus i altres programes maliciosos. Pots consultar un llistat de programari antivirus i consells de seguretat fent clic a la seg�ent drecera:
ca.page_safetywarning_title=Advert�ncia de virus
ca.page_safetywarning_subtitle=Tens un programa antiv�ric instal�lat?
ca.CreateDesktopIcon=Afegeix una icona a l'&escriptori
ca.CreateQuickLaunchIcon=Afegeix una icona a la barra d'Acc�s R�pid
ca.PathNotExist=Error: la carpeta %1 no existeix.
; Czech
cz.components_plugins=Dopl�ky
cz.components_skins=Skiny
cz.tasks_languages=V�ce jazyk�
cz.tasks_allusers=v�echny u�ivatele
cz.tasks_selectusers=Instalovat %1 pro:
cz.tasks_currentuser=jen %1
cz.tasks_multisetup=Povolit p��stup v�ce u�ivatel�
;cz.tasks_upnp=Povolit zji��ov�n� Plug'n'Play za��zen�
cz.tasks_firewall=P�idat pravidlo do Windows firewallu
cz.tasks_webhook=Povolit webov� prohl�ec integrace
cz.tasks_resetdiscoveryhostcache=Obnovit seznamy server�
cz.tasks_deleteoldsetup=Smazat star� instal�tory
cz.run_skinexe=Spou�t�m instalaci skin�...
cz.reg_incomingchat=P��choz� zpr�va chatu
cz.reg_apptitle=PeerProject P2P Filesharing
cz.icons_license=AGPL Licence
cz.icons_uninstall=Odinstalovat
cz.icons_downloads=Sta�en� soubory
cz.icons_basicmode=Basc�c re�im
cz.icons_tabbedmode=Re�im z�lo�ek
cz.icons_windowedmode=Re�im oken
cz.dialog_shutdown=Aplikace %1 je spu�t�na. Chcete ukon�it %1 a pokra�ovat v instalaci?
cz.dialog_firewall=Instal�tor nem��e p�idat pravidlo pro PeerProject do nastaven� Windows Firewallu.%nPros�m p�idejte pravidlo ru�n�.
cz.dialog_malwaredetected=Byl nalezen �kodliv� software (malware) v %1. P�ed pokra�ov�n� instalace jej pros�m odstra�te pomoc� vhodn�ho n�stroje pro odstra�ov�n� �kodliv�ho softwaru. Ukon�it instalaci?
cz.page_safetywarning_text=P�i pou��v�n� internetu se v�dy ujist�te, �e m�te aktu�ln� antivirov� program, kter� v�s chr�n� p�ed nebezpe�n�mi a �kodliv�mi programy. Seznam vhodn�ch program� a dal�� rady t�kaj�c� se bezpe�nosti m��ete nal�zt na n�sleduj�c� adrese:
cz.page_safetywarning_title=Nalezen virus
cz.page_safetywarning_subtitle=M�te nainstalovan� antivirov� program?
cz.CreateDesktopIcon=Zobrazovat ikonu na &plo�e
cz.CreateQuickLaunchIcon=Zobrazovat ikonu v panelu &Snadn� spou�t�n�
; German
de.components_plugins=Plugins
de.components_skins=Skins
de.tasks_languages=Sprachen
de.tasks_allusers=Alle Benutzer
de.tasks_selectusers=Installieren %1 f�r:
de.tasks_currentuser=Nur f�r %1
de.tasks_multisetup=Mehrbenutzerunterst�tzung aktivieren
;de.tasks_upnp=Automatische Routerkonfiguration benutzen (Plug'n'Play)
de.tasks_firewall=Eine Ausnahme zur Windows-Firewall hinzuf�gen
de.tasks_webhook=Aktivieren Sie Web Browser-Integration
de.tasks_resetdiscoveryhostcache=Service-Provider und Host-Cache zur�cksetzen
de.tasks_deleteoldsetup=Alte Installer l�schen
de.run_skinexe=SkinInstaller einrichten...
de.reg_incomingchat=Eingehende Chatnachricht
de.reg_apptitle=PeerProjekt P2P
de.icons_license=AGPL Lizenz
de.icons_uninstall=PeerProjekt deinstallieren
de.icons_downloads=Downloads
de.icons_basicmode=Basicmodus
de.icons_tabbedmode=Tab-Modus
de.icons_windowedmode=Fenstermodus
de.dialog_shutdown=%1 wird zur Zeit ausgef�hrt. Wollen Sie %1 beenden, um mit der Installation fortzufahren?
de.dialog_firewall=Setup konnte PeerProjekt nicht zur Windows-Firewall hinzuf�gen.%nBitte tragen Sie PeerProjekt manuell in die Ausnahmenliste ein.
de.dialog_malwaredetected=Auf ihrem Computer wurde sch�dliche Software im Ordner %1 entdeckt. Bitte entfernen Sie die Software mit Hilfe eines Viren- bzw. Malware-Scanners bevor Sie mir der Installation von PeerProjekt fortfahren.M�chten sie die Installation jetzt abbrechen?
de.page_safetywarning_text=Wenn Sie das Internet benutzen, sollten Sie stets einen aktuellen Virenscanner installiert haben, der ihren Computer vor Trojanern, W�rmern und anderen b�sartigen Programmen besch�tzt. Sie finden eine Liste guter Virenscanner und andere Tipps, wie Sie ihren Computer sch�tzen k�nnen, unter folgendem Link:
de.page_safetywarning_title=Virenwarnung
de.page_safetywarning_subtitle=Haben Sie ein Antivirenprogramm installiert?
de.CreateDesktopIcon=&Symbol auf dem Desktop erstellen
de.CreateQuickLaunchIcon=&PeerProjekt zur Schnellstartleiste hinzuf�gen
de.PathNotExist=Fehler: der Pfad zum %1-Ordner existiert nicht
; Estonian
ee.components_plugins=Pluginad
ee.components_skins=Nahad
ee.tasks_languages=Keeled
ee.tasks_allusers=K�ik kasutajad
ee.tasks_selectusers=Installi %1 jaoks:
ee.tasks_currentuser=%1 ainult
ee.tasks_multisetup=V�imalda mitmekasutaja tugi
;ee.tasks_upnp=Enable Plug'n'Play port-forwarding
ee.tasks_firewall=Lisa erand Windowsi Tulem��ri
ee.tasks_webhook=Luba brauseri integratsioon
ee.tasks_resetdiscoveryhostcache=Taasta v�rgu allikatest  (Avastus/Puhvri vahem�lu)
ee.tasks_deleteoldsetup=Kustuta vanad installerid
ee.run_skinexe=K�ivitan Naha installi...
ee.reg_incomingchat=Sisse tulev vestlusteade
ee.reg_apptitle=PeerProject Failijagamine
ee.icons_license=AGPL Litsents
ee.icons_uninstall=Uninstalli
ee.icons_downloads=T�mbamised
ee.icons_basicmode=Tavaline Vaade
ee.icons_tabbedmode=Sakiline Vaade
ee.icons_windowedmode=Akendega Vaade
ee.dialog_shutdown=%1 t��tab hetkel. Kas tahad  %1 sulgeda, et saaksid installeerimist j�tkata?
ee.dialog_firewall=Installeril eba�nnestus PeerProject lisamine Windowsi Tulem��ri.%Palun lisa PeerProject k�sitsi erandite nimekirja.
ee.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
ee.page_safetywarning_text=Internetti kasutades peaksid kontrollima, et sul oleks uusim viiruset�rje, et kaitsta ennast troojalaste, usside, viiruste ja teiste kahjulike programmide eest. Sa leiad nimekirja headest viirus sk�nneritest ja teisi turva n�uandeid oma arvuti kaitseks sellelt lingilt:
ee.page_safetywarning_title=Viiruse Hoiatus
ee.page_safetywarning_subtitle=Kas sul on AntiVirus programm installeeeritud?
ee.CreateDesktopIcon=Loo &T��laua ikoon
ee.CreateQuickLaunchIcon=Loo &Quick Launch ikoon
; Spanish
es.components_plugins=Plugins
es.components_skins=Skins
es.tasks_languages=Instalar multi-idiomas
es.tasks_allusers=Todos los usuarios
es.tasks_selectusers=Instalar %1 para:
es.tasks_currentuser=%1 solamente
es.tasks_multisetup=Habilitar soporte multi-usuario
;es.tasks_upnp=Habilitar reenv�o de puertos Plug'n'Play
es.tasks_firewall=Agregar una excepci�n al Firewall de Windows
es.tasks_webhook=Permitir la integraci�n del navegador web
es.tasks_resetdiscoveryhostcache=Resetear las fuentes de red  (Descubrimiento/Hosts)
es.tasks_deleteoldsetup=Borrar archivos de instaladores viejos
es.run_skinexe=Instalando Skin...
es.reg_incomingchat=Hay un mensaje de chat entrante
es.reg_apptitle=PeerProject P2P Filesharing
es.icons_license=AGPL Licencia
es.icons_uninstall=Desinstalar
es.icons_downloads=Descargas
es.icons_basicmode=Modo B�sico
es.icons_tabbedmode=Modo Avanzado
es.icons_windowedmode=Modo Ventanas
es.dialog_shutdown=%1 se encuentra ejecut�ndose. �Deseas que %1 sea cerrado para que la instalaci�n pueda continuar?
es.dialog_firewall=La instalaci�n fallo al agregar la excepci�n de PeerProject al cortafuego Firewall.%n Por favor h�galo manualmente.
es.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
es.page_safetywarning_text=Cuando estas usando Internet, debes siempre asegurarte que tienes un antivirus actualizado hasta la fecha para protegerte de troyanos, gusanos, y otros programas maliciosos. Puedes encontrar una lista de buenos antivirus y sugerencias de seguridad para proteger tu computadora en la siguiente direcci�n:
es.page_safetywarning_title=Peligro de Virus
es.page_safetywarning_subtitle=�Tienes un programa antivirus instalado?
es.CreateDesktopIcon=Mostrar icono de &Escritorio
es.CreateQuickLaunchIcon=Mostrar icono de &Inicio R�pido
; Spanish Mexican
es_mx.components_plugins=Plugins
es_mx.components_skins=Skins
es_mx.tasks_languages=Instalar multi-idiomas
es_mx.tasks_allusers=Todos los usuarios
es_mx.tasks_selectusers=Instalar %1 para:
es_mx.tasks_currentuser=%1 solamente
es_mx.tasks_multisetup=Habilitar soporte multi-usuario
;es_mx.tasks_upnp=Habilitar reenv�o de puertos Plug'n'Play
es_mx.tasks_firewall=Agregar una excepci�n al Firewall de Windows
es_mx.tasks_webhook=Permitir la integraci�n del navegador web
es_mx.tasks_resetdiscoveryhostcache=Resetear las fuentes de red  (Descubrimiento/Hosts)
es_mx.tasks_deleteoldsetup=Borrar archivos de instaladores viejos
es_mx.run_skinexe=Instalando Skin...
es_mx.reg_incomingchat=Hay un mensaje de chat entrante
es_mx.reg_apptitle=PeerProject P2P Filesharing
es_mx.icons_license=AGPL Licencia
es_mx.icons_uninstall=Desinstalar
es_mx.icons_downloads=Descargas
es_mx.icons_basicmode=Modo B�sico
es_mx.icons_tabbedmode=Modo Avanzado
es_mx.icons_windowedmode=Modo Ventanas
es_mx.dialog_shutdown=%1 se encuentra ejecut�ndose. �Deseas que %1 sea cerrado para que la instalaci�n pueda continuar?
es_mx.dialog_firewall=La instalaci�n fallo al agregar la excepci�n de PeerProject al cortafuego Firewall.%n Por favor h�galo manualmente.
es_mx.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
es_mx.page_safetywarning_text=Cuando estas usando Internet, debes siempre asegurarte que tienes un antivirus actualizado hasta la fecha para protegerte de troyanos, gusanos, y otros programas maliciosos. Puedes encontrar una lista de buenos antivirus y sugerencias de seguridad para proteger tu computadora en la siguiente direcci�n:
es_mx.page_safetywarning_title=Peligro de Virus
es_mx.page_safetywarning_subtitle=�Tienes un programa antivirus instalado?
es_mx.CreateDesktopIcon=Mostrar icono de &Escritorio
es_mx.CreateQuickLaunchIcon=Mostrar icono de &Inicio R�pido
; French
fr.components_plugins=Plugins
fr.components_skins=Skins
fr.tasks_languages=Langues
fr.tasks_allusers=Tous les utilisateurs
fr.tasks_selectusers=Installer %1 pour:
fr.tasks_currentuser=%1 seulement
fr.tasks_multisetup=Activer le support multi-utilisateurs
;fr.tasks_upnp=Activer UPnP pour essayer de d�couvrir les pare-feux/routeurs.
fr.tasks_firewall=Ajouter une exception au Pare-feu Windows
fr.tasks_webhook=Activer l'int�gration au navigateur Web
fr.tasks_resetdiscoveryhostcache=Remettre � z�ro les services de d�couverte et le tampon pour les h�tes
fr.tasks_deleteoldsetup=Voulez-vous supprimer les anciens fichiers d'installation de PeerProject ?
fr.run_skinexe=Installation de la skin en cours...
fr.reg_incomingchat=R�ception d'un message chat
fr.reg_apptitle=PeerProject P2P Filesharing
fr.icons_license=AGPL Licence
fr.icons_uninstall=D�sinstaller
fr.icons_downloads=T�l�chargements
fr.icons_basicmode=Mode simple
fr.icons_tabbedmode=Mode tabul�
fr.icons_windowedmode=Mode fen�tr�
fr.dialog_shutdown=%1 est en cours d'ex�cution. Voulez-vous quitter %1 pour que l'installation puisse se poursuivre?
fr.dialog_firewall=L'installation n'a pas pu ajouter PeerProject au Pare-feu Windows.%nVeuillez ajouter PeerProject manuellement � la liste des exceptions.
fr.dialog_malwaredetected=Un logiciel malveillant a �t� d�tect� sur votre syst�me � %1, veuillez l'enlever avec un antivirus avant d'installer PeerProject. Voulez-vous quitter maintenant ?
fr.page_safetywarning_text=Lorsque vous utilisez internet, vous devriez toujours vous assurer que vous avez un antivirus � jour pour vous prot�ger des troyens (trojans), vers (worms), et autres programmes malveillants. Vous pouvez trouver une liste de bons antivirus et des conseils de s�curit� pour prot�ger votre ordinateur en suivant ce lien :
fr.page_safetywarning_title=Avertissement sur les virus
fr.page_safetywarning_subtitle=Avez-vous un antivirus install�?
fr.CreateDesktopIcon=Afficher un raccourci sur le &Bureau
fr.CreateQuickLaunchIcon=Afficher un raccourci dans la barre de &Lancement rapide
; Finnish
fi.components_plugins=Laajennukset
fi.components_skins=Ulkoasut
fi.tasks_languages=Kielet
fi.tasks_allusers=Kaikille k�ytt�jille
fi.tasks_selectusers=Asenna %1 k�ytt�jille:
fi.tasks_currentuser=%1 vain
fi.tasks_multisetup=Asenna kaikille koneen k�ytt�jille
;fi.tasks_upnp=Enable Plug'n'Play port-forwarding
fi.tasks_firewall=Lis�� poikkeus Windowsin palomuuriin
fi.tasks_webhook=Ota selaimen integrointi
fi.tasks_resetdiscoveryhostcache=Palauta verkon l�hteet (L�yt�/Hosts)
fi.tasks_deleteoldsetup=Poista vanhat asennusohjelmat
fi.run_skinexe=K�ynniss� ulkoasujen asennus...
fi.reg_incomingchat=Tuleva keskusteluviesti
fi.reg_apptitle=PeerProject jako-ohjelma
fi.icons_license=AGPL Lisenssi
fi.icons_uninstall=Poista
fi.icons_downloads=Lataukset
fi.icons_basicmode=Basicaali Tila
fi.icons_tabbedmode=V�lilehti Tila
fi.icons_windowedmode=Ikkunoitu Tila
fi.dialog_shutdown=%1 on t�ll� hetkell� k�ynniss�. Haluatko ett� %1 suljetaan jotta asennus voisi jatkua?
fi.dialog_firewall=Asentaja ep�onnistui lis�tess��n PeerProjecta Windowsiin Firewall.%nOle hyv� ja lis�� PeerProject poikkeuslistaan manuaalisesti.
fi.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
fi.page_safetywarning_text=Kun k�yt�t interneti�, sinun tulee aina varmistaa ett� sinulla on viimeisimm�t p�ivitykset virusohjelmissasi jotka suojaavat sinua troijalaisilta, madoilta, ja muilta haittaohjelmilta. L�yd�t hyv�n listan hyvist� virusohjelmista ja turvallisuusvinkkej� seuraavista linkeist�:
fi.page_safetywarning_title=Virus Varoitus
fi.page_safetywarning_subtitle=Onko sinulla AntiVirus ohjelmaa asennettuna?
fi.CreateDesktopIcon=Luo kuvake ty�p�yd�lle
fi.CreateQuickLaunchIcon=Luo kuvake pikak�ynnistyspalkkiin
; Greek
gr.components_plugins=Plugins
gr.components_skins=Skins
gr.tasks_languages=�������
gr.tasks_allusers=���� �� �������
gr.tasks_selectusers=����������� %1 ���:
gr.tasks_currentuser=%1 ����
gr.tasks_multisetup=������������ ��� �������� ��������� �������
;gr.tasks_upnp=Enable Plug'n'Play port-forwarding
gr.tasks_firewall=���� ��� �������� ��� ������ ���������� ��� Windows
;gr.tasks_webhook=������������ ������������ ���������� web ������
gr.tasks_resetdiscoveryhostcache=��������� ����� ��� ������� (���������/�����������)
gr.tasks_deleteoldsetup=�������� ��� ����� �����������
gr.run_skinexe=Running ����������� ��� skin...
gr.reg_incomingchat=��� ������ chat
gr.reg_apptitle=PeerProject P2P Filesharing
gr.icons_license=AGPL �����
gr.icons_uninstall=�������������
gr.icons_downloads=���������
gr.icons_basicmode=�������� �����
gr.icons_tabbedmode=���������� �����
gr.icons_windowedmode=����� Windowed
gr.dialog_shutdown=�� %1 ����� ������. ������ �� ����������� ��� ����������� ��� %1 ��� �� ���������� � �����������?
gr.dialog_firewall=� ����������� ��� ������������ ������� �� ��������� �� PeerProject ��� Windows Firewall. % �������� ��������� �� PeerProject ���� exception ����� �����������
gr.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
gr.page_safetywarning_text=���� �������������� �� internet, �� ������ ����� �� ����� ��� ��������� ���������� ��� ���� ����������� ��� �� ��� ����������� ��� ������ ��� ���� ���������� �����������. �������� �� ������ ��� ����� �� ���� ����������� ���������� ��� ���� ��� ���� ��������� ��� �� ������������ ��� ���������� ��� ������������ ����� ��� ��������:
gr.page_safetywarning_title=������������� ��� ��
gr.page_safetywarning_subtitle=����� ��� ��������� ���������� ��� ���� �������������?
gr.CreateDesktopIcon=�������� �� &��������� ���� ��������� ��������
gr.CreateQuickLaunchIcon=�������� ��� �&�������� �������� ���������
; Hebrew
heb.components_plugins=������
heb.components_skins=������
heb.tasks_languages=����
heb.tasks_allusers=�� ��������
heb.tasks_selectusers=���� �� %1 ����
heb.tasks_currentuser=%1 ��
heb.tasks_multisetup=���� ����� �������� ������
;heb.tasks_upnp=Enable Plug'n'Play port-forwarding
heb.tasks_firewall=���� ��� ����� ��� ������� ����
;heb.tasks_webhook=����� ����� ����� �������
heb.tasks_resetdiscoveryhostcache=����� ������ ���  (�����/������)
heb.tasks_deleteoldsetup=��� ������ �����
heb.run_skinexe=���� ����� ������...
heb.reg_incomingchat=����� �'�� �����
heb.reg_apptitle=����� ������ ����������� �� ����
heb.icons_license=AGPL �����
heb.icons_uninstall=��� �����
heb.icons_downloads=������
heb.icons_basicmode=��� ����
heb.icons_tabbedmode=��� �����
heb.icons_windowedmode=��� ������
heb.dialog_shutdown=?���� %1 ���� ��� �� ���� ����� �� %1 �� ������� ���� �����
heb.dialog_firewall=������ ����� ������ �� ���� �� ���� ���%n��� ���� �� ���� ������ ������� ����� ��� ����� ����
heb.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
heb.page_safetywarning_text=����/� ����� �������� ����� ���� ����� ������ ���� ����-����� ������ ����� ���� �������/������/������, ������ �� ����-������� ������ ����� ����� ��� ������ ���:
heb.page_safetywarning_title=����� �����
heb.page_safetywarning_subtitle=?��� �� �� ����� ����-����� ������
heb.CreateDesktopIcon= ��� ��� �� �&���� �����
heb.CreateQuickLaunchIcon=��� ��� �� �&���� �����
; Hungarian
hu.components_plugins=Pluginek
hu.components_skins=Kin�zetek
hu.tasks_languages=Nyelvek telep�t�se
hu.tasks_allusers=Minden felhaszn�l�
hu.tasks_selectusers=Megadott felhasz�l�:
hu.tasks_currentuser=Jelenlegi felhaszn�l�
hu.tasks_multisetup=T�bbfelhaszn�l�s m�d enged�lyez�se
;hu.tasks_upnp=Automatikus routerbe�ll�t�s (UPnP szolg�ltat�s)
hu.tasks_firewall=Felv�tel a Windows t�zfal kiv�teleihez
hu.tasks_webhook=Enged�lyezze a b�ng�szo integr�ci�ja
hu.tasks_resetdiscoveryhostcache=A Szerverkeres� �s a Kiszolg�l�k list�j�nak alaphelyzetbe �ll�t�sa
hu.tasks_deleteoldsetup=R�gi telep�t�k t�rl�se
hu.run_skinexe=Kin�zet telep�t�se folyamatban...
hu.reg_incomingchat=Bej�v� chat �zenet
hu.reg_apptitle=PeerProject f�jlmegoszt� program
hu.icons_license=AGPL Licenc
hu.icons_uninstall=PeerProject elt�vol�t�sa
hu.icons_downloads=Let�lt�sek
hu.icons_basicmode=Egyszer� n�zet
hu.icons_tabbedmode=�sszetett n�zet
hu.icons_windowedmode=Ablakos n�zet
hu.dialog_shutdown=A %1 jelenleg fut. Be akarja z�rni a programot, hogy a telep�t�s folytat�dhasson?
hu.dialog_firewall=A telep�t� nem tudta hozz�adni a PeerProject a Windows t�zfal kiv�teleihez.%nManu�lisan kell ezt megtenni.
hu.dialog_malwaredetected=A telep�t� egy k�rt�kony programot tal�lt a rendszerben: %1. Miel�tt telep�ten� a PeerProject, el�bb t�vol�tsa el azt. Ki akar most l�pni?
hu.page_safetywarning_text=Az internet haszn�lata sor�n mindig legyen feltelep�tve egy, a legfrissebb v�rusadatb�zissal rendelkez� antiv�rus program, ami megv�d a f�rgekt�l, tr�jai �s egy�b k�rt�kony programokt�l. Ha k�veti ezt a linket, sok j� v�ruskeres�t tal�lhat �s tov�bbi hasznos tippeket kaphat a sz�m�t�g�p v�delm�r�l:
hu.page_safetywarning_title=V�rusvesz�ly
hu.page_safetywarning_subtitle=Van feltelep�tett antiv�rus programja?
hu.CreateDesktopIcon=Ikon l�trehoz�sa az &Asztalon
hu.CreateQuickLaunchIcon=Ikon l�trehoz�sa a &Gyorsind�t�s eszk�zt�ron
hu.PathNotExist=Hiba, a megadott %1 mappa nem l�tezik
; Italian
it.components_plugins=Plugin
it.components_skins=Skin
it.tasks_languages=Multi-lingua
it.tasks_allusers=Tutti gli utenti
it.tasks_selectusers=Installa %1 per:
it.tasks_currentuser=Solo %1
it.tasks_multisetup=Abilita il supporto multi-utente
;it.tasks_upnp=Abilita il rilevamento dei dispositivi Plug'n'Play
it.tasks_firewall=Aggiungi un'eccezione a Windows Firewall
it.tasks_webhook=Abilitare l'integrazione del browser web
it.tasks_resetdiscoveryhostcache=Resetta i servizi di connessione e la cache host
it.tasks_deleteoldsetup=Cancella installer vecchi
it.run_skinexe=Installazione delle skin in corso...
it.reg_incomingchat=Messaggio di chat in arrivo
it.reg_apptitle=PeerProject, il programma definitivo di P2P
it.icons_license=AGPL Licenza
it.icons_uninstall=Disinstalla
it.icons_downloads=Download
it.icons_basicmode=Modalit� basico
it.icons_tabbedmode=Modalit� a tabelle
it.icons_windowedmode=Modalit� a finestre
it.dialog_shutdown=%1 � attualmente in esecuzione. Vuoi che %1 sia chiuso cos� l'installazione pu� continuare?
it.dialog_firewall=Impossibile aggiungere PeerProject a Windows Firewall.%nAggiungi PeerProject alla lista delle eccezioni manualmente.
it.dialog_malwaredetected=Un programma malevolo (malware) � stato rilevato sul tuo sistema in %1, rimuovilo con un programma anti-virus/anti-malware prima di installare PeerProject. Vuoi uscire adesso?
it.page_safetywarning_text=Quando usi internet, dovresti sempre assicurarti di aver un antivirus aggiornato per proteggerti dai trojan, worm e dagli altri programmi malevoli. Puoi trovare una lista di buoni antivirus e altri suggerimenti di sicurezza per proteggere il tuo computer seguendo questo link:
it.page_safetywarning_title=Attenzione ai virus
it.page_safetywarning_subtitle=Hai installato un programma antivirus?
it.CreateDesktopIcon=Visualizza un'icona sul &desktop
it.CreateQuickLaunchIcon=Visualizza un'icona in &Avvio veloce
it.PathNotExist=Errore, il percorso della cartella %1 non esiste
; Lithuanian
lt.components_plugins=Papildiniai
lt.components_skins=Apvalkalai
lt.tasks_languages=Kalbos
lt.tasks_allusers=Visiems vartotojams
lt.tasks_selectusers=�diegti �%1� �iems vartotojams:
lt.tasks_currentuser=Tik vartotojui %1
lt.tasks_multisetup=�galinti keli� vartotoj� palaikym�
;lt.tasks_upnp=�galinti Plug'n'Play �rengini� aptikim�
lt.tasks_firewall=Prid�ti prie Windows ugniasien�s i�im�i� s�ra�o
lt.tasks_webhook=Ijungti interneto nar�ykles integracija
lt.tasks_resetdiscoveryhostcache=Apnulinti tarnybas ir adresat� talpyklas
lt.tasks_deleteoldsetup=Trinti senas diegimo programas
lt.run_skinexe=Vykdoma apvalkalo s�ranka...
lt.reg_incomingchat=Gaunama �inut� pokalbiui
lt.reg_apptitle=PeerProject � geriausia programa byl� mainams
lt.icons_license=AGPL Licencin� sutartis
lt.icons_uninstall=Pa�alinti
lt.icons_downloads=Atsisiuntimai
lt.icons_basicmode=�prastin� veiksena
lt.icons_tabbedmode=Korteli� veiksena
lt.icons_windowedmode=Lang� veiksena
lt.dialog_shutdown=�%1� �iuo metu dirba. Ar norite nutraukti �%1� darb�, tam kad b�t� galima t�sti �diegim�?
lt.dialog_firewall=�diegimo programai nepavyko prid�ti �PeerProject� prie Windows ugniasien�s i�im�i� s�ra�o.%nPrid�kite j� � �� s�ra�� rankiniu b�du.
lt.dialog_malwaredetected=J�s� sistemoje aptiktos kenk�ji�kos programos (%1). Prie� diegdami �PeerProject�, pa�alinkite jas antivirusine programa ar skeneriu. Nutraukti diegim� dabar?
lt.page_safetywarning_text=Visada, kai naudojat�s Internetu, �sitikinkite, jog turite naujausi� virus� skener�, tam kad b�tum�te apsaugoti nuo trojan�, kirmin� ir kitoki� kenk�ji�k� program�. J�s galite rasti ger� virus� skeneri� s�ra�� ir kitokius kompiuterio apsaugojimo patarimus nu�j� �iuo adresu:
lt.page_safetywarning_title=�sp�jimas apie virusus
lt.page_safetywarning_subtitle=Ar J�s turite �sidieg� antivirusin� program�?
lt.CreateDesktopIcon=Rodyti piktogram� &Darbalaukyje
lt.CreateQuickLaunchIcon=Rodyti Spar�iosios &Paleisties piktogram�
lt.PathNotExist=Klaida, katalogo kelias �%1� neegzistuoja
; Dutch
nl.components_plugins=Plugins
nl.components_skins=Skins
nl.tasks_languages=Talen
nl.tasks_allusers=Alle gebruikers
nl.tasks_selectusers=Installeer %1 voor:
nl.tasks_currentuser=Alleen %1
nl.tasks_multisetup=Ondersteuning voor meerdere gebruikers inschakelen
;nl.tasks_upnp=Configureer automatisch mijn router (Plug'n'Play)
nl.tasks_firewall=Een uitzondering aan de Windows Firewall toevoegen
nl.tasks_webhook=Schakel webbrowser integratie
nl.tasks_resetdiscoveryhostcache=Herstel de Discovery- en Hostcachelist
nl.tasks_deleteoldsetup=Oude installatieprogramma's wissen
nl.run_skinexe=Skin installatie uitvoeren...
nl.reg_incomingchat=Nieuw chat bericht
nl.reg_apptitle=PeerProject P2P FileSharer
nl.icons_license=AGPL Gebruiksovereenkomst
nl.icons_uninstall=Verwijderen
nl.icons_downloads=Downloads
nl.icons_basicmode=Basic Stijl
nl.icons_tabbedmode=Tabblad Stijl
nl.icons_windowedmode=Venster Stijl
nl.dialog_shutdown=%1 is momenteel open. Wil je %1 afsluiten zodat de installatie verder kan gaan?
nl.dialog_firewall=Het installatieprogramma kon PeerProject niet toevoegen aan de Windows Firewall.%nVoeg PeerProject alstublieft manueel toe aan de uitzonderingenlijst.
nl.dialog_malwaredetected=Er werd malware gevonden op %1, verwijder het alstublieft met een virus/malware scanner vooraleer PeerProject verder te installeren. Wil je nu sluiten?
nl.page_safetywarning_text=Als u het internet gebruikt moet u een recente virusscanner gebruiken om u te beschermen tegen virussen, wormen en andere kwaadaardige programma's. U kan een lijst van virusscanners en andere veiligheidstips vinden om uw computer the beschermen door deze link te volgen:
nl.page_safetywarning_title=Virus Waarschuwing
nl.page_safetywarning_subtitle=Heeft u een antivirus programma ge�nstalleerd?
nl.CreateDesktopIcon=Toon een snelkoppeling op het &bureaublad
nl.CreateQuickLaunchIcon=Toon een snelkoppeling op de &Snel Starten werkbalk
; Norwegian
no.components_plugins=Plugins
no.components_skins=Skins
no.tasks_languages=Spr�k
no.tasks_allusers=Alle brukere
no.tasks_selectusers=Installer %1 for:
no.tasks_currentuser=Kun %1
no.tasks_multisetup=Flere brukere
;no.tasks_upnp=Enable Plug'n'Play port-forwarding
no.tasks_firewall=Lag nytt unntak i Windows brannmur
no.tasks_webhook=Aktiver nettleser integrasjon
no.tasks_resetdiscoveryhostcache=Nullstiller nettverksinnstillingene kilder (Verter)
no.tasks_deleteoldsetup=Slett eldre installasjonsfiler
no.run_skinexe=Kj�rer skin installasjon...
no.reg_incomingchat=Innkommende melding
no.reg_apptitle=PeerProject P2P Filesharing
no.icons_license=AGPL Lisens
no.icons_uninstall=Uninstall
no.icons_downloads=Downloads
no.icons_basicmode=Basicmodus
no.icons_tabbedmode=Fanemodus
no.icons_windowedmode=Vindumodus
no.dialog_shutdown=%1 kj�rer. �nsker du at %1 avsluttes slik at installasjonen kan fortsette?
no.dialog_firewall=Installasjonen klarte ikke � lage unntak for PeerProject i Windows Brannmuren. %nVennligst legg til PeerProject i brannmurens unntak manuelt.
no.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
no.page_safetywarning_text=N�r du bruker internett b�r du alltid ha et oppdatert antivirus-program, for � beskytte deg fra trojaner, ormer, og annen skadelig programvare. Du kan finne en liste over gode antivirus-prgrammer og andre sikkerhetstips, for � beskytte din datamaskin, ved � f�lge denne linken:
no.page_safetywarning_title=Virusadvarsel
no.page_safetywarning_subtitle=Har du et antivirus-program installert?
no.CreateDesktopIcon=Vis ikon p� &skrivebordet
no.CreateQuickLaunchIcon=Vis et &Hurtigstarts-ikon
; Polish
pl.components_plugins=Wtyczki
pl.components_skins=Sk�rki
pl.tasks_languages=J�zyki
pl.tasks_allusers=Dla wszystkich u�ytkownik�w
pl.tasks_selectusers=Instaluj dla %1:
pl.tasks_currentuser=tylko %1
pl.tasks_multisetup=W��cz obs�ug� wielu u�ytkownik�w
;pl.tasks_upnp=Enable Plug'n'Play port-forwarding
pl.tasks_firewall=Dodaj wyj�tek do Firewall'a Windows'a
pl.tasks_webhook=Wlacz integracje przegladarki internetowej
pl.tasks_resetdiscoveryhostcache=Reset network sources  (Discovery/Hostcache)
pl.tasks_deleteoldsetup=Usu� stare instalatory
pl.run_skinexe=Instalowanie sk�rek...
pl.reg_incomingchat=Przychodz�ca wiadomo�� chatowa
pl.reg_apptitle=PeerProject P2P Filesharing
pl.icons_license=AGPL Licencja
pl.icons_uninstall=Odinstaluj
pl.icons_downloads=Pobierania
pl.icons_basicmode=Tryb basic
pl.icons_tabbedmode=Tryb zaawansowany
pl.icons_windowedmode=Tryb okienkowy
pl.dialog_shutdown=%1 obecnie dzia�a. Czy chcia�by� wy��czy� %1 aby kontynuowa� instalacj�?
pl.dialog_firewall=Instalator nie potrafi� doda� PeerProejct do Firewall'a Windows'a.%nProsz� doda� PeerProject do listy wyj�tk�w r�cznie.
pl.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
pl.page_safetywarning_text=Podczas u�ywania internetu zawsze powiniene� by� pewny, �e masz program antywirusowy z aktualn� baz� wirus�w, kt�ry ci� ochroni przed trojanami, robakami i innym niebezpiecznym oprogramowaniem. Mo�esz znale�� list� dobrych program�w antywirusowych i porady jak zabezpieczy� komputer pod nast�puj�cymi adresami:
pl.page_safetywarning_title=Ostrze�enie przed wirusem
pl.page_safetywarning_subtitle=Czy masz zainstalowany jaki� program antywirusowy?
pl.CreateDesktopIcon=Wy�wietl ikon� na pulpicie
pl.CreateQuickLaunchIcon=Wy�wietl ikon� na pasku szybkiego uruchamiania
; Portuguese std
pt.components_plugins=Plugins
pt.components_skins=Skins
pt.tasks_languages=Linguagens
pt.tasks_allusers=Todos os usu�rios
pt.tasks_selectusers=Instalar %1 para:
pt.tasks_currentuser=somente %1
pt.tasks_multisetup=Ativar o suporte a v�rios usu�rios
;pt.tasks_upnp=Ativar a descoberta de dispositivos Plug'n'Play
pt.tasks_firewall=Adicionar exce��o ao Firewall do Windows
pt.tasks_webhook=Permitir a integra��o navegador
pt.tasks_resetdiscoveryhostcache=Resetar o Cache de Armazenamento e Descoberta
pt.tasks_deleteoldsetup=Apagar os instaladores antigos
pt.run_skinexe=Instalando a Skin...
pt.reg_incomingchat=Mensagem de chat
pt.reg_apptitle=PeerProject Compartilhamento de Arquivos Incompar�vel
pt.icons_license=AGPL Licen�a
pt.icons_uninstall=Desinstalar
pt.icons_downloads=Downloads
pt.icons_basicmode=Modo Basic
pt.icons_tabbedmode=Modo de Abas
pt.icons_windowedmode=Modo de Janelas
pt.dialog_shutdown=O %1 est� sendo executado. Voc� gostaria que o %1 fosse fechado para que a instala��o continue?
pt.dialog_firewall=O Setup falhou em adicionar o PeerProject no Firewall do Windows.%nPor favor adicione o PeerProject a lista de exce��es manualmente.
pt.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
pt.page_safetywarning_text=Quando usando a internet, voc� deve sempre garantir que voc� tenha um scanner contra v�rus atualizado para proteger voc� de trojans, worms e outros programas maliciosos. Voc� pode achar uma lista de bons scanners contra v�rus e outras dicas de seguran�a para proteger o seu computador seguindo este link:
pt.page_safetywarning_title=Alerta contra V�rus
pt.page_safetywarning_subtitle=Voc� tem um programa Anti-V�rus instalado?
pt.CreateDesktopIcon=Criar �cone no &Ambiente de Trabalho
pt.CreateQuickLaunchIcon=Criar �cone na barra de Inicializa��o &R�pida
; Portuguese-Brazil
pt_br.components_plugins=Plugins
pt_br.components_skins=Skins
pt_br.tasks_languages=Multi-idioma
pt_br.tasks_allusers=Todos os Usu�rios
pt_br.tasks_selectusers=Instalar o %1 para:
pt_br.tasks_currentuser=s� para o %1
pt_br.tasks_multisetup=Ativar suporte para v�rios usu�rios
;pt_br.tasks_upnp=Ativar o descobrimento de dispositivos Plug'n'Play
pt_br.tasks_firewall=Adicionar uma exce��o ao Firewall do Windows
pt_br.tasks_webhook=Permitir a integra��o navegador
pt_br.tasks_resetdiscoveryhostcache=Resetar o Descobrimento e o Cache dos Hosts
pt_br.tasks_deleteoldsetup=Apagar os instaladores antigos
pt_br.run_skinexe=Executando a instala��o das skins...
pt_br.reg_incomingchat=Mensagem de chat a entrar
pt_br.reg_apptitle=PeerProject Compartilhador de Arquivo
pt_br.icons_license=AGPL Licen�a
pt_br.icons_uninstall=Desinstalar
pt_br.icons_downloads=Downloads
pt_br.icons_basicmode=Modo Simples
pt_br.icons_tabbedmode=Modo em Abas
pt_br.icons_windowedmode=Modo de Janelas
pt_br.dialog_shutdown=O %1 est� rodando atualmente. Voc� gostaria que o %1 fosse fechado para que a instala��o possa continuar?
pt_br.dialog_firewall=A instala��o falhou ao tentar adicionar o PeerProject a lista de exce��es do Firewall do Windows.%nPor favor adicione o PeerProject a lista de exce��es manualmente.
pt_br.dialog_malwaredetected=Um malware foi detectado no seu sistema em %1, por favor remova-o com um scanner contra v�rus/malware antes de instalar o PeerProject. Voc� quer sair agora?
pt_br.page_safetywarning_text=Quando usar a Internet voc� deve sempre garantir que voc� tenha um scanner contra v�rus atualizado para proteger voc� de trojans, worms e outros programas maliciosos. Voc� pode achar uma lista de bons scanners contra v�rus e outras dicas de seguran�a para proteger seu computador seguindo este link:
pt_br.page_safetywarning_title=Aviso sobre_V�rus
pt_br.page_safetywarning_subtitle=Voc� tem um programa anti-v�rus instalado?
pt_br.CreateDesktopIcon=Exibir um �cone na &�rea de trabalho
pt_br.CreateQuickLaunchIcon=Exibir um �cone na barra de &Inicializa��o R�pida
pt_br.PathNotExist=Erro, o caminho da pasta %1 n�o existe
; Russian
ru.components_plugins=������
ru.components_skins=������
ru.tasks_languages=�����
ru.tasks_allusers=���� �������������
ru.tasks_selectusers=���������� %1 ���:
ru.tasks_currentuser=������ ��� %1
ru.tasks_multisetup=��������� ��������� ���������� �������������
;ru.tasks_upnp=�������� ���������� �������� Plug'n'Play
ru.tasks_firewall=�������� � ������ ���������� ���������� Windows
ru.tasks_webhook=��������� ���������� ���-��������
ru.tasks_resetdiscoveryhostcache=�������� ������� ���������� � ��� ������
ru.tasks_deleteoldsetup=������� ������ ������������
ru.run_skinexe=��� ��������� ������...
ru.reg_incomingchat=�������� ��������� ��� ����
ru.reg_apptitle=PeerProject - ��������� ��� ������ �������
ru.icons_license=AGPL ��������
ru.icons_uninstall=�������������
ru.icons_downloads=��������
ru.icons_basicmode=������� �����
ru.icons_tabbedmode=����� ��������
ru.icons_windowedmode=������� �����
ru.dialog_shutdown=%1 � ������ ������ ��������. ������ �� ��������� ������ %1, ����� ���������� ���������?
ru.dialog_firewall=��������� ��������� �� ������ �������� PeerProject � ������ ���������� ����������� Windows.%n����������, �������� �� � ���� ������ �������.
ru.dialog_malwaredetected=� ����� ������� ����������� ����������� ��������� (%1), ������� �� ����� ������������ PeerProject ��� ������� ������������ ��������� ��� �������. ������ ����� ������?
ru.page_safetywarning_text=������, ����� ����������� ����������, ��������������, ��� � ��� ���� �������� ������ ��� �������, ����� �������� ��������� �� �������, ������ � ������ ������������� ��������. �� ������ ����� ������ ������� �������� ��� ������� � ������ ������ � ������ ���������� �� ����� ������:
ru.page_safetywarning_title=��������������� � �������
ru.page_safetywarning_subtitle=������ �� �� ������������� ������������ ���������?
ru.CreateDesktopIcon=���������� ������ �� &������� �����
ru.CreateQuickLaunchIcon=���������� ������ � &������ �������� �������
ru.PathNotExist=������, ���� � ����� %1 �� ����������
; Slovenian
sl.components_plugins=Vti�niki
sl.components_skins=Preobleke
sl.tasks_languages=Jeziki
sl.tasks_allusers=Vsi uporabniki
sl.tasks_selectusers=Namesti %1 za:
sl.tasks_currentuser=Samo %1
sl.tasks_multisetup=Omogo�i ve�-uporabni�ko podporo
;sl.tasks_upnp=Enable Plug'n'Play port-forwarding
sl.tasks_firewall=Dodaj izjemo v Windows po�arni zid
sl.tasks_webhook=Omogocajo integracijo spletnega brskalnika
sl.tasks_resetdiscoveryhostcache=Reset network sources  (Discovery/Hostcache)
sl.tasks_deleteoldsetup=Bri�i stare name��evalce
sl.run_skinexe=Namestitev preobleke v teku...
sl.reg_incomingchat=Vhodno kramljalno sporo�ilo
sl.reg_apptitle=Izjemno PeerProject deljenje datotek
sl.icons_license=AGPL Licenca
sl.icons_uninstall=Odnamestitev
sl.icons_downloads=Prenosi
sl.icons_basicmode=Navadni na�in
sl.icons_tabbedmode=Na�in z zavihki
sl.icons_windowedmode=Na�in z okni
sl.dialog_shutdown=%1 je trenutno v teku. Ali �elite zapreti %1 zato, da se lahko nadaljuje namestitev?
sl.dialog_firewall=Program namestitve ni uspel dodati PeerProject v po�arni zid Windows-ov.%nPeerProject boste morali ro�no dodati v seznam izjem v po�arnem zidu.
sl.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
sl.page_safetywarning_text=Pri uporabi medmre�ja imejte name��eno vedno najnovej�o razli�ico protivirusne za��ite. Tako boste kar najbolje za��iteni pred trojanskimi konji, �rvi in drugimi zlonamernimi programji. Seznam dobrih protivirusnih programov, ter drugih nasvetov glede za��ite va�ega ra�unalnika, boste na�li preko naslednje spletne povezave:
sl.page_safetywarning_title=Virusno opozorilo
sl.page_safetywarning_subtitle=Ali imate name��en protivirusni program?
sl.CreateDesktopIcon=Prika�i ikono &namizja
sl.CreateQuickLaunchIcon=Prika�i ikono &Hitrega zaganjalnika
; Albanian
sq.components_plugins=Spina
sq.components_skins=Veshje
sq.tasks_languages=Shum� gjuh�
sq.tasks_allusers=T�r� p�rdoruesit
sq.tasks_selectusers=Instalo %1 p�r:
sq.tasks_currentuser=%1 vet�m
sq.tasks_multisetup=Lejo p�rkrahjen m� shum� p�rdorues
;sq.tasks_upnp=Lejo zbulimin e pajisjeve Plug'n'Play
sq.tasks_firewall=Shto nj� p�rjashtim te Fajruolli i Windows-it
sq.tasks_webhook=Mund�soj� integrimin shfletuesit web
sq.tasks_resetdiscoveryhostcache=Rivendos Zbulimin dhe Depon me Hostet
sq.tasks_deleteoldsetup=Fshiji instaluesit e vjet�r
sq.run_skinexe=Vepron instalimi i veshjes...
sq.reg_incomingchat=Mesazh hyr�s n� chat
sq.reg_apptitle=Shp�rndarja m� e Lart� e Skedave me PeerProject
sq.icons_license=AGPL Licenc�
sq.icons_uninstall=�instalo
sq.icons_downloads=Shkarkesa
sq.icons_basicmode=M�nyr� me Thjesht�
sq.icons_tabbedmode=M�nyr� me Fusha
sq.icons_windowedmode=M�nyr� me Dritare
sq.dialog_shutdown=%1 aktualisht po vepron. A do e mbyll�sh %1, q� t� vijosh me instalimin?
sq.dialog_firewall=Sistemimi d�shtoi ta shtoj� PeerProject-n te Fajruolli i Windows-it.%nT� lutem shtoje vet� PeerProject-n te lista e p�rjashtimeve.
sq.dialog_malwaredetected=Te sistemi yt n� %1 diktohet nj� keqb�r�s. T� lutem fshije at� me nj� skanues virusi/keqb�r�si para se t� instalohet PeerProject. Do e mbyll�sh tani?
sq.page_safetywarning_text=Kur p�rdor internetin duhet t� sigurohesh q� ke nj� skanues virusi t� ridatuar q� t� mbrohesh nga trojan�t, krimbat dhe programet e tjera keqb�r�se. N�se ndjek k�t� link mund t� gjesh nj� list� me skanuesa virusi t� mir� dhe k�shilla t� tjera sigurie se si mbrohet kompjuteri:
sq.page_safetywarning_title=Lajm�rim Virusi
sq.page_safetywarning_subtitle=A ke instaluar nj� program AntiVirus?
sq.CreateDesktopIcon=Trego nj� ikon� n� &tryez�
sq.CreateQuickLaunchIcon=Trego nj� ikon� n� &Quick Launch
sq.PathNotExist=Gabim, shtegu i dosjes s� %1 nuk ekziston!
; Serbian
sr.components_plugins=Pluginovi
sr.components_skins=Skinovi
sr.tasks_languages=Jezici
sr.tasks_allusers=Svi korisnici
sr.tasks_selectusers=Instaliraj %1 za:
sr.tasks_currentuser=%1 samo
sr.tasks_multisetup=Omogu�i vi�e-korisni�ku podr�ku
;sr.tasks_upnp=Enable Plug'n'Play port-forwarding
sr.tasks_firewall=Dodaj izuzetak u Windows Vatrozid
;sr.tasks_webhook=Enable web browser integration
sr.tasks_resetdiscoveryhostcache=Reset network sources  (Discovery/Hostcache)
sr.tasks_deleteoldsetup=Ukloni stare instalere
sr.run_skinexe=Instalacija skina u toku...
sr.reg_incomingchat=Dolaze�e cet poruke
sr.reg_apptitle=PeerProject Najbolji P2P za Deljenje Datoteka
sr.icons_license=AGPL Licenca
sr.icons_uninstall=Ukloni Program
sr.icons_downloads=Skinutno
sr.icons_basicmode=Basic Prikaz
sr.icons_tabbedmode=Prikaz s Karticama
sr.icons_windowedmode=U Prozoru
sr.dialog_shutdown=%1 je uklju�ena. Da li bi �eleli da %1 bude uga�ena da bi se instalacija nastavila?
sr.dialog_firewall=Instalacija nije uspla da doda PeerProject-u u Windows Vatrozid.%nMolimo dodajte PeerProject-u na listu izuzetaka ru�no.
sr.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
sr.page_safetywarning_text=Kada koristite internet, trebali bi uvek da budete sigurni da imate a�uriaran virus skener koji Vas �titi od trojanaca, crva, i drugih zlonamernih programa. Mo�ete prona�i listu dobrih anti-virus programa i drugih sigurnosnih saveta kako da za�titite Va� ra�unar prate�i ovaj link:
sr.page_safetywarning_title=Virus Uopzorenje
sr.page_safetywarning_subtitle=Da li imate AntiVirus program instaliran?
sr.CreateDesktopIcon=Napravi &desktop ikonu
sr.CreateQuickLaunchIcon=Napravi &Brzo Pokretanje(QL) ikonu
; Swedish
sv.components_plugins=Insticksprogram
sv.components_skins=Skinn
sv.tasks_languages=Spr�k
sv.tasks_allusers=Alla anv�ndare
sv.tasks_selectusers=Installera %1 f�r:
sv.tasks_currentuser=%1 endast
sv.tasks_multisetup=Aktivera st�d f�r flera anv�ndare
;sv.tasks_upnp=Till�t uppt�ckt av UPnP-enheter
sv.tasks_firewall=L�gg till ett undantag till Windows brandv�gg
sv.tasks_webhook=Aktivera integrering webbl�sare
sv.tasks_resetdiscoveryhostcache=�terst�ll uppt�ckstj�nster och v�rdcache
sv.tasks_deleteoldsetup=Radera gamla installationer
sv.run_skinexe=K�r skinninstallation...
sv.reg_incomingchat=Inkommande chattmeddelande
sv.reg_apptitle=PeerProject Fildelning
sv.icons_license=AGPL Licens
sv.icons_uninstall=Avinstallera
sv.icons_downloads=Nedladdningar
sv.icons_basicmode=Basic l�ge
sv.icons_tabbedmode=Tabbl�ge
sv.icons_windowedmode=F�nsterl�ge
sv.dialog_shutdown=%1 k�rs f�r tillf�llet. Vill du att %1 ska st�ngas ned s� att installationen kan forts�tta?
sv.dialog_firewall=Installationsprogrammet lyckades inte l�gga till PeerProject till brandv�ggen i Windows .%nL�gg till PeerProject till undantagslistan manuellt.
sv.dialog_malwaredetected=Skadlig programvara har hittats p� din dator vid %1, ta bort den med ett antivirusprogram innan du installerar PeerProject. Vill du avbryta nu?
sv.page_safetywarning_text=N�r du anv�nder internet ska du alltid f�rs�kra dig om att du har ett uppdaterat antivirusprogram som skyddar dig mot trojaner, maskar och andra skadliga program. H�r finns en lista p� bra antivirusprogram och andra s�kerhetstips f�r att skydda din dator:
sv.page_safetywarning_title=Virusvarning
sv.page_safetywarning_subtitle=Har du ett antivirusprogram installerat?
sv.CreateDesktopIcon=Skapa en ikon p� srivbordet
sv.CreateQuickLaunchIcon=Skapa en ikon i Snabbstartf�ltet
sv.PathNotExist=S�kv�gen till mappen %1 existerar inte
; Turkish
tr.components_plugins=Eklentiler
tr.components_skins=Aray�zler
tr.tasks_languages=Diller
tr.tasks_allusers=T�m Kullan�c�lar
tr.tasks_selectusers=%1 Kuruldu:
tr.tasks_currentuser=Sadece %1
tr.tasks_multisetup=�oklu kullan�c� deste�ini etkinle�tir
;tr.tasks_upnp=Plug'n'Play cihazlarin kesfini etkinlestir
tr.tasks_firewall=Windows G�venlik Duvar�na bir de�i�iklik ekle
tr.tasks_webhook=Web tarayicisi entegrasyonu etkinlestir
tr.tasks_resetdiscoveryhostcache=Kesif servisleri ile host bellegini sifirla
tr.tasks_deleteoldsetup=Eski kurulumlar� sil
tr.run_skinexe=Aray�z kurulumu �al���yor...
tr.reg_incomingchat=Gelen sohbet mesaj�
tr.reg_apptitle=PeerProject En iyi Dosya Payla��m�
tr.icons_license=AGPL Lisans
tr.icons_uninstall=Kurulumu Kald�r
tr.icons_downloads=�ndirmeler
tr.icons_basicmode=Temel Mod
tr.icons_tabbedmode=Sekmeli Mod
tr.icons_windowedmode=Pencereli Mode
tr.dialog_shutdown=�uan %1 �al���yor.Kurulumun devam edebilmesi i�in %1'in kapal� olmas�n� istiyor musunuz?
tr.dialog_firewall=Windows g�venlik duvar�na PeerProject kurulumunu eklemek ba�ar�s�z oldu.%n L�tfen PeerProject'y� el ile istisna listesine ekle
tr.dialog_malwaredetected=K�t�c�l bir yazilim %1'de tespit edilmistir, PeerProject'nin kurulumundan �nce s�z� ge�en k�t�c�l yazilimi bir antivirus programi kullanarak sisteminizden silmeniz gerekmektedir. Simdi terk etmek ister misiniz?
tr.page_safetywarning_text=�nternet kullan�yorken, trojanlardan, wormlardan ve di�er k�t� niyetli programlardan sizi koruyan g�ncel bir vir�s taray�c�s�na sahip oldu�unuzdan emin olmal�s�n�z. Bu ba�lant�y� izleyerek bilgisayar�n�z� koruyan iyi vir�s taray�c�lar�n�n ve di�er g�venlik tiplerinin listesini bulacaks�n�z:
tr.page_safetywarning_title=Vir�s Uyar�s�
tr.page_safetywarning_subtitle=Bir AntiVirus program� y�klediniz mi?
tr.CreateDesktopIcon=Bir &Masa�st� ikonu g�r�nt�le
tr.CreateQuickLaunchIcon=Bir &H�zl� Ba�lat ikonu g�r�nt�le
; Chinese Traditional
zht.components_plugins=�~��
zht.components_skins=�~�[
zht.tasks_languages=�y��
zht.tasks_allusers=�Ҧ����ϥΪ�
zht.tasks_selectusers=���o�ǨϥΪ̦w�� %1:
zht.tasks_currentuser=�u�� %1
zht.tasks_multisetup=�ҥΦh���ϥΪ̤䴩
;zht.tasks_upnp=Enable Plug'n'Play port-forwarding
zht.tasks_firewall=�W�[�ҥ~�� Windows ������ (XP)
zht.tasks_webhook=�ҥ�Web�s��������
zht.tasks_resetdiscoveryhostcache=���m�������]�r��/�D���^
zht.tasks_deleteoldsetup=�R���ª��w�˵{��
zht.run_skinexe=���b�w�˥~�[...
zht.reg_incomingchat=��J����ѰT��
zht.reg_apptitle=PeerProject---�̲ת��ɮפ��ɳn��
zht.icons_license=AGPL �n����v��w
zht.icons_uninstall=�Ѱ��w��
zht.icons_downloads=�U��
zht.icons_basicmode=�зǼҦ�
zht.icons_tabbedmode=���ҼҦ�
zht.icons_windowedmode=�h�������Ҧ�
zht.dialog_shutdown=%1 ���b�B�@��. �z�n���� %1 , ���w�˵{���o�H�~��i���?
zht.dialog_firewall=�w�˵{���L�k�s�W PeerProject ��Windows ������.%n�Ф�ʷs�W PeerProject �ܨҥ~�M��
zht.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
zht.page_safetywarning_text=�ϥκ��ں�����, �z�����`�O�T�O���r�n�鬰�̷s����, �p���~��O�@�z�קK����차, į��, �άO�c�N�{�����I�`. �z�i�H�I��o�ӳs�����o�w���ʯ��Z�P�}�n���r�n�骺�M��:
zht.page_safetywarning_title=�f�rĵ�i
zht.page_safetywarning_subtitle=�z�O�_�w�g�w�ˤF�@�Ө��r�n��?
zht.CreateDesktopIcon=�зs�W�@��&�ୱ�ϥ�
zht.CreateQuickLaunchIcon=�зs�W�@��&�ֳt�Ұʹϥ�
; Chinese Simplified
zhs.components_plugins=���
zhs.components_skins=Ƥ��
zhs.tasks_languages=����
zhs.tasks_allusers=�����û�
zhs.tasks_selectusers=��װ %1 Ϊ:
zhs.tasks_currentuser=�� %1
zhs.tasks_multisetup=���ö��û�֧��
;zhs.tasks_upnp=���� UPnP ��������ѯ
zhs.tasks_firewall=���һ�����⵽ Windows ����ǽ
zhs.tasks_webhook=����Web���������
zhs.tasks_resetdiscoveryhostcache=��������Դ���侰/������
zhs.tasks_deleteoldsetup=ɾ���ɵİ�װ�ļ�
zhs.run_skinexe=��װƤ��...
zhs.reg_incomingchat=����������Ϣ
zhs.reg_apptitle=PeerProject �ռ��ļ�����
zhs.icons_license=AGPL ���
zhs.icons_uninstall=ж��
zhs.icons_downloads=����
zhs.icons_basicmode=��ͨģʽ
zhs.icons_tabbedmode=��ǩģʽ
zhs.icons_windowedmode=�Ӵ�ģʽ
zhs.dialog_shutdown=%1 �������С���ϣ���ر� %1 �Ա������װ��
zhs.dialog_firewall=��װ��� PeerProject �� Windows ����ǽʧ�ܡ�%n�뽫 PeerProject �ֶ�����������б�
zhs.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
zhs.page_safetywarning_text=�����û�����ʱ������Ҫȷ����ӵ�����µĲ���ɨ������Ա���������ľ�������������������ֺ����������������������ҵ��ϺõĲ���ɨ��������б��Լ������������ļ�����İ�ȫ����:
zhs.page_safetywarning_title=��������
zhs.page_safetywarning_subtitle=����װ�˷������������
zhs.CreateDesktopIcon=��ʾ����ͼ��(&D)
zhs.CreateQuickLaunchIcon=��ʾ����������ͼ��(&Q)
; Japanese
ja.components_plugins=�v���O�C��
ja.components_skins=�X�L��
ja.tasks_languages=����t�@�C��
ja.tasks_allusers=���ׂẴ��[�U�[
ja.tasks_selectusers=%1�����悤���郆�[�U�[:
ja.tasks_currentuser=%1�̂�
ja.tasks_multisetup=�}���`���[�U�[�T�|�[�g
;ja.tasks_upnp=UPnP�Ή��@��̌��o��L���ɂ���
ja.tasks_firewall=Windows�t�@�C���[�E�H�[���̗�O�ɐݒ�
ja.tasks_webhook=Web�u���E�U�̓�����L���ɂ���
ja.tasks_resetdiscoveryhostcache=�l�b�g���[�N�\�[�X�����Z�b�g���܂��B �i�f�B�X�J�o���[/�z�X�g�j
ja.tasks_deleteoldsetup=�Â��C���X�g�[���[�̍폜
ja.run_skinexe=�X�L���C���X�g�[���[�����s���Ă��܂�...
ja.reg_incomingchat=�`���b�g���b�Z�[�W���󂯓����
ja.reg_apptitle=PeerProject�t�@�C�����L�\�t�g
ja.icons_license=AGPL ���C�Z���X
ja.icons_uninstall=�A���C���X�g�[��
ja.icons_downloads=�_�E�����[�h
ja.icons_basicmode=�W�����[�h
ja.icons_tabbedmode=�^�u���[�h
ja.icons_windowedmode=�E�B���h�E���[�h
ja.dialog_shutdown=%1 ���������ł�. %1���I�����ăC���X�g�[���𑱂��܂���?
ja.dialog_firewall=WindowsXP�t�@�C���[�E�H�[���̓o�^�Ɏ��s���܂���.%n�蓮�œo�^���Ă�������.
ja.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
ja.page_safetywarning_text=���Ȃ����C���^�[�l�b�g�ɐڑ�����Ƃ��́A�g���C�⃏�[�����́A����ȊO�̊댯�ȃt�@�C������PC��ی삷�邽�߂ɁA�K���E�C���X��`�t�@�C�����ŐV�̂��̂ɂ��܂��傤�B�E�C���X�X�L���i�[��Z�L�����e�B-�Ɋւ����񂪉��L�̃����N�ɂ���܂��B
ja.page_safetywarning_title=�E�C���X�̌x��
ja.page_safetywarning_subtitle=�A���`�E�E�C���X�E�\�t�g�͓����Ă��܂���?
ja.CreateDesktopIcon=�f�X�N�g�b�v�ɃA�C�R����\��(&d)
ja.CreateQuickLaunchIcon=�N�C�b�N�����`�ɃA�C�R����\��(&Q)
; Korean (ToDo:)
