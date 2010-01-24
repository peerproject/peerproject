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

; Install mutltilanguage remotes
Source: "Remote\cz\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\de\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\es\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\fr\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\gr\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\hu\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\it\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\lt\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\nl\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\no\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\pt-br\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\ru\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\sv\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\sq\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\ja\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language
Source: "Remote\chs\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Tasks: language

; Install localized remote otherwise
; English-UK
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: en_uk; Tasks: not language
; Czech
Source: "Remote\cz\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: cz; Tasks: not language
; German
Source: "Remote\de\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: de; Tasks: not language
; Spanish std
Source: "Remote\es\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: es; Tasks: not language
; Spanish Mexican
Source: "Remote\es\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: es_mx; Tasks: not language
; French
Source: "Remote\fr\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: fr; Tasks: not language
; Greek
Source: "Remote\gr\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: gr; Tasks: not language
; Hungarian
Source: "Remote\hu\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: hu; Tasks: not language
; Italian
Source: "Remote\it\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: it; Tasks: not language
; Lithuanian
Source: "Remote\lt\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: lt; Tasks: not language
; Dutch
Source: "Remote\nl\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: nl; Tasks: not language
; Norwegian
Source: "Remote\no\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: no; Tasks: not language
; Portuguese std
Source: "Remote\pt-br\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: pt; Tasks: not language
; Portuguese Braz
Source: "Remote\pt-br\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: pt_br; Tasks: not language
; Russian
Source: "Remote\ru\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: ru; Tasks: not language
; Swedish
Source: "Remote\sv\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: sv; Tasks: not language
; Albanian
Source: "Remote\sq\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: sq; Tasks: not language
; Serbian
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: sr; Tasks: not language
; Turkish
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: tr; Tasks: not language
; Slovenian
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: sl; Tasks: not language
; Catalan
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: ca; Tasks: not language
; Polish
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: pl; Tasks: not language
; Hebrew
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: heb; Tasks: not language
; Korean
;Source: "Remote\en\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: ko; Tasks: not language
; Japanese
Source: "Remote\ja\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: ja; Tasks: not language
; Chinese Simp
Source: "Remote\chs\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: zhs; Tasks: not language
; Chinese Trad
Source: "Remote\chs\*"; DestDir: "{app}\Remote"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Excludes: ".svn"; Languages: zht; Tasks: not language


; Common License file
Source: "Installer\License\Default.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension

; Install localized license
; English-UK
;Source: "Installer\License\Default.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: en_uk
; Dutch
Source: "Installer\License\Dutch.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: nl
; Lithuanian
Source: "Installer\License\Lithuanian.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: lt
; German
Source: "Installer\License\German.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: de
; Italian
Source: "Installer\License\Italian.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: it
; Norwegian
Source: "Installer\License\Norwegian.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: no
; Afrikaans
Source: "Installer\License\Afrikaans.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: af
; Portuguese std
Source: "Installer\License\Portuguese.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt
; Portuguese Braz
Source: "Installer\License\Portuguese.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pt_br
; French
Source: "Installer\License\French.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fr
; Spanish std
Source: "Installer\License\Spanish.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es
; Spanish Mexican
Source: "Installer\License\Spanish.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: es_mx
; Russian
Source: "Installer\License\Russian.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ru
; Greek
Source: "Installer\License\Greek.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: gr
; Hungarian
Source: "Installer\License\Hungarian.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: hu
; Swedish
Source: "Installer\License\Swedish.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sv
; Finnish
Source: "Installer\License\Finnish.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: fi
; Hebrew
Source: "Installer\License\Hebrew.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: heb
; Polish
Source: "Installer\License\Polish.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: pl
; Czech
Source: "Installer\License\Czech.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: cz
; Serbian
Source: "Installer\License\Serbian.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sr
; Turkish
Source: "Installer\License\Turkish.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: tr
; Japanese
Source: "Installer\License\Japanese.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ja
; Arabic
Source: "Installer\License\Arabic.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ar
; Estonian
Source: "Installer\License\Estonian.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ee
; Chinese Simp
Source: "Installer\License\Chinese-Simple.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zhs
; Chinese Trad
Source: "Installer\License\Chinese-Traditional.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: zht
; Slovenian
Source: "Installer\License\Default.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sl
; Catalan
Source: "Installer\License\Catalan.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: ca
; Catalan
Source: "Installer\License\Albanian.rtf"; DestDir: "{app}\Uninstall"; DestName: "GPL License.rtf"; Flags: ignoreversion overwritereadonly uninsremovereadonly sortfilesbyextension; Languages: sq


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

SelectLanguageLabel=%nSelect a default language:
WelcomeLabel1=%nWelcome to the%n[name] Setup Wizard
WelcomeLabel2=This will install [name/ver]%nP2P filesharing on your computer.%n%nIt is recommended to close applications before continuing.
FinishedHeadingLabel=%nCompleting the [name]%nInstaller Wizard
FinishedLabel=Setup has finished installing [name] on your computer.%nIt can be launched from the installed shortcut icons.
LicenseLabel3=Please read the following copyright license agreement.%nYou should understand the basic terms of this License before continuing installation.


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
tasks_multisetup=Setup Windows User's default folder locations
tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
tasks_firewall=Add an exception to the Windows Firewall
tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
tasks_deleteoldsetup=Delete old installers
run_skinexe=Running skin installation...
reg_incomingchat=Incoming chat message
reg_apptitle=PeerProject P2P Filesharing
icons_license=GPL License
icons_uninstall=Uninstall
icons_downloads=Downloads
icons_basicmode=Basic Mode
icons_tabbedmode=Tabbed Mode
icons_windowedmode=Windowed Mode
CreateQuickLaunchIcon=Add a PeerProject &quick launch icon
CreateDesktopIcon=Add a PeerProject &desktop shortcut icon
CreateDesktopIconWizard=Add a TorrentWizard &desktop shortcut icon
PathNotExist=Error, the path of the %1 folder doesn't exist
dialog_shutdown=%1 is currently running. Would you like %1 to be shutdown so the installation can continue?
dialog_firewall=Setup failed to add PeerProject to the Windows Firewall.%nPlease add PeerProject to the exception list manually.
dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
page_viruswarning_title=Safety Awareness
page_viruswarning_subtitle=Do you have an AntiVirus program running?
page_viruswarning_text=Filesharing is a home server with browser to find others like you.%n%nWhen downloading, always have an updated virus scanner to protect you from malicious files.  Be aware that anti-p2p companies will return fake results to searches, but can easily be banned.%n%nWhen uploading, recognize Permissive and Restricted material.%nSharing unlicensed copyright works may be liable in some countries. The best policy is to boycott corporate copyright-abusers entirely:%nDo not promote those who do not appreciate it, plenty of others do.%n%nRemember, you are ultimately responsible for your own actions.
; Don't copy these last 2 messages, they are just links.
page_viruswarning_link=http://Security.PeerProject.org/
page_viruswarning_destination=http://Security.PeerProject.org/

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
af.tasks_firewall=Voeg 'n uitsondering by die Windows Netskans
af.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
af.tasks_deleteoldsetup=Skrap ou opstellerslêers
af.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
af.run_skinexe=Hardloop omslagte installasie...
af.reg_incomingchat=Inkomende Geselsie-boodskap
af.reg_apptitle=PeerProject P2P Filesharing
af.icons_license=Lisensie-ooreenkoms
af.icons_uninstall=Deïnstalleer
af.icons_downloads=Aflaaie
af.icons_basicmode=Basic Modus
af.icons_tabbedmode=Tabelmodus
af.icons_windowedmode=Venstermodus
af.dialog_shutdown=%1 is op die oomblik besig om te loop. Wil jy graag %1 sluit sodat die installasie kan voortgaan?
af.dialog_firewall=Die Opsteller kon nie PeerProject by die Windows netskans uitsonderings voeg nie.%nVoeg PeerProject asseblief met die hand op die uitsonderingslys.
af.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
af.page_viruswarning_text=Maak altyd seker dat jy 'n opgedateerde anti-virus program geïnstalleer het wanneer jy die internet gebruik, om jou rekenaar te beskerm teen virusse, wurms, en ander ongewenste programme. Jy kan 'n lys van goeie anti-virus programme en ander sekuriteitswenke oor hoe om jou rekenaar te beskerm verkry deur die volgende skakel te volg:
af.page_viruswarning_title=Virus Waarskuwing
af.page_viruswarning_subtitle=Het jy 'n Anti-Virus program geïnstalleer?
af.CreateDesktopIcon=Vertoon 'n &werkskerm ikoon
af.CreateQuickLaunchIcon=Vertoon 'n &Quick Launch ikoon
; Arabic
ar.components_plugins=ÇáæÙÇÆİ ÇáÅÖÇİíÉ
ar.components_skins=ÇáÛáÇİÇÊ
ar.tasks_languages=ÇááÛÇÊ
ar.tasks_allusers=ßÇİÉ ÇáãÓÊÎÏãíä
ar.tasks_selectusers=ãä ÃÌá %1 ÊÑßíÈ:
ar.tasks_currentuser=%1 İŞØ
ar.tasks_multisetup=Êãßíä ÇáÏÚã áÚÏÉ ãÓÊÎÏãíä
ar.tasks_firewall=ÅÖÇİÉ ÅÓÊËäÇÁ Åáì ÌÏÇÑ ÇáæíäÏæÒ ÇáäÇÑí
ar.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
ar.tasks_deleteoldsetup=ÍĞİ ãáİÇÊ ÇáÊÑßíÈ ÇáŞÏíãÉ
ar.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
ar.run_skinexe=...íÊã ÊÔÛíá ÊÑßíÈ ÇáÛáÇİ
ar.reg_incomingchat=ÑÓÇáÉ ãÍÇÏËÉ æÇÑÏÉ
ar.reg_apptitle=ÔíÑ-ÃÒÇ ááãÔÇÑßÉ ÇáÛíÑ ãÍÏæÏÉ ÈÇáãáİÇÊ
ar.icons_license=ÇáÑÎÕÉ
ar.icons_uninstall=ÅáÛÇÁ ÇáÊËÈíÊ
ar.icons_downloads=ÇáÊÍãíáÇÊ
ar.icons_basicmode=äÙÇã ÚÇÏí
ar.icons_tabbedmode=äÙÇã ãÊŞÏã
ar.icons_windowedmode=äÙÇã Ğæ äæÇİĞ
ar.dialog_shutdown=%1 íÚãá ÍÇáíÇ . åá ÊÑíÏ ÅÛáÇŞ %1 áíÓÊãÑ ÇáÊÑßíÈ ¿
ar.dialog_firewall=İÔá ÇáÅÚÏÇÏ İí ÅÖÇİÉ ÔíÑ-ÃÒÇ Åáì ÈÑäÇãÌ ÍãÇíÉ ÇáæíäÏæÒ %nÇáÑÌÇÁ ÅÖÇİÉ ÔíÑ-ÃÒÇ Åáì ŞÇÆãÉ ÇáÅÓÊËäÇÁÇÊ íÏæíÇ
ar.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
ar.page_viruswarning_text=ÚäÏãÇ ÅÓÊÚãÇá ÇáÅäÊÑäÊ ¡ íÌÈ Ãä ÊÊÃßÏ ãä æÌæÏ ÃÍÏË ÈÑäÇãÌ ááİíÑæÓ áÏíß . íãßäß ÇáÍÕæá Úáì ÈÑÇãÌ ááİíÑæÓÇÊ æ äÕÇÆÍ ÃãäíÉ ÃÎÑì áÍãÇíÉ ßãÈíæÊÑß ãä åĞå ÇáæÕáÉ:
ar.page_viruswarning_title=ÊÍĞíÑ ãä ÇáİíÑæÓÇÊ
ar.page_viruswarning_subtitle=åá ÚäÏß ÈÑäÇãÌ ááİíÑæÓÇÊ ¿
ar.CreateDesktopIcon=ÅÙåÇÑ &ÃíŞæäÉ ÓØÍ ÇáãßÊÈ
ar.CreateQuickLaunchIcon=ÅÙåÇÑ &ÃíŞæäÉ ÇáÊÔÛíá ÇáÓÑíÚ
; Catalan
ca.components_plugins=Agregats (plug-in)
ca.components_skins=Pells (skins)
ca.tasks_languages=Idiomes
ca.tasks_allusers=Tots els usuaris
ca.tasks_selectusers=Instal·lar %1 per a:
ca.tasks_currentuser=%1 únicament
ca.tasks_multisetup=Habilita el suport multi-usuari
ca.tasks_firewall=Afegeix una excepció al tallafocs del Windows
ca.tasks_upnp=Habilita el descobriment de dispositius Plug'n'Play
ca.tasks_deleteoldsetup=Esborra instal·lacions antigues
ca.tasks_resetdiscoveryhostcache=Reestableix els serveis de descobriment i caxè de servidors
ca.run_skinexe=Executant la instal·lació de la pell (skin)...
ca.reg_incomingchat=Missatge de xat entrant
ca.reg_apptitle=PeerProject: compartició d'arxius d'última generació
ca.icons_license=Llicència
ca.icons_uninstall=Desinstal·lació
ca.icons_downloads=Descàrregues
ca.icons_basicmode=Mode basico
ca.icons_tabbedmode=Mode en pestanyes
ca.icons_windowedmode=Mode de finestres
ca.dialog_shutdown=%1 està sent executat. Dessitja que %1 siga aturat per que la instal·lació puga continuar?
ca.dialog_firewall=La instal·lació ha fallat mentre s'afegia una exepció al tallafocs del Windows.%nSi us plau, afegeix PeerProject al llistat d'excepcions manualment.
ca.dialog_malwaredetected=PeerProject ha detectat un arxiu maliciós (malware) a %1. Si us plau, elimine'l amb un antivirus abans d'instal·lar. Vol sortir de la instal·lació?
ca.page_viruswarning_text=Mentre utilitzes Internet, has d'assegurar-te que tens un antivirus actualitzat per a protegir-te de troians, cucs, virus i altres programes maliciosos. Pots consultar un llistat de programari antivirus i consells de seguretat fent clic a la següent drecera:
ca.page_viruswarning_title=Advertència de virus
ca.page_viruswarning_subtitle=Tens un programa antivíric instal·lat?
ca.CreateDesktopIcon=Afegeix una icona a l'&escriptori
ca.CreateQuickLaunchIcon=Afegeix una icona a la barra d'Accés Ràpid
ca.PathNotExist=Error: la carpeta %1 no existeix.
; Czech
cz.components_plugins=Doplòky
cz.components_skins=Skiny
cz.tasks_languages=Více jazykù
cz.tasks_allusers=všechny uivatele
cz.tasks_selectusers=Instalovat %1 pro:
cz.tasks_currentuser=jen %1
cz.tasks_multisetup=Povolit pøístup více uivatelù
cz.tasks_firewall=Pøidat pravidlo do Windows firewallu
cz.tasks_upnp=Povolit zjišování Plug'n'Play zaøízení
cz.tasks_deleteoldsetup=Smazat staré instalátory
cz.tasks_resetdiscoveryhostcache=Obnovit seznamy serverù
cz.run_skinexe=Spouštím instalaci skinù...
cz.reg_incomingchat=Pøíchozí zpráva chatu
cz.reg_apptitle=PeerProject P2P Filesharing
cz.icons_license=Licence
cz.icons_uninstall=Odinstalovat
cz.icons_downloads=Staené soubory
cz.icons_basicmode=Bascíc reim
cz.icons_tabbedmode=Reim záloek
cz.icons_windowedmode=Reim oken
cz.dialog_shutdown=Aplikace %1 je spuštìna. Chcete ukonèit %1 a pokraèovat v instalaci?
cz.dialog_firewall=Instalátor nemùe pøidat pravidlo pro PeerProject do nastavení Windows Firewallu.%nProsím pøidejte pravidlo ruènì.
cz.dialog_malwaredetected=Byl nalezen škodlivı software (malware) v %1. Pøed pokraèování instalace jej prosím odstraòte pomocí vhodného nástroje pro odstraòování škodlivého softwaru. Ukonèit instalaci?
cz.page_viruswarning_text=Pøi pouívání internetu se vdy ujistìte, e máte aktuální antivirovı program, kterı vás chrání pøed nebezpeènımi a škodlivımi programy. Seznam vhodnıch programù a další rady tıkající se bezpeènosti mùete nalézt na následující adrese:
cz.page_viruswarning_title=Nalezen virus
cz.page_viruswarning_subtitle=Máte nainstalovanı antivirovı program?
cz.CreateDesktopIcon=Zobrazovat ikonu na &ploše
cz.CreateQuickLaunchIcon=Zobrazovat ikonu v panelu &Snadné spouštìní
; German
de.components_plugins=Plugins
de.components_skins=Skins
de.tasks_languages=Sprachen
de.tasks_allusers=Alle Benutzer
de.tasks_selectusers=Installieren %1 für:
de.tasks_currentuser=Nur für %1
de.tasks_multisetup=Mehrbenutzerunterstützung aktivieren
de.tasks_firewall=Eine Ausnahme zur Windows-Firewall hinzufügen
de.tasks_upnp=Automatische Routerkonfiguration benutzen (Plug'n'Play)
de.tasks_deleteoldsetup=Alte Installer löschen
de.tasks_resetdiscoveryhostcache=Service-Provider und Host-Cache zurücksetzen
de.run_skinexe=SkinInstaller einrichten...
de.reg_incomingchat=Eingehende Chatnachricht
de.reg_apptitle=PeerProjekt P2P
de.icons_license=Lizenz
de.icons_uninstall=PeerProjekt deinstallieren
de.icons_downloads=Downloads
de.icons_basicmode=Basicmodus
de.icons_tabbedmode=Tab-Modus
de.icons_windowedmode=Fenstermodus
de.dialog_shutdown=%1 wird zur Zeit ausgeführt. Wollen Sie %1 beenden, um mit der Installation fortzufahren?
de.dialog_firewall=Setup konnte PeerProjekt nicht zur Windows-Firewall hinzufügen.%nBitte tragen Sie PeerProjekt manuell in die Ausnahmenliste ein.
de.dialog_malwaredetected=Auf ihrem Computer wurde schädliche Software im Ordner %1 entdeckt. Bitte entfernen Sie die Software mit Hilfe eines Viren- bzw. Malware-Scanners bevor Sie mir der Installation von PeerProjekt fortfahren.Möchten sie die Installation jetzt abbrechen?
de.page_viruswarning_text=Wenn Sie das Internet benutzen, sollten Sie stets einen aktuellen Virenscanner installiert haben, der ihren Computer vor Trojanern, Würmern und anderen bösartigen Programmen beschützt. Sie finden eine Liste guter Virenscanner und andere Tipps, wie Sie ihren Computer schützen können, unter folgendem Link:
de.page_viruswarning_title=Virenwarnung
de.page_viruswarning_subtitle=Haben Sie ein Antivirenprogramm installiert?
de.CreateDesktopIcon=&Symbol auf dem Desktop erstellen
de.CreateQuickLaunchIcon=&PeerProjekt zur Schnellstartleiste hinzufügen
de.PathNotExist=Fehler: der Pfad zum %1-Ordner existiert nicht
; Estonian
ee.components_plugins=Pluginad
ee.components_skins=Nahad
ee.tasks_languages=Keeled
ee.tasks_allusers=Kõik kasutajad
ee.tasks_selectusers=Installi %1 jaoks:
ee.tasks_currentuser=%1 ainult
ee.tasks_multisetup=Võimalda mitmekasutaja tugi
ee.tasks_firewall=Lisa erand Windowsi Tulemüüri
ee.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
ee.tasks_deleteoldsetup=Kustuta vanad installerid
ee.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
ee.run_skinexe=Käivitan Naha installi...
ee.reg_incomingchat=Sisse tulev vestlusteade
ee.reg_apptitle=PeerProject Failijagamine
ee.icons_license=Litsents
ee.icons_uninstall=Uninstalli
ee.icons_downloads=Tõmbamised
ee.icons_basicmode=Tavaline Vaade
ee.icons_tabbedmode=Sakiline Vaade
ee.icons_windowedmode=Akendega Vaade
ee.dialog_shutdown=%1 töötab hetkel. Kas tahad  %1 sulgeda, et saaksid installeerimist jätkata?
ee.dialog_firewall=Installeril ebaõnnestus PeerProject lisamine Windowsi Tulemüüri.%Palun lisa PeerProject käsitsi erandite nimekirja.
ee.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
ee.page_viruswarning_text=Internetti kasutades peaksid kontrollima, et sul oleks uusim viirusetõrje, et kaitsta ennast troojalaste, usside, viiruste ja teiste kahjulike programmide eest. Sa leiad nimekirja headest viirus skänneritest ja teisi turva nõuandeid oma arvuti kaitseks sellelt lingilt:
ee.page_viruswarning_title=Viiruse Hoiatus
ee.page_viruswarning_subtitle=Kas sul on AntiVirus programm installeeeritud?
ee.CreateDesktopIcon=Loo &Töölaua ikoon
ee.CreateQuickLaunchIcon=Loo &Quick Launch ikoon
; Spanish
es.components_plugins=Plugins
es.components_skins=Skins
es.tasks_languages=Instalar multi-idiomas
es.tasks_allusers=Todos los usuarios
es.tasks_selectusers=Instalar %1 para:
es.tasks_currentuser=%1 solamente
es.tasks_multisetup=Habilitar soporte multi-usuario
es.tasks_firewall=Agregar una excepción al Firewall de Windows
es.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
es.tasks_deleteoldsetup=Borrar archivos de instaladores viejos
es.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
es.run_skinexe=Instalando Skin...
es.reg_incomingchat=Hay un mensaje de chat entrante
es.reg_apptitle=PeerProject P2P Filesharing
es.icons_license=Licencia
es.icons_uninstall=Desinstalar
es.icons_downloads=Descargas
es.icons_basicmode=Modo Básico
es.icons_tabbedmode=Modo Avanzado
es.icons_windowedmode=Modo Ventanas
es.dialog_shutdown=%1 se encuentra ejecutándose. ¿Deseas que %1 sea cerrado para que la instalación pueda continuar?
es.dialog_firewall=La instalación fallo al agregar la excepción de PeerProject al cortafuego Firewall.%n Por favor hágalo manualmente.
es.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
es.page_viruswarning_text=Cuando estas usando Internet, debes siempre asegurarte que tienes un antivirus actualizado hasta la fecha para protegerte de troyanos, gusanos, y otros programas maliciosos. Puedes encontrar una lista de buenos antivirus y sugerencias de seguridad para proteger tu computadora en la siguiente dirección:
es.page_viruswarning_title=Peligro de Virus
es.page_viruswarning_subtitle=¿Tienes un programa antivirus instalado?
es.CreateDesktopIcon=Mostrar icono de &Escritorio
es.CreateQuickLaunchIcon=Mostrar icono de &Inicio Rápido
; Spanish Mexican
es_mx.components_plugins=Plugins
es_mx.components_skins=Skins
es_mx.tasks_languages=Instalar multi-idiomas
es_mx.tasks_allusers=Todos los usuarios
es_mx.tasks_selectusers=Instalar %1 para:
es_mx.tasks_currentuser=%1 solamente
es_mx.tasks_multisetup=Habilitar soporte multi-usuario
es_mx.tasks_firewall=Agregar una excepción al Firewall de Windows
es_mx.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
es_mx.tasks_deleteoldsetup=Borrar archivos de instaladores viejos
es_mx.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
es_mx.run_skinexe=Instalando Skin...
es_mx.reg_incomingchat=Hay un mensaje de chat entrante
es_mx.reg_apptitle=PeerProject P2P Filesharing
es_mx.icons_license=Licencia
es_mx.icons_uninstall=Desinstalar
es_mx.icons_downloads=Descargas
es_mx.icons_basicmode=Modo Básico
es_mx.icons_tabbedmode=Modo Avanzado
es_mx.icons_windowedmode=Modo Ventanas
es_mx.dialog_shutdown=%1 se encuentra ejecutándose. ¿Deseas que %1 sea cerrado para que la instalación pueda continuar?
es_mx.dialog_firewall=La instalación fallo al agregar la excepción de PeerProject al cortafuego Firewall.%n Por favor hágalo manualmente.
es_mx.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
es_mx.page_viruswarning_text=Cuando estas usando Internet, debes siempre asegurarte que tienes un antivirus actualizado hasta la fecha para protegerte de troyanos, gusanos, y otros programas maliciosos. Puedes encontrar una lista de buenos antivirus y sugerencias de seguridad para proteger tu computadora en la siguiente dirección:
es_mx.page_viruswarning_title=Peligro de Virus
es_mx.page_viruswarning_subtitle=¿Tienes un programa antivirus instalado?
es_mx.CreateDesktopIcon=Mostrar icono de &Escritorio
es_mx.CreateQuickLaunchIcon=Mostrar icono de &Inicio Rápido
; French
fr.components_plugins=Plugins
fr.components_skins=Skins
fr.tasks_languages=Langues
fr.tasks_allusers=Tous les utilisateurs
fr.tasks_selectusers=Installer %1 pour:
fr.tasks_currentuser=%1 seulement
fr.tasks_multisetup=Activer le support multi-utilisateurs
fr.tasks_firewall=Ajouter une exception au Pare-feu Windows
fr.tasks_upnp=Activer UPnP pour essayer de découvrir les pare-feux/routeurs.
fr.tasks_deleteoldsetup=Voulez-vous supprimer les anciens fichiers d'installation de PeerProject ?
fr.tasks_resetdiscoveryhostcache=Remettre à zéro les services de découverte et le tampon pour les hôtes
fr.run_skinexe=Installation de la skin en cours...
fr.reg_incomingchat=Réception d'un message chat
fr.reg_apptitle=PeerProject P2P Filesharing
fr.icons_license=Licence
fr.icons_uninstall=Désinstaller
fr.icons_downloads=Téléchargements
fr.icons_basicmode=Mode simple
fr.icons_tabbedmode=Mode tabulé
fr.icons_windowedmode=Mode fenêtré
fr.dialog_shutdown=%1 est en cours d'exécution. Voulez-vous quitter %1 pour que l'installation puisse se poursuivre?
fr.dialog_firewall=L'installation n'a pas pu ajouter PeerProject au Pare-feu Windows.%nVeuillez ajouter PeerProject manuellement à la liste des exceptions.
fr.dialog_malwaredetected=Un logiciel malveillant a été détecté sur votre système à %1, veuillez l'enlever avec un antivirus avant d'installer PeerProject. Voulez-vous quitter maintenant ?
fr.page_viruswarning_text=Lorsque vous utilisez internet, vous devriez toujours vous assurer que vous avez un antivirus à jour pour vous protéger des troyens (trojans), vers (worms), et autres programmes malveillants. Vous pouvez trouver une liste de bons antivirus et des conseils de sécurité pour protéger votre ordinateur en suivant ce lien :
fr.page_viruswarning_title=Avertissement sur les virus
fr.page_viruswarning_subtitle=Avez-vous un antivirus installé?
fr.CreateDesktopIcon=Afficher un raccourci sur le &Bureau
fr.CreateQuickLaunchIcon=Afficher un raccourci dans la barre de &Lancement rapide
; Finnish
fi.components_plugins=Laajennukset
fi.components_skins=Ulkoasut
fi.tasks_languages=Kielet
fi.tasks_allusers=Kaikille käyttäjille
fi.tasks_selectusers=Asenna %1 käyttäjille:
fi.tasks_currentuser=%1 vain
fi.tasks_multisetup=Asenna kaikille koneen käyttäjille
fi.tasks_firewall=Lisää poikkeus Windowsin palomuuriin
fi.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
fi.tasks_deleteoldsetup=Poista vanhat asennusohjelmat
fi.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
fi.run_skinexe=Käynnissä ulkoasujen asennus...
fi.reg_incomingchat=Tuleva keskusteluviesti
fi.reg_apptitle=PeerProject jako-ohjelma
fi.icons_license=Lisenssi
fi.icons_uninstall=Poista
fi.icons_downloads=Lataukset
fi.icons_basicmode=Basicaali Tila
fi.icons_tabbedmode=Välilehti Tila
fi.icons_windowedmode=Ikkunoitu Tila
fi.dialog_shutdown=%1 on tällä hetkellä käynnissä. Haluatko että %1 suljetaan jotta asennus voisi jatkua?
fi.dialog_firewall=Asentaja epäonnistui lisätessään PeerProjecta Windowsiin Firewall.%nOle hyvä ja lisää PeerProject poikkeuslistaan manuaalisesti.
fi.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
fi.page_viruswarning_text=Kun käytät internetiä, sinun tulee aina varmistaa että sinulla on viimeisimmät päivitykset virusohjelmissasi jotka suojaavat sinua troijalaisilta, madoilta, ja muilta haittaohjelmilta. Löydät hyvän listan hyvistä virusohjelmista ja turvallisuusvinkkejä seuraavista linkeistä:
fi.page_viruswarning_title=Virus Varoitus
fi.page_viruswarning_subtitle=Onko sinulla AntiVirus ohjelmaa asennettuna?
fi.CreateDesktopIcon=Luo kuvake työpöydälle
fi.CreateQuickLaunchIcon=Luo kuvake pikakäynnistyspalkkiin
; Greek
gr.components_plugins=Plugins
gr.components_skins=Skins
gr.tasks_languages=Ãëşóóåò
gr.tasks_allusers=¼ëïé ïé ÷ñŞóôåò
gr.tasks_selectusers=ÅãêáôÜóôáóç %1 ãéá:
gr.tasks_currentuser=%1 ìüíï
gr.tasks_multisetup=Åíåñãïğïßçóç ôçò âïŞèåéáò ğïëëáğëşí ÷ñçóôşí
gr.tasks_firewall=ÂÜëå ìéá åîáßñåóç óôï ôåß÷ïò ğñïóôáóßáò ôùí Windows
gr.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
gr.tasks_deleteoldsetup=Äéİãñáøå ôçí ğáëéÜ åãêáôÜóôáóç
gr.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
gr.run_skinexe=Running ÅãêáôÜóôáóç ôïõ skin...
gr.reg_incomingchat=Íİï ìŞíõìá chat
gr.reg_apptitle=PeerProject P2P Filesharing
gr.icons_license=¢äåéá
gr.icons_uninstall=ÁğåãêáôÜóôáóç
gr.icons_downloads=Êáôİâáóìá
gr.icons_basicmode=ÊáíïíéêŞ ÌïñöŞ
gr.icons_tabbedmode=ĞáñáèõñéêŞ ÌïñöŞ
gr.icons_windowedmode=ÌïñöŞ Windowed
gr.dialog_shutdown=Ôï %1 áêüìá ôñİ÷åé. Èİëåôå íá ôåñìáôßóåôå ôçí ëåéôïõñãåßá ôïõ %1 ãéá íá óõíå÷éóôåß ç åãêáôÜóôáóç?
gr.dialog_firewall=Ç åãêáôÜóôáóç ôïõ ğñïãñÜììáôïò áğİôõ÷å íá ğñïóèİóåé ôï PeerProject óôï Windows Firewall. % Ğáñáêáëş ğñïóèİóôå ôï PeerProject óôçí exception ëßóôá ÷åéñïêßíçôá
gr.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
gr.page_viruswarning_text=¼ôáí ÷ñçóéìïğïéåßôå ôï internet, èá ğñİğåé ğÜíôá íá İ÷åôå İíá ğñüãñáììá ğñïóôáóßáò ãéá éïıò åíçìåñùìİíï ãéá íá óáò ğñïóôáôåıåé áğï áõôïıò êáé Üëëá åğéêßíäõíá ğñïãñÜììáôá. Ìğïñåßôå íá âñåßôå ìéá ëßóôá ìå êáëÜ ğñïãñÜììáôá ğñïóôáóßáò ãéá éïıò êáé Üëëá âïçèŞìáôá ãéá íá ğñïóôáôİøåôå ôïí õğïëïãéóôŞ óáò áêïëïõèşíôáò áõôüí ôïí óıíäåóìï:
gr.page_viruswarning_title=Ğñïåéäïğïßçóç ãéá éü
gr.page_viruswarning_subtitle=¸÷åôå İíá ğñüãñáììá ğñïóôáóßáò ãéá éïıò åãêáôåóôçìİíï?
gr.CreateDesktopIcon=ÅìöÜíéóå ôï &åéêïíßäéï óôçí åğéöÜíåéá åñãáóßáò
gr.CreateQuickLaunchIcon=ÅìöÜíéóå İíá å&éêïíßäéï ÃñŞãïñçò Åêêßíçóçò
; Hebrew
heb.components_plugins=úåñôéí
heb.components_skins=îòèôåú
heb.tasks_languages=ùôåú
heb.tasks_allusers=ëì äîùúîùéí
heb.tasks_selectusers=äú÷ï àú %1 òáåø
heb.tasks_currentuser=%1 ø÷
heb.tasks_multisetup=àôùø úîéëä ìîùúîùéí îøåáéí
heb.tasks_firewall=äåñó ëìì ìçåîú äàù ìàéôùåø ùøæä
heb.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
heb.tasks_deleteoldsetup=îç÷ äú÷ğåú éùğåú
heb.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
heb.run_skinexe=îøéõ äú÷ğú îòèôåú...
heb.reg_incomingchat=äåãòú ö'àè ğëğñú
heb.reg_apptitle=ùéúåó ä÷áöéí äàåìèéîèéáé ùì ùøæä
heb.icons_license=øùéåï
heb.icons_uninstall=äñø äú÷ğä
heb.icons_downloads=äåøãåú
heb.icons_basicmode=îöá øâéì
heb.icons_tabbedmode=îöá èàáéí
heb.icons_windowedmode=îöá çìåğåú
heb.dialog_shutdown=?ëøâò %1 ôåòì äàí àú øåöä ìñâåø àú %1 ëê ùääú÷ğä úåëì ìäîùê
heb.dialog_firewall=ääú÷ğä ğëùìä ìäåñéó àú ùøæä àì çåîú äàù%nàğà äåñó àú ùøæä ìøùéîú äçøéâéí áçåîú äàù áàåôï éãğé
heb.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
heb.page_viruswarning_text=ëùàú/ä îùúîù áàéğèøğè òìééê úîéã ìååãà ùîåú÷ï àöìê àğèé-åéøåñ îòåãëï ìäâğä îôğé åéøåñéí/úåìòéí/øåâìåú, ìøùéîä ùì àğèé-åéøåñéí åèéôéí áğåùà àáèçä ôğä ì÷éùåø äáà:
heb.page_viruswarning_title=äúøàú åéøåñ
heb.page_viruswarning_subtitle=?äàí éù ìê úåëğú àğèé-åéøåñ îåú÷ğú
heb.CreateDesktopIcon= äöâ ñîì òì ù&åìçï òáåãä
heb.CreateQuickLaunchIcon=äöâ ñîì òì ä&ôòìä îäéøä
; Hungarian
hu.components_plugins=Pluginek
hu.components_skins=Kinézetek
hu.tasks_languages=Nyelvek telepítése
hu.tasks_allusers=Minden felhasználó
hu.tasks_selectusers=Megadott felhaszáló:
hu.tasks_currentuser=Jelenlegi felhasználó
hu.tasks_multisetup=Többfelhasználós mód engedélyezése
hu.tasks_firewall=Felvétel a Windows tûzfal kivételeihez
hu.tasks_upnp=Automatikus routerbeállítás (UPnP szolgáltatás)
hu.tasks_deleteoldsetup=Régi telepítõk törlése
hu.tasks_resetdiscoveryhostcache=A Szerverkeresõ és a Kiszolgálók listájának alaphelyzetbe állítása
hu.run_skinexe=Kinézet telepítése folyamatban...
hu.reg_incomingchat=Bejövõ chat üzenet
hu.reg_apptitle=PeerProject fájlmegosztó program
hu.icons_license=Licenc
hu.icons_uninstall=PeerProject eltávolítása
hu.icons_downloads=Letöltések
hu.icons_basicmode=Egyszerû nézet
hu.icons_tabbedmode=Összetett nézet
hu.icons_windowedmode=Ablakos nézet
hu.dialog_shutdown=A %1 jelenleg fut. Be akarja zárni a programot, hogy a telepítés folytatódhasson?
hu.dialog_firewall=A telepítõ nem tudta hozzáadni a PeerProject a Windows tûzfal kivételeihez.%nManuálisan kell ezt megtenni.
hu.dialog_malwaredetected=A telepítõ egy kártékony programot talált a rendszerben: %1. Mielõtt telepítené a PeerProject, elõbb távolítsa el azt. Ki akar most lépni?
hu.page_viruswarning_text=Az internet használata során mindig legyen feltelepítve egy, a legfrissebb vírusadatbázissal rendelkezõ antivírus program, ami megvéd a férgektõl, trójai és egyéb kártékony programoktól. Ha követi ezt a linket, sok jó víruskeresõt találhat és további hasznos tippeket kaphat a számítógép védelmérõl:
hu.page_viruswarning_title=Vírusveszély
hu.page_viruswarning_subtitle=Van feltelepített antivírus programja?
hu.CreateDesktopIcon=Ikon létrehozása az &Asztalon
hu.CreateQuickLaunchIcon=Ikon létrehozása a &Gyorsindítás eszköztáron
hu.PathNotExist=Hiba, a megadott %1 mappa nem létezik
; Italian
it.components_plugins=Plugin
it.components_skins=Skin
it.tasks_languages=Multi-lingua
it.tasks_allusers=Tutti gli utenti
it.tasks_selectusers=Installa %1 per:
it.tasks_currentuser=Solo %1
it.tasks_multisetup=Abilita il supporto multi-utente
it.tasks_firewall=Aggiungi un'eccezione a Windows Firewall
it.tasks_upnp=Abilita il rilevamento dei dispositivi Plug'n'Play
it.tasks_deleteoldsetup=Cancella installer vecchi
it.tasks_resetdiscoveryhostcache=Resetta i servizi di connessione e la cache host
it.run_skinexe=Installazione delle skin in corso...
it.reg_incomingchat=Messaggio di chat in arrivo
it.reg_apptitle=PeerProject, il programma definitivo di P2P
it.icons_license=Licenza
it.icons_uninstall=Disinstalla
it.icons_downloads=Download
it.icons_basicmode=Modalità basico
it.icons_tabbedmode=Modalità a tabelle
it.icons_windowedmode=Modalità a finestre
it.dialog_shutdown=%1 è attualmente in esecuzione. Vuoi che %1 sia chiuso così l'installazione può continuare?
it.dialog_firewall=Impossibile aggiungere PeerProject a Windows Firewall.%nAggiungi PeerProject alla lista delle eccezioni manualmente.
it.dialog_malwaredetected=Un programma malevolo (malware) è stato rilevato sul tuo sistema in %1, rimuovilo con un programma anti-virus/anti-malware prima di installare PeerProject. Vuoi uscire adesso?
it.page_viruswarning_text=Quando usi internet, dovresti sempre assicurarti di aver un antivirus aggiornato per proteggerti dai trojan, worm e dagli altri programmi malevoli. Puoi trovare una lista di buoni antivirus e altri suggerimenti di sicurezza per proteggere il tuo computer seguendo questo link:
it.page_viruswarning_title=Attenzione ai virus
it.page_viruswarning_subtitle=Hai installato un programma antivirus?
it.CreateDesktopIcon=Visualizza un'icona sul &desktop
it.CreateQuickLaunchIcon=Visualizza un'icona in &Avvio veloce
it.PathNotExist=Errore, il percorso della cartella %1 non esiste
; Lithuanian
lt.components_plugins=Papildiniai
lt.components_skins=Apvalkalai
lt.tasks_languages=Kalbos
lt.tasks_allusers=Visiems vartotojams
lt.tasks_selectusers=Ádiegti „%1“ ğiems vartotojams:
lt.tasks_currentuser=Tik vartotojui %1
lt.tasks_multisetup=Ágalinti keliø vartotojø palaikymà
lt.tasks_firewall=Pridëti prie Windows ugniasienës iğimèiø sàrağo
lt.tasks_upnp=Ágalinti Plug'n'Play árenginiø aptikimà
lt.tasks_deleteoldsetup=Trinti senas diegimo programas
lt.tasks_resetdiscoveryhostcache=Apnulinti tarnybas ir adresatø talpyklas
lt.run_skinexe=Vykdoma apvalkalo sàranka...
lt.reg_incomingchat=Gaunama şinutë pokalbiui
lt.reg_apptitle=PeerProject — geriausia programa bylø mainams
lt.icons_license=Licencinë sutartis
lt.icons_uninstall=Pağalinti
lt.icons_downloads=Atsisiuntimai
lt.icons_basicmode=Áprastinë veiksena
lt.icons_tabbedmode=Korteliø veiksena
lt.icons_windowedmode=Langø veiksena
lt.dialog_shutdown=„%1“ ğiuo metu dirba. Ar norite nutraukti „%1“ darbà, tam kad bûtø galima tæsti ádiegimà?
lt.dialog_firewall=Ádiegimo programai nepavyko pridëti „PeerProject“ prie Windows ugniasienës iğimèiø sàrağo.%nPridëkite jà á ğá sàrağà rankiniu bûdu.
lt.dialog_malwaredetected=Jûsø sistemoje aptiktos kenkëjiğkos programos (%1). Prieğ diegdami „PeerProject“, pağalinkite jas antivirusine programa ar skeneriu. Nutraukti diegimà dabar?
lt.page_viruswarning_text=Visada, kai naudojatës Internetu, ásitikinkite, jog turite naujausià virusø skenerá, tam kad bûtumëte apsaugoti nuo trojanø, kirminø ir kitokiø kenkëjiğkø programø. Jûs galite rasti gerø virusø skeneriø sàrağà ir kitokius kompiuterio apsaugojimo patarimus nuëjæ ğiuo adresu:
lt.page_viruswarning_title=Áspëjimas apie virusus
lt.page_viruswarning_subtitle=Ar Jûs turite ásidiegæ antivirusinæ programà?
lt.CreateDesktopIcon=Rodyti piktogramà &Darbalaukyje
lt.CreateQuickLaunchIcon=Rodyti Sparèiosios &Paleisties piktogramà
lt.PathNotExist=Klaida, katalogo kelias „%1“ neegzistuoja
; Dutch
nl.components_plugins=Plugins
nl.components_skins=Skins
nl.tasks_languages=Talen
nl.tasks_allusers=Alle gebruikers
nl.tasks_selectusers=Installeer %1 voor:
nl.tasks_currentuser=Alleen %1
nl.tasks_multisetup=Ondersteuning voor meerdere gebruikers inschakelen
nl.tasks_firewall=Een uitzondering aan de Windows Firewall toevoegen
nl.tasks_upnp=Configureer automatisch mijn router (Plug'n'Play)
nl.tasks_deleteoldsetup=Oude installatieprogramma's wissen
nl.tasks_resetdiscoveryhostcache=Herstel de Discovery- en Hostcachelist
nl.run_skinexe=Skin installatie uitvoeren...
nl.reg_incomingchat=Nieuw chat bericht
nl.reg_apptitle=PeerProject P2P FileSharer
nl.icons_license=Gebruiksovereenkomst
nl.icons_uninstall=Verwijderen
nl.icons_downloads=Downloads
nl.icons_basicmode=Basic Stijl
nl.icons_tabbedmode=Tabblad Stijl
nl.icons_windowedmode=Venster Stijl
nl.dialog_shutdown=%1 is momenteel open. Wil je %1 afsluiten zodat de installatie verder kan gaan?
nl.dialog_firewall=Het installatieprogramma kon PeerProject niet toevoegen aan de Windows Firewall.%nVoeg PeerProject alstublieft manueel toe aan de uitzonderingenlijst.
nl.dialog_malwaredetected=Er werd malware gevonden op %1, verwijder het alstublieft met een virus/malware scanner vooraleer PeerProject verder te installeren. Wil je nu sluiten?
nl.page_viruswarning_text=Als u het internet gebruikt moet u een recente virusscanner gebruiken om u te beschermen tegen virussen, wormen en andere kwaadaardige programma's. U kan een lijst van virusscanners en andere veiligheidstips vinden om uw computer the beschermen door deze link te volgen:
nl.page_viruswarning_title=Virus Waarschuwing
nl.page_viruswarning_subtitle=Heeft u een antivirus programma geïnstalleerd?
nl.CreateDesktopIcon=Toon een snelkoppeling op het &bureaublad
nl.CreateQuickLaunchIcon=Toon een snelkoppeling op de &Snel Starten werkbalk
; Norwegian
no.components_plugins=Plugins
no.components_skins=Skins
no.tasks_languages=Språk
no.tasks_allusers=Alle brukere
no.tasks_selectusers=Installer %1 for:
no.tasks_currentuser=Kun %1
no.tasks_multisetup=Flere brukere
no.tasks_firewall=Lag nytt unntak i Windows brannmur
no.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
no.tasks_deleteoldsetup=Slett eldre installasjonsfiler
no.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
no.run_skinexe=Kjører skin installasjon...
no.reg_incomingchat=Innkommende melding
no.reg_apptitle=PeerProject P2P Filesharing
no.icons_license=Lisens
no.icons_uninstall=Uninstall
no.icons_downloads=Downloads
no.icons_basicmode=Basicmodus
no.icons_tabbedmode=Fanemodus
no.icons_windowedmode=Vindumodus
no.dialog_shutdown=%1 kjører. Ønsker du at %1 avsluttes slik at installasjonen kan fortsette?
no.dialog_firewall=Installasjonen klarte ikke å lage unntak for PeerProject i Windows Brannmuren. %nVennligst legg til PeerProject i brannmurens unntak manuelt.
no.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
no.page_viruswarning_text=Når du bruker internett bør du alltid ha et oppdatert antivirus-program, for å beskytte deg fra trojaner, ormer, og annen skadelig programvare. Du kan finne en liste over gode antivirus-prgrammer og andre sikkerhetstips, for å beskytte din datamaskin, ved å følge denne linken:
no.page_viruswarning_title=Virusadvarsel
no.page_viruswarning_subtitle=Har du et antivirus-program installert?
no.CreateDesktopIcon=Vis ikon på &skrivebordet
no.CreateQuickLaunchIcon=Vis et &Hurtigstarts-ikon
; Polish
pl.components_plugins=Wtyczki
pl.components_skins=Skórki
pl.tasks_languages=Jêzyki
pl.tasks_allusers=Dla wszystkich u¿ytkowników
pl.tasks_selectusers=Instaluj dla %1:
pl.tasks_currentuser=tylko %1
pl.tasks_multisetup=W³¹cz obs³ugê wielu u¿ytkowników
pl.tasks_firewall=Dodaj wyj¹tek do Firewall'a Windows'a
pl.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
pl.tasks_deleteoldsetup=Usuñ stare instalatory
pl.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
pl.run_skinexe=Instalowanie skórek...
pl.reg_incomingchat=Przychodz¹ca wiadomoœæ chatowa
pl.reg_apptitle=PeerProject P2P Filesharing
pl.icons_license=Licencja
pl.icons_uninstall=Odinstaluj
pl.icons_downloads=Pobierania
pl.icons_basicmode=Tryb basic
pl.icons_tabbedmode=Tryb zaawansowany
pl.icons_windowedmode=Tryb okienkowy
pl.dialog_shutdown=%1 obecnie dzia³a. Czy chcia³byœ wy³¹czyæ %1 aby kontynuowaæ instalacjê?
pl.dialog_firewall=Instalator nie potrafi³ dodaæ PeerProejct do Firewall'a Windows'a.%nProszê dodaæ PeerProject do listy wyj¹tków rêcznie.
pl.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
pl.page_viruswarning_text=Podczas u¿ywania internetu zawsze powinieneœ byæ pewny, ¿e masz program antywirusowy z aktualn¹ baz¹ wirusów, który ciê ochroni przed trojanami, robakami i innym niebezpiecznym oprogramowaniem. Mo¿esz znaleŸæ listê dobrych programów antywirusowych i porady jak zabezpieczyæ komputer pod nastêpuj¹cymi adresami:
pl.page_viruswarning_title=Ostrze¿enie przed wirusem
pl.page_viruswarning_subtitle=Czy masz zainstalowany jakiœ program antywirusowy?
pl.CreateDesktopIcon=Wyœwietl ikonê na pulpicie
pl.CreateQuickLaunchIcon=Wyœwietl ikonê na pasku szybkiego uruchamiania
; Portuguese std
pt.components_plugins=Plugins
pt.components_skins=Skins
pt.tasks_languages=Linguagens
pt.tasks_allusers=Todos os usuários
pt.tasks_selectusers=Instalar %1 para:
pt.tasks_currentuser=somente %1
pt.tasks_multisetup=Ativar o suporte a vários usuários
pt.tasks_firewall=Adicionar exceção ao Firewall do Windows
pt.tasks_upnp=Ativar a descoberta de dispositivos Plug'n'Play
pt.tasks_deleteoldsetup=Apagar os instaladores antigos
pt.tasks_resetdiscoveryhostcache=Resetar o Cache de Armazenamento e Descoberta
pt.run_skinexe=Instalando a Skin...
pt.reg_incomingchat=Mensagem de chat
pt.reg_apptitle=PeerProject Compartilhamento de Arquivos Incomparável
pt.icons_license=Licença
pt.icons_uninstall=Desinstalar
pt.icons_downloads=Downloads
pt.icons_basicmode=Modo Basic
pt.icons_tabbedmode=Modo de Abas
pt.icons_windowedmode=Modo de Janelas
pt.dialog_shutdown=O %1 está sendo executado. Você gostaria que o %1 fosse fechado para que a instalação continue?
pt.dialog_firewall=O Setup falhou em adicionar o PeerProject no Firewall do Windows.%nPor favor adicione o PeerProject a lista de exceções manualmente.
pt.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
pt.page_viruswarning_text=Quando usando a internet, você deve sempre garantir que você tenha um scanner contra vírus atualizado para proteger você de trojans, worms e outros programas maliciosos. Você pode achar uma lista de bons scanners contra vírus e outras dicas de segurança para proteger o seu computador seguindo este link:
pt.page_viruswarning_title=Alerta contra Vírus
pt.page_viruswarning_subtitle=Você tem um programa Anti-Vírus instalado?
pt.CreateDesktopIcon=Criar ícone no &Ambiente de Trabalho
pt.CreateQuickLaunchIcon=Criar ícone na barra de Inicialização &Rápida
; Portuguese-Brazil
pt_br.components_plugins=Plugins
pt_br.components_skins=Skins
pt_br.tasks_languages=Multi-idioma
pt_br.tasks_allusers=Todos os Usuários
pt_br.tasks_selectusers=Instalar o %1 para:
pt_br.tasks_currentuser=só para o %1
pt_br.tasks_multisetup=Ativar suporte para vários usuários
pt_br.tasks_firewall=Adicionar uma exceção ao Firewall do Windows
pt_br.tasks_upnp=Ativar o descobrimento de dispositivos Plug'n'Play
pt_br.tasks_deleteoldsetup=Apagar os instaladores antigos
pt_br.tasks_resetdiscoveryhostcache=Resetar o Descobrimento e o Cache dos Hosts
pt_br.run_skinexe=Executando a instalação das skins...
pt_br.reg_incomingchat=Mensagem de chat a entrar
pt_br.reg_apptitle=PeerProject Compartilhador de Arquivo
pt_br.icons_license=Licença
pt_br.icons_uninstall=Desinstalar
pt_br.icons_downloads=Downloads
pt_br.icons_basicmode=Modo Simples
pt_br.icons_tabbedmode=Modo em Abas
pt_br.icons_windowedmode=Modo de Janelas
pt_br.dialog_shutdown=O %1 está rodando atualmente. Você gostaria que o %1 fosse fechado para que a instalação possa continuar?
pt_br.dialog_firewall=A instalação falhou ao tentar adicionar o PeerProject a lista de exceções do Firewall do Windows.%nPor favor adicione o PeerProject a lista de exceções manualmente.
pt_br.dialog_malwaredetected=Um malware foi detectado no seu sistema em %1, por favor remova-o com um scanner contra vírus/malware antes de instalar o PeerProject. Você quer sair agora?
pt_br.page_viruswarning_text=Quando usar a Internet você deve sempre garantir que você tenha um scanner contra vírus atualizado para proteger você de trojans, worms e outros programas maliciosos. Você pode achar uma lista de bons scanners contra vírus e outras dicas de segurança para proteger seu computador seguindo este link:
pt_br.page_viruswarning_title=Aviso sobre_Vírus
pt_br.page_viruswarning_subtitle=Você tem um programa anti-vírus instalado?
pt_br.CreateDesktopIcon=Exibir um ícone na &área de trabalho
pt_br.CreateQuickLaunchIcon=Exibir um ícone na barra de &Inicialização Rápida
pt_br.PathNotExist=Erro, o caminho da pasta %1 não existe
; Russian
ru.components_plugins=Ìîäóëè
ru.components_skins=Øêóğêè
ru.tasks_languages=ßçûêè
ru.tasks_allusers=Âñåì ïîëüçîâàòåëÿì
ru.tasks_selectusers=Óñòàíîâèòü %1 äëÿ:
ru.tasks_currentuser=Òîëüêî äëÿ %1
ru.tasks_multisetup=Ğàçğåøèòü ïîääåğæêó íåñêîëüêèõ ïîëüçîâàòåëåé
ru.tasks_firewall=Äîáàâèòü â ñïèñîê èñêëş÷åíèé áğàíìàóığà Windows
ru.tasks_upnp=Âêëş÷èòü íàõîæäåíèå óñòğîéñâ Plug'n'Play
ru.tasks_deleteoldsetup=Óäàëèòü ñòàğûå èíñòàëëÿòîğû
ru.tasks_resetdiscoveryhostcache=Ñáğîñèòü ğåñóğñû ñîåäèíåíèé è êıø õîñòîâ
ru.run_skinexe=Èä¸ò óñòàíîâêà øêóğîê...
ru.reg_incomingchat=Âõîäÿùåå ñîîáùåíèå äëÿ ÷àòà
ru.reg_apptitle=PeerProject - ïğîãğàììà äëÿ îáìåíà ôàéëàìè
ru.icons_license=Ëèöåíçèÿ
ru.icons_uninstall=Äåèíñòàëëÿöèÿ
ru.icons_downloads=Çàãğóçêè
ru.icons_basicmode=Îáû÷íûé ğåæèì
ru.icons_tabbedmode=Ğåæèì çàêëàäîê
ru.icons_windowedmode=Îêîííûé ğåæèì
ru.dialog_shutdown=%1 â äàííûé ìîìåíò ğàáîòàåò. Õîòèòå ëè çàâåğøèòü ğàáîòó %1, ÷òîáû ïğîäîëæèòü óñòàíîâêó?
ru.dialog_firewall=Ïğîãğàììà óñòàíîâêè íå ñìîãëà äîáàâèòü PeerProject â ñïèñîê èñêëş÷åíèé áğàíäìàóığà Windows.%nÏîæàëóéñòà, äîáàâüòå åå â ıòîò ñïèñîê âğó÷íóş.
ru.dialog_malwaredetected=Â Âàøåé ñèñòåìå îáíàğóæåííû âğåäîíîñíûå ïğîãğàììû (%1), óäàëèòå èõ ïåğåä èíñòàëëÿöèåé PeerProject ïğè ïğîìîùè àíòèâèğóñíîé ïğîãğàììû èëè ñêåíåğà. Õîòèòå âûéòè ñåé÷àñ?
ru.page_viruswarning_text=Âñåãäà, êîãäà ïîëüçóåòåñü èíòåğíåòîì, óäîñòîâåğüòåñü, ÷òî ó Âàñ åñòü íîâåéøèé ñêàíåğ äëÿ âèğóñîâ, ÷òîáû çàùèòèòü êîìïüşòåğ îò òğîÿíîâ, ÷åğâåé è äğóãèõ âğåäèòåëüñêèõ ïğîãğàìì. Âû ìîæåòå íàéòè ñïèñîê õîğîøèõ ñêàíåğîâ äëÿ âèğóñîâ è äğóãèå ñîâåòû î çàùèòå êîìïüşòåğà ïî ıòîìó àäğåñó:
ru.page_viruswarning_title=Ïğåäîñòåğåæåíèå î âèğóñàõ
ru.page_viruswarning_subtitle=Èìååòå ëè Âû óñòàíîâëåííóş àíòèâèğóñíóş ïğîãğàììó?
ru.CreateDesktopIcon=Ïîêàçûâàòü çíà÷îê íà &Ğàáî÷åì ñòîëå
ru.CreateQuickLaunchIcon=Ïîêàçûâàòü çíà÷îê â &Ïàíåëè áûñòğîãî çàïóñêà
ru.PathNotExist=Îøèáêà, ïóòü ê ïàïêå %1 íå ñóùåñòâóåò
; Slovenian
sl.components_plugins=Vtièniki
sl.components_skins=Preobleke
sl.tasks_languages=Jeziki
sl.tasks_allusers=Vsi uporabniki
sl.tasks_selectusers=Namesti %1 za:
sl.tasks_currentuser=Samo %1
sl.tasks_multisetup=Omogoèi veè-uporabniško podporo
sl.tasks_firewall=Dodaj izjemo v Windows poarni zid
sl.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
sl.tasks_deleteoldsetup=Briši stare namešèevalce
sl.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
sl.run_skinexe=Namestitev preobleke v teku...
sl.reg_incomingchat=Vhodno kramljalno sporoèilo
sl.reg_apptitle=Izjemno PeerProject deljenje datotek
sl.icons_license=Licenca
sl.icons_uninstall=Odnamestitev
sl.icons_downloads=Prenosi
sl.icons_basicmode=Navadni naèin
sl.icons_tabbedmode=Naèin z zavihki
sl.icons_windowedmode=Naèin z okni
sl.dialog_shutdown=%1 je trenutno v teku. Ali elite zapreti %1 zato, da se lahko nadaljuje namestitev?
sl.dialog_firewall=Program namestitve ni uspel dodati PeerProject v poarni zid Windows-ov.%nPeerProject boste morali roèno dodati v seznam izjem v poarnem zidu.
sl.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
sl.page_viruswarning_text=Pri uporabi medmreja imejte namešèeno vedno najnovejšo razlièico protivirusne zašèite. Tako boste kar najbolje zašèiteni pred trojanskimi konji, èrvi in drugimi zlonamernimi programji. Seznam dobrih protivirusnih programov, ter drugih nasvetov glede zašèite vašega raèunalnika, boste našli preko naslednje spletne povezave:
sl.page_viruswarning_title=Virusno opozorilo
sl.page_viruswarning_subtitle=Ali imate namešèen protivirusni program?
sl.CreateDesktopIcon=Prikai ikono &namizja
sl.CreateQuickLaunchIcon=Prikai ikono &Hitrega zaganjalnika
; Albanian
sq.components_plugins=Spina
sq.components_skins=Veshje
sq.tasks_languages=Shumë gjuhë
sq.tasks_allusers=Tërë përdoruesit
sq.tasks_selectusers=Instalo %1 për:
sq.tasks_currentuser=%1 vetëm
sq.tasks_multisetup=Lejo përkrahjen më shumë përdorues
sq.tasks_firewall=Shto një përjashtim te Fajruolli i Windows-it
sq.tasks_upnp=Lejo zbulimin e pajisjeve Plug'n'Play
sq.tasks_deleteoldsetup=Fshiji instaluesit e vjetër
sq.tasks_resetdiscoveryhostcache=Rivendos Zbulimin dhe Depon me Hostet
sq.run_skinexe=Vepron instalimi i veshjes...
sq.reg_incomingchat=Mesazh hyrës në chat
sq.reg_apptitle=Shpërndarja më e Lartë e Skedave me PeerProject
sq.icons_license=Licencë
sq.icons_uninstall=Çinstalo
sq.icons_downloads=Shkarkesa
sq.icons_basicmode=Mënyrë me Thjeshtë
sq.icons_tabbedmode=Mënyrë me Fusha
sq.icons_windowedmode=Mënyrë me Dritare
sq.dialog_shutdown=%1 aktualisht po vepron. A do e mbyllësh %1, që të vijosh me instalimin?
sq.dialog_firewall=Sistemimi dështoi ta shtojë PeerProject-n te Fajruolli i Windows-it.%nTë lutem shtoje vetë PeerProject-n te lista e përjashtimeve.
sq.dialog_malwaredetected=Te sistemi yt në %1 diktohet një keqbërës. Të lutem fshije atë me një skanues virusi/keqbërësi para se të instalohet PeerProject. Do e mbyllësh tani?
sq.page_viruswarning_text=Kur përdor internetin duhet të sigurohesh që ke një skanues virusi të ridatuar që të mbrohesh nga trojanët, krimbat dhe programet e tjera keqbërëse. Nëse ndjek këtë link mund të gjesh një listë me skanuesa virusi të mirë dhe këshilla të tjera sigurie se si mbrohet kompjuteri:
sq.page_viruswarning_title=Lajmërim Virusi
sq.page_viruswarning_subtitle=A ke instaluar një program AntiVirus?
sq.CreateDesktopIcon=Trego një ikonë në &tryezë
sq.CreateQuickLaunchIcon=Trego një ikonë në &Quick Launch
sq.PathNotExist=Gabim, shtegu i dosjes së %1 nuk ekziston!
; Serbian
sr.components_plugins=Pluginovi
sr.components_skins=Skinovi
sr.tasks_languages=Jezici
sr.tasks_allusers=Svi korisnici
sr.tasks_selectusers=Instaliraj %1 za:
sr.tasks_currentuser=%1 samo
sr.tasks_multisetup=Omoguæi više-korisnièku podršku
sr.tasks_firewall=Dodaj izuzetak u Windows Vatrozid
sr.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
sr.tasks_deleteoldsetup=Ukloni stare instalere
sr.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
sr.run_skinexe=Instalacija skina u toku...
sr.reg_incomingchat=Dolazeæe cet poruke
sr.reg_apptitle=PeerProject Najbolji P2P za Deljenje Datoteka
sr.icons_license=Licenca
sr.icons_uninstall=Ukloni Program
sr.icons_downloads=Skinutno
sr.icons_basicmode=Basic Prikaz
sr.icons_tabbedmode=Prikaz s Karticama
sr.icons_windowedmode=U Prozoru
sr.dialog_shutdown=%1 je ukljuèena. Da li bi eleli da %1 bude ugašena da bi se instalacija nastavila?
sr.dialog_firewall=Instalacija nije uspla da doda PeerProject-u u Windows Vatrozid.%nMolimo dodajte PeerProject-u na listu izuzetaka ruèno.
sr.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
sr.page_viruswarning_text=Kada koristite internet, trebali bi uvek da budete sigurni da imate auriaran virus skener koji Vas štiti od trojanaca, crva, i drugih zlonamernih programa. Moete pronaæi listu dobrih anti-virus programa i drugih sigurnosnih saveta kako da zaštitite Vaš raèunar prateæi ovaj link:
sr.page_viruswarning_title=Virus Uopzorenje
sr.page_viruswarning_subtitle=Da li imate AntiVirus program instaliran?
sr.CreateDesktopIcon=Napravi &desktop ikonu
sr.CreateQuickLaunchIcon=Napravi &Brzo Pokretanje(QL) ikonu
; Swedish
sv.components_plugins=Insticksprogram
sv.components_skins=Skinn
sv.tasks_languages=Språk
sv.tasks_allusers=Alla användare
sv.tasks_selectusers=Installera %1 för:
sv.tasks_currentuser=%1 endast
sv.tasks_multisetup=Aktivera stöd för flera användare
sv.tasks_firewall=Lägg till ett undantag till Windows brandvägg
sv.tasks_upnp=Tillåt upptäckt av UPnP-enheter
sv.tasks_deleteoldsetup=Radera gamla installationer
sv.tasks_resetdiscoveryhostcache=Återställ upptäckstjänster och värdcache
sv.run_skinexe=Kör skinninstallation...
sv.reg_incomingchat=Inkommande chattmeddelande
sv.reg_apptitle=PeerProject Fildelning
sv.icons_license=Licens
sv.icons_uninstall=Avinstallera
sv.icons_downloads=Nedladdningar
sv.icons_basicmode=Basic läge
sv.icons_tabbedmode=Tabbläge
sv.icons_windowedmode=Fönsterläge
sv.dialog_shutdown=%1 körs för tillfället. Vill du att %1 ska stängas ned så att installationen kan fortsätta?
sv.dialog_firewall=Installationsprogrammet lyckades inte lägga till PeerProject till brandväggen i Windows .%nLägg till PeerProject till undantagslistan manuellt.
sv.dialog_malwaredetected=Skadlig programvara har hittats på din dator vid %1, ta bort den med ett antivirusprogram innan du installerar PeerProject. Vill du avbryta nu?
sv.page_viruswarning_text=När du använder internet ska du alltid försäkra dig om att du har ett uppdaterat antivirusprogram som skyddar dig mot trojaner, maskar och andra skadliga program. Här finns en lista på bra antivirusprogram och andra säkerhetstips för att skydda din dator:
sv.page_viruswarning_title=Virusvarning
sv.page_viruswarning_subtitle=Har du ett antivirusprogram installerat?
sv.CreateDesktopIcon=Skapa en ikon på srivbordet
sv.CreateQuickLaunchIcon=Skapa en ikon i Snabbstartfältet
sv.PathNotExist=Sökvägen till mappen %1 existerar inte
; Turkish
tr.components_plugins=Eklentiler
tr.components_skins=Arayüzler
tr.tasks_languages=Diller
tr.tasks_allusers=Tüm Kullanıcılar
tr.tasks_selectusers=%1 Kuruldu:
tr.tasks_currentuser=Sadece %1
tr.tasks_multisetup=Çoklu kullanıcı desteğini etkinleştir
tr.tasks_firewall=Windows Güvenlik Duvarına bir değişiklik ekle
tr.tasks_upnp=Plug'n'Play cihazlarin kesfini etkinlestir
tr.tasks_deleteoldsetup=Eski kurulumları sil
tr.tasks_resetdiscoveryhostcache=Kesif servisleri ile host bellegini sifirla
tr.run_skinexe=Arayüz kurulumu çalışıyor...
tr.reg_incomingchat=Gelen sohbet mesajı
tr.reg_apptitle=PeerProject En iyi Dosya Paylaşımı
tr.icons_license=Lisans
tr.icons_uninstall=Kurulumu Kaldır
tr.icons_downloads=İndirmeler
tr.icons_basicmode=Temel Mod
tr.icons_tabbedmode=Sekmeli Mod
tr.icons_windowedmode=Pencereli Mode
tr.dialog_shutdown=Şuan %1 çalışıyor.Kurulumun devam edebilmesi için %1'in kapalı olmasını istiyor musunuz?
tr.dialog_firewall=Windows güvenlik duvarına PeerProject kurulumunu eklemek başarısız oldu.%n Lütfen PeerProject'yı el ile istisna listesine ekle
tr.dialog_malwaredetected=Kötücül bir yazilim %1'de tespit edilmistir, PeerProject'nin kurulumundan önce sözü geçen kötücül yazilimi bir antivirus programi kullanarak sisteminizden silmeniz gerekmektedir. Simdi terk etmek ister misiniz?
tr.page_viruswarning_text=İnternet kullanıyorken, trojanlardan, wormlardan ve diğer kötü niyetli programlardan sizi koruyan güncel bir virüs tarayıcısına sahip olduğunuzdan emin olmalısınız. Bu bağlantıyı izleyerek bilgisayarınızı koruyan iyi virüs tarayıcılarının ve diğer güvenlik tiplerinin listesini bulacaksınız:
tr.page_viruswarning_title=Virüs Uyarısı
tr.page_viruswarning_subtitle=Bir AntiVirus programı yüklediniz mi?
tr.CreateDesktopIcon=Bir &Masaüstü ikonu görüntüle
tr.CreateQuickLaunchIcon=Bir &Hızlı Başlat ikonu görüntüle
; Chinese Traditional
zht.components_plugins=¥~±¾
zht.components_skins=¥~Æ[
zht.tasks_languages=»y¨¥
zht.tasks_allusers=©Ò¦³ªº¨Ï¥ÎªÌ
zht.tasks_selectusers=¬°³o¨Ç¨Ï¥ÎªÌ¦w¸Ë %1:
zht.tasks_currentuser=¥u¦³ %1
zht.tasks_multisetup=±Ò¥Î¦h­«¨Ï¥ÎªÌ¤ä´©
zht.tasks_firewall=¼W¥[¨Ò¥~¦Ü Windows ¨¾¤õÀğ (XP)
zht.tasks_upnp=Enable Plug'n'Play network devices for port-forwarding
zht.tasks_deleteoldsetup=§R°£ÂÂªº¦w¸Ëµ{¦¡
zht.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
zht.run_skinexe=¥¿¦b¦w¸Ë¥~Æ[...
zht.reg_incomingchat=¿é¤Jªº²á¤Ñ°T®§
zht.reg_apptitle=PeerProject---³Ì²×ªºÀÉ®×¤À¨É³nÅé
zht.icons_license=³nÅé±ÂÅv¨ó©w
zht.icons_uninstall=¸Ñ°£¦w¸Ë
zht.icons_downloads=¤U¸ü
zht.icons_basicmode=¼Ğ·Ç¼Ò¦¡
zht.icons_tabbedmode=¼ĞÅÒ¼Ò¦¡
zht.icons_windowedmode=¦h­«µøµ¡¼Ò¦¡
zht.dialog_shutdown=%1 ¥¿¦b¹B§@¤¤. ±z­nÃö³¬ %1 , Åı¦w¸Ëµ{¦¡±o¥HÄ~Äò¶i¦æ¶Ü?
zht.dialog_firewall=¦w¸Ëµ{¦¡µLªk·s¼W PeerProject ¦ÜWindows ¨¾¤õÀğ.%n½Ğ¤â°Ê·s¼W PeerProject ¦Ü¨Ò¥~²M³æ
zht.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
zht.page_viruswarning_text=¨Ï¥Îºô»Úºô¸ô®É, ±zÀ³¸ÓÁ`¬O½T«O¨¾¬r³nÅé¬°³Ì·sª©¥», ¦p¦¹¤~¯à«OÅ@±zÁ×§K¨ü¨ì¤ì°¨, Ä¯ÂÎ, ©Î¬O´c·Nµ{¦¡ªº«I®`. ±z¥i¥HÂI¿ï³o­Ó³sµ²¨ú±o¦w¥ş©Ê¯µ³Z»P¨}¦n¨¾¬r³nÅéªº²M³æ:
zht.page_viruswarning_title=¯f¬rÄµ§i
zht.page_viruswarning_subtitle=±z¬O§_¤w¸g¦w¸Ë¤F¤@­Ó¨¾¬r³nÅé?
zht.CreateDesktopIcon=½Ğ·s¼W¤@­Ó&®à­±¹Ï¥Ü
zht.CreateQuickLaunchIcon=½Ğ·s¼W¤@­Ó&§Ö³t±Ò°Ê¹Ï¥Ü
; Chinese Simplified
zhs.components_plugins=²å¼ş
zhs.components_skins=Æ¤·ô
zhs.tasks_languages=ÓïÑÔ
zhs.tasks_allusers=ËùÓĞÓÃ»§
zhs.tasks_selectusers=°²×° %1 Îª:
zhs.tasks_currentuser=½ö %1
zhs.tasks_multisetup=ÆôÓÃ¶àÓÃ»§Ö§³Ö
zhs.tasks_firewall=Ìí¼ÓÒ»¸öÀıÍâµ½ Windows ·À»ğÇ½
zhs.tasks_upnp=ÆôÓÃ UPnP Çı¶¯Æ÷²éÑ¯
zhs.tasks_deleteoldsetup=É¾³ı¾ÉµÄ°²×°ÎÄ¼ş
zhs.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
zhs.run_skinexe=°²×°Æ¤·ô...
zhs.reg_incomingchat=´«ÈëÁÄÌìÏûÏ¢
zhs.reg_apptitle=PeerProject ÖÕ¼«ÎÄ¼ş¹²Ïí
zhs.icons_license=Ğí¿É
zhs.icons_uninstall=Ğ¶ÔØ
zhs.icons_downloads=ÏÂÔØ
zhs.icons_basicmode=ÆÕÍ¨Ä£Ê½
zhs.icons_tabbedmode=±êÇ©Ä£Ê½
zhs.icons_windowedmode=ÊÓ´°Ä£Ê½
zhs.dialog_shutdown=%1 ÕıÔÚÔËĞĞ¡£ÄúÏ£Íû¹Ø±Õ %1 ÒÔ±ã¼ÌĞø°²×°Âğ£¿
zhs.dialog_firewall=°²×°Ìí¼Ó PeerProject µ½ Windows ·À»ğÇ½Ê§°Ü¡£%nÇë½« PeerProject ÊÖ¶¯Ìí¼ÓÖÁ³ıÍâÁĞ±í¡£
zhs.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
zhs.page_viruswarning_text=ÔÚÊÔÓÃ»¥ÁªÍøÊ±£¬ÄúĞèÒªÈ·¶¨ÄúÓµÓĞ×îĞÂµÄ²¡¶¾É¨ÃèÈí¼şÒÔ±£»¤ÄúÃâÊÜÄ¾Âí¡¢Èä³æºÍÆäËû¶ñÒâ³ÌĞòµÄÇÖº¦¡£Äú¿ÉÒÔÔÚÒÔÏÂÁ´½ÓÖĞÕÒµ½½ÏºÃµÄ²¡¶¾É¨ÃèÈí¼şµÄÁĞ±íÒÔ¼°ÆäËû±£»¤ÄúµÄ¼ÆËã»úµÄ°²È«½¨Òé:
zhs.page_viruswarning_title=²¡¶¾¾¯¸æ
zhs.page_viruswarning_subtitle=Äú°²×°ÁË·À²¡¶¾Èí¼şÁËÂğ£¿
zhs.CreateDesktopIcon=ÏÔÊ¾×ÀÃæÍ¼±ê(&D)
zhs.CreateQuickLaunchIcon=ÏÔÊ¾¿ìËÙÆô¶¯À¸Í¼±ê(&Q)
; Japanese
ja.components_plugins=ƒvƒ‰ƒOƒCƒ“
ja.components_skins=ƒXƒLƒ“
ja.tasks_languages=Œ¾Œêƒtƒ@ƒCƒ‹
ja.tasks_allusers=‚·‚×‚Ä‚Ìƒ†[ƒU[
ja.tasks_selectusers=%1‚ğ‚µ‚æ‚¤‚·‚éƒ†[ƒU[:
ja.tasks_currentuser=%1‚Ì‚İ
ja.tasks_multisetup=ƒ}ƒ‹ƒ`ƒ†[ƒU[ƒTƒ|[ƒg
ja.tasks_firewall=Windowsƒtƒ@ƒCƒ„[ƒEƒH[ƒ‹‚Ì—áŠO‚Éİ’è
ja.tasks_upnp=UPnP‘Î‰‹@Ší‚ÌŒŸo‚ğ—LŒø‚É‚·‚é
ja.tasks_deleteoldsetup=ŒÃ‚¢ƒCƒ“ƒXƒg[ƒ‰[‚Ìíœ
ja.tasks_resetdiscoveryhostcache=Reset Discovery and Hostcache
ja.run_skinexe=ƒXƒLƒ“ƒCƒ“ƒXƒg[ƒ‰[‚ğÀs‚µ‚Ä‚¢‚Ü‚·...
ja.reg_incomingchat=ƒ`ƒƒƒbƒgƒƒbƒZ[ƒW‚ğó‚¯“ü‚ê‚é
ja.reg_apptitle=PeerProjectƒtƒ@ƒCƒ‹‹¤—Lƒ\ƒtƒg
ja.icons_license=ƒ‰ƒCƒZƒ“ƒX
ja.icons_uninstall=ƒAƒ“ƒCƒ“ƒXƒg[ƒ‹
ja.icons_downloads=ƒ_ƒEƒ“ƒ[ƒh
ja.icons_basicmode=•W€ƒ‚[ƒh
ja.icons_tabbedmode=ƒ^ƒuƒ‚[ƒh
ja.icons_windowedmode=ƒEƒBƒ“ƒhƒEƒ‚[ƒh
ja.dialog_shutdown=%1 ‚ªŠˆ“®’†‚Å‚·. %1‚ğI—¹‚µ‚ÄƒCƒ“ƒXƒg[ƒ‹‚ğ‘±‚¯‚Ü‚·‚©?
ja.dialog_firewall=WindowsXPƒtƒ@ƒCƒ„[ƒEƒH[ƒ‹‚Ì“o˜^‚É¸”s‚µ‚Ü‚µ‚½.%nè“®‚Å“o˜^‚µ‚Ä‚­‚¾‚³‚¢.
ja.dialog_malwaredetected=Malware has been detected on your system at %1, please remove it with a virus/malware scanner before installing PeerProject. Do you want to exit now?
ja.page_viruswarning_text=‚ ‚È‚½‚ªƒCƒ“ƒ^[ƒlƒbƒg‚ÉÚ‘±‚·‚é‚Æ‚«‚ÍAƒgƒƒC‚âƒ[ƒ€–”‚ÍA‚»‚êˆÈŠO‚ÌŠëŒ¯‚Èƒtƒ@ƒCƒ‹‚©‚çPC‚ğ•ÛŒì‚·‚é‚½‚ß‚ÉA•K‚¸ƒEƒCƒ‹ƒX’è‹`ƒtƒ@ƒCƒ‹‚ğÅV‚Ì‚à‚Ì‚É‚µ‚Ü‚µ‚å‚¤BƒEƒCƒ‹ƒXƒXƒLƒƒƒi[‚âƒZƒLƒ…ƒŠƒeƒB-‚ÉŠÖ‚·‚éî•ñ‚ª‰º‹L‚ÌƒŠƒ“ƒN‚É‚ ‚è‚Ü‚·B
ja.page_viruswarning_title=ƒEƒCƒ‹ƒX‚ÌŒx
ja.page_viruswarning_subtitle=ƒAƒ“ƒ`EƒEƒCƒ‹ƒXEƒ\ƒtƒg‚Í“ü‚Á‚Ä‚¢‚Ü‚·‚©?
ja.CreateDesktopIcon=ƒfƒXƒNƒgƒbƒv‚ÉƒAƒCƒRƒ“‚ğ•\¦(&d)
ja.CreateQuickLaunchIcon=ƒNƒCƒbƒNƒ‰ƒ“ƒ`‚ÉƒAƒCƒRƒ“‚ğ•\¦(&Q)
