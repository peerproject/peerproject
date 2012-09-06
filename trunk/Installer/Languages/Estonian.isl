; Inno Setup version 5.1.0+ Estonian Messages (PeerProject: ee)
; Translation made with Translator 1.32 (http://www2.arnes.si/~sopjsimo/translator.html)

; Estonian translation by rix
; E-mail: admin@mmaailm.net
; Last modification date: 2005-6-01
; T�lge on valminud M�rt Kelderi esialgse versiooni baasil ning kohandatud uuemale versioonile.

;
; To download user-contributed translations of this file, go to:
;   http://www.jrsoftware.org/is3rdparty.php
;
; Note: Do not add periods (.), where added automatically.
;
; $jrsoftware: issrc/Files/Languages/Estonian.isl,v 1.7 2005/06/01 16:58:26 jr Exp $

[LangOptions]
LanguageName=Estonian
LanguageID=$0425
; If the language you are translating to requires special font faces or sizes,
; uncomment any of the following entries and change them accordingly.
;DialogFontName=MS Shell Dlg
;DialogFontSize=8
;DialogFontStandardHeight=13
;TitleFontName=Arial
;TitleFontSize=29
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;CopyrightFontName=Arial
;CopyrightFontSize=8

LanguageCodePage=0
DialogFontName=
DialogFontSize=8
WelcomeFontName=Verdana
WelcomeFontSize=12
TitleFontName=Arial
TitleFontSize=29
CopyrightFontName=Arial
CopyrightFontSize=8
[Messages]

; *** Application titles
SetupAppTitle=Paigaldaja
SetupWindowTitle=%1 paigaldamine
UninstallAppTitle=Kustutamine
UninstallAppFullTitle=%1 kustutamine

; *** Misc. common
InformationTitle=Informatsioon
ConfirmTitle=Kinnitus
ErrorTitle=Viga

; *** SetupLdr messages
SetupLdrStartupMessage=%1 paigaldatakse. Kas te soovite j�tkata?
LdrCannotCreateTemp=Ei suutnud tekitada ajutist faili. Paigaldamine katkestati
LdrCannotExecTemp=Ei suutnud k�ivitada faili ajutises kataloogis. Paigaldamine katkestati

; *** Startup error messages
LastErrorMessage=%1.%n%nViga %2: %3
SetupFileMissing=Fail %1 puudub paigaldamise kaustast. Palun parandage see viga v�i leidke uus versioon programmist.
SetupFileCorrupt=Paigaldaja failid on rikutud. Palun laadige paigaldaja uuesti alla.
SetupFileCorruptOrWrongVer=Paigaldaja failid on rikutud v�i ei t��ta selle paigaldaja versiooniga. Palun parandage see viga v�i leidke uus versioon programmist.
NotOnThisPlatform=%1 - programm ei t��ta sellel platvormil.
OnlyOnThisPlatform=%1 - programm t��tab ainult sellel platvormil.
OnlyOnTheseArchitectures=Seda programmi saab installida ainult Windowsi versioonidel, mis on m�eldud j�rgmistele protsessori arhitektuuridele:%n%n%1
MissingWOW64APIs=Teie poolt kasutatava Windowsi versioon ei sisalda Paigaldamise poolt vajatud funktsionaalsust 64-bit paigaldamisteks. Selle probleemi parandamiseks installige Service Pack %1.
WinVersionTooLowError=Programmi t��tamiseks on vaja %1 versiooni %2 v�i uuemat.
WinVersionTooHighError=Seda programmi ei saa paigaldada %1 versioon %2 v�i uuemale.
AdminPrivilegesRequired=Te peate administraatorina sisse logima, et seda programmi paigaldada.
PowerUserPrivilegesRequired=Te peate administraatorina v�i "Power userina" sisse logima, et seda programmi paigaldada.
SetupAppRunningError=%1 t��tab hetkel.%n%nPalun pange programm kinni, j�tkamiseks vajutage OK, katkestamiseks Cancel.
UninstallAppRunningError=%1 t��tab hetkel.%n%nPalun pange programm kinni, j�tkamiseks vajutage OK, katkestamiseks Cancel.

; *** Misc. errors
ErrorCreatingDir=Paigaldaja ei suutnud teha kataloogi "%1"
ErrorTooManyFilesInDir=Ei suutnud lisada faili kataloogi "%1", seal on juba liiga palju faile

; *** Setup common messages
ExitSetupTitle=V�ljuge paigaldajast
ExitSetupMessage=Paigaldamine pole valmis. Kui te praegu v�ljute, siis programmi ei paigaldata.%n%nPaigaldamise l�petamiseks v�ite te paigaldaja m�ni teine kord uuesti k�ivitada.%n%nV�ljute paigaldajast?
AboutSetupMenuItem=&Paigaldajast...
AboutSetupTitle=Paigaldajast
AboutSetupMessage=%1 versioon %2%n%3%n%n%1 koduleht:%n%4
AboutSetupNote=

; *** Buttons
TranslatorNote=
ButtonBack=< &Tagasi
ButtonNext=&Edasi >
ButtonInstall=&Paigalda
ButtonOK=Sobib
ButtonCancel=Loobu
ButtonYes=&Jah
ButtonYesToAll=Jah &k�igile
ButtonNo=&Ei
ButtonNoToAll=E&i k�igile
ButtonFinish=&L�peta
ButtonBrowse=&Sirvi...

; *** "Select Language" dialog messages
ButtonWizardBrowse=Si&rvi...
ButtonNewFolder=&Loo uus kaust
SelectLanguageTitle=Valige paigaldaja keel
SelectLanguageLabel=Valige paigaldamise keel:

; *** Common wizard text
ClickNext=J�tkamiseks vajutage Edasi, katkestamiseks Loobu.
BeveledLabel=

; *** "Welcome" wizard page
BrowseDialogTitle=Sirvi kausta
BrowseDialogLabel=Valige listist kaust ja klikkige OK.
NewFolderName=Uus kaust
WelcomeLabel1=[name]  paigaldaja
WelcomeLabel2=N��d paigaldatakse [name/ver] teie arvutisse.%n%nEnne j�tkamist on soovitatav sulgeda k�ik programmid.

; *** "Password" wizard page
WizardPassword=Salas�na
PasswordLabel1=See paigaldaja on kaitstud salas�naga.
PasswordLabel3=Palun sisestage salas�na ja vajutage Edasi. Salas�nad on t�stutundlikud.
PasswordEditLabel=&Salas�na:
IncorrectPassword=Sisestatud salas�na on vale. Palun proovige uuesti.

; *** "License Agreement" wizard page
WizardLicense=Kasutamistingimused
LicenseLabel=Palun lugege see oluline informatsioon l�bi enne kui j�tkate.
LicenseLabel3=Palun lugege kasutamistingimusi. Te peate nendega n�ustuma enne kui paigaldaja saab j�tkata.
LicenseAccepted=Ma &n�ustun tingimustega
LicenseNotAccepted=Ma &ei n�ustu tingimustega

; *** "Information" wizard pages
WizardInfoBefore=Informatsioon
InfoBeforeLabel=Palun lugege see oluline informatsioon l�bi enne kui j�tkate.
InfoBeforeClickLabel=Kui olete valmis, vajutage Edasi.
WizardInfoAfter=Informatsioon
InfoAfterLabel=Palun lugege see oluline informatsioon l�bi enne kui j�tkate.
InfoAfterClickLabel=Kui olete valmis, vajutage Edasi.

; *** "User Information" wizard page
WizardUserInfo=Informatsioon kasutaja kohta
UserInfoDesc=Palun sisestage oma andmed.
UserInfoName=&Kasutajanimi:
UserInfoOrg=&Organisatsioon:
UserInfoSerial=&Seeria number:
UserInfoNameRequired=Te peate nime sisestama.

; *** "Select Destination Directory" wizard page
WizardSelectDir=Valige programmile kaust
SelectDirDesc=Kuhu [name] paigaldada?
SelectDirLabel3=Paigaldaja installib [name] j�rgnevasse kausta.
SelectDirBrowseLabel=J�tkamiseks kliki J�rgmine. Kui soovite valida teise kausta, klikkige Sirvi.
DiskSpaceMBLabel=Programm vajab v�hemalt [mb] MB k�vakettaruumi.
ToUNCPathname=Paigaldaja ei saa paigaldada UNC kataloogi. Kui te �ritate programmi v�rku paigaldada, siis peate v�rguketta enne m��rama.
InvalidPath=Te peate sisestama t�is tee koos ketta nimega;
InvalidDrive=Ketas v�i UNC share mille valisite, ei eksisteeri v�i pole ligip��setav. Palun valige m�ni teine.
DiskSpaceWarningTitle=Liiga v�he ruumi k�vakettal
DiskSpaceWarning=Paigaldamiseks on vaja v�hemalt %1 KB k�vakettaruumi, aga kettal on vaba ainult %2 KB.%n%nKas te soovite sellegipoolest j�tkata?
DirNameTooLong=Kausta nimi v�i teerada on liiga pikk
InvalidDirName=Kausta nimi on vigane.
BadDirName32=Kausta nimed ei tohi sisaldada j�rgmisi m�rke:%n%n%1
DirExistsTitle=Kaust on olemas
DirExists=Kaust:%n%n%1%n%non juba olemas. Kas Te soovite sellegipoolest sinna paigaldada?
DirDoesntExistTitle=Kausta pole
DirDoesntExist=Kausta:%n%n%1%n%npole. Kas te tahate, et see kaust luuakse?

; *** "Select Components" wizard page
WizardSelectComponents=valige komponendid
SelectComponentsDesc=Missugused komponendid paigaldada?
SelectComponentsLabel2=Valige komponendid, mida paigaldada; kustutage komponendid, mida Te ei soovi paigaldada. Kui valmis olete, vajutage edasi.
FullInstallation=T�ielik paigaldamine
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktne paigaldamine
CustomInstallation=Kohandatud paigaldamine
NoUninstallWarningTitle=Komponendid eksisteerivad
NoUninstallWarning=paigaldaja avastas, et j�rgmised komponendid on juba olemas Teie arvutis:%n%n%1%n%nNende mittevalimine ei kustuta neid.%n%nKas Te soovite sellegipoolest j�tkata?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Praegu valitud komponentidele on vaja [mb] MB kettaruumi.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Valige t�iendavad �lesanded
SelectTasksDesc=Millised t�iendavad �lesanded t�ita?
SelectTasksLabel2=Valige millised t�iendavad �lesanded [name] paigaldaja peab t�itma ja vajutage Edasi.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Valige Start men�� kaust
SelectStartMenuFolderDesc=Kuhu luua programmi otseteed?
SelectStartMenuFolderLabel3=Paigaldaja loob programmi l�hiteed j�rgnevasse Start Men�� kausta.
SelectStartMenuFolderBrowseLabel=J�tkamiseks kliki J�rgmine. Kui soovite valida teise kausta, klikkige Sirvi.
MustEnterGroupName=Te peate sisestama kausta nime.
GroupNameTooLong=Kausta nimi v�i teerada on liiga pikk.
InvalidGroupName=Kausta nimi on vigane.
BadGroupName=Kausta nimi ei tohi sisaldada j�rgmiseid m�rke:%n%n%1
NoProgramGroupCheck2=&�ra loo Start men�� kausta

; *** "Ready to Install" wizard page
WizardReady=Valmis paigaldama
ReadyLabel1=Paigaldaja on valmis paigaldama [name] Teie arvutisse.
ReadyLabel2a=L�petamiseks vajutage Paigalda.
ReadyLabel2b=J�tkamiseks vajutage Paigalda.
ReadyMemoUserInfo=Kasutaja informatsioon:
ReadyMemoDir=Sihtkaust:
ReadyMemoType=paigaldamise t��p:
ReadyMemoComponents=Valitud komponendid:
ReadyMemoGroup=Start Men�� kaust:
ReadyMemoTasks=Lisa�lesanded:

; *** "Preparing to Install" wizard page
WizardPreparing=Paigaldamiseks valmistumine
PreparingDesc=Paigaldaja valmistub paigaldama [name] sinu arvutisse.
PreviousInstallNotCompleted=Eelmise programmi paigaldamine/kustutamine ei olnud l�petanud. Te peate arvuti restartima ja l�petama selle paigaldamise.%n%nP�rast restarti k�ivitage [name] Paigaldaja uuesti.
CannotContinue=Paigaldaja ei saa j�tkata. V�ljumiseks vajutage Loobu.

; *** "Installing" wizard page
WizardInstalling=Paigaldamine
InstallingLabel=Palun oodake kuni [name] paigaldatakse Teie arvutisse.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] paigaldamise l�petamine
FinishedLabelNoIcons=[name] paigaldamine on l�petatud.
FinishedLabel=[name] paigaldamine on l�petatud. Programmi saab k�ivitada klikkides m�nd tekitatud ikooni.
ClickFinish=V�ljumiseks vajutage L�peta.
FinishedRestartLabel=[name] paigaldamise l�petamiseks peab arvuti restartima, kas Te soovite kohe restartida?
FinishedRestartMessage=[name] paigaldamise l�petamiseks peab arvuti restartima, %n%nkas Te soovite kohe restartida?
ShowReadmeCheck=Jah, ma sooviksin n�ha Readme faili
YesRadio=&Jah, soovin arvuti n��d restartida
NoRadio=&Ei, ma soovin arvuti hiljem restartida
; used for example as 'Run MyProg.exe'
RunEntryExec=K�ivita %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Vaata %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Paigaldaja vajab j�rgmist diski
SelectDiskLabel2=Palun sisesta disk %1 ja kliki OK.%n%nKui diskil olevad failid on olemas ka kuskil mujal, sisesta �ige asukoht v�i kliki Sirvi.
PathLabel=&Asukoht:
FileNotInDir2=Faili "%1" ei suudetud leida asukohas "%2". Palun sisesta �ige disk v�i vali teine kaust.
SelectDirectoryLabel=Palun t�psusta asukoht j�rgneval diskil.

; *** Installation phase messages
SetupAborted=Paigaldamist ei l�petetud.%n%nPalun parandage viga ja k�ivitage paigaldaja uuesti.
EntryAbortRetryIgnore=Uuesti proovimiseks vajutage Retry, j�tkamiseks Ignore v�i l�petamisks Loobu.

; *** Installation status messages
StatusCreateDirs=Kaustade loomine...
StatusExtractFiles=Failide lahtipakkimine...
StatusCreateIcons=Otseteede loomine...
StatusCreateIniEntries=INI sisendite loomine...
StatusCreateRegistryEntries=Registrisse lisamine...
StatusRegisterFiles=Failide registreerimine...
StatusSavingUninstall=Info salvestamise kustutamiseks...
StatusRunProgram=Paigaldamise l�petamine...
StatusRollback=Tehtud muudatuste kustutamine...

; *** Misc. errors
ErrorInternal2=Sisemine viga: %1
ErrorFunctionFailedNoCode=%1 luhtus
ErrorFunctionFailed=%1 luhtus; code %2
ErrorFunctionFailedWithMessage=%1 luhtus; code %2.%n%3
ErrorExecutingProgram=Ei saanud k�ivitada faili:%n%1

; *** Registry errors
ErrorRegOpenKey=Ei saanud avada regirty v�tit:%n%1\%2
ErrorRegCreateKey=Ei saanud luua regirty v�tit:%n%1\%2
ErrorRegWriteKey=Ei saanud kirjutada regirty v�tit:%n%1\%2

; *** INI errors
ErrorIniEntry=Viga INI sisestuse tekitamisel failis "%1".

; *** File copying errors
FileAbortRetryIgnore=Uuesti proovimiseks vajutage Retry, faili vahelej�tmiseks Ignore (ei soovita) v�i l�petamiseks Abort.
FileAbortRetryIgnore2=Uuesti proovimiseks vajutage Retry, faili vahelej�tmiseks Ignore (ei soovita) v�i l�petamiseks Abort.
SourceIsCorrupted=Fail on rikutud
SourceDoesntExist=Faili "%1" ei eksisteeri
ExistingFileReadOnly=Fail on m�rgitud Read only.%n%nRead only atribuudi kustutamiseks vajutage Retry, faili vahelej�tmiseks Ignore (ei soovita) v�i l�petamiseks Abort
ErrorReadingExistingDest=Faili lugemisel tekkis viga:
FileExists=Fail on olemas.%n%nKas soovite, et paigaldaja selle �le kirjutaks?
ExistingFileNewer=Olemasolev fail on uuem kui see, mida paigaldaja �ritab paigaldada. Soovitatav on olemasolev fail alles hoida.%n%nSoovite olemasoleva faili alles hoida?
ErrorChangingAttr=Faili atribuutide muutmisel ilmnes viga:
ErrorCreatingTemp=Faili loomisek sihtkataloogi ilmnes viga:
ErrorReadingSource=Faili lugemisel ilmnes viga:
ErrorCopying=Faili kopeerimisel ilmnes viga:
ErrorReplacingExistingFile=Olemasoleva faili asendamisel ilmnes viga:
ErrorRestartReplace=Faili asendamine peale restarti ei �nnestunud:
ErrorRenamingTemp=Faili nime muutmisel sihtkataloogis ilmnes viga:
ErrorRegisterServer=Ei saa registreerida DLL/OCX: %1
ErrorRegSvr32Failed=RegSvr32 luhtus with exit code %1
ErrorRegisterTypeLib=V�imatu registreerida library t��pi: %1

; *** Post-installation errors
ErrorOpeningReadme=Readme faili avamisel ilmnes viga.
ErrorRestartingComputer=Paigaldaja ei suutnud arvutit restartida. Palun tehke seda k�sitsi.

; *** Uninstaller messages
UninstallNotFound=Faili"%1" ei ole olemas. Ei saa kustutada.
UninstallOpenError=Faili"%1" ei saa avada. Ei saa kustutada.
UninstallUnsupportedVer=Kustutamise logi faili "%1" formaat on tundmatu. Ei saa kustutada
UninstallUnknownEntry=Tundmatu sisend (%1) leiti kustutaja logist
ConfirmUninstall=Olete kindel, et soovite kustutada %1 ja k�ik tema komponendid?
UninstallOnlyOnWin64=Paigaldamine on eemaldatav ainult 64-bit Windowsitel.
OnlyAdminCanUninstall=Selle programmi saab kustutada ainult Teie arvuti administraator.
UninstallStatusLabel=Palun oodake kuni %1 kustutatakse.
UninstalledAll=%1 kustutamine �nnestus.
UninstalledMost=%1 kustutamine �nnestus.%n%nM�ned failid j�id kustutamata. Need v�ib k�sitsi kustutada.
UninstalledAndNeedsRestart=%1 kustutamise l�petamiseks peab arvuti restartima.%n%nSoovite kohe restartida?
UninstallDataCorrupted="%1" fail on rikutud. Ei saa ksututada

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Kustutada jagatud fail?
ConfirmDeleteSharedFile2=S�steem kinnitab, et jagatud faili ei kasuta �kski teine programm. Kas te soovite, et kustutaja selle kustutaks?%n%nKui m�ni programm seda siiski veel kasutab, siis ei pruugi ta enam korralikult t��tada. Kui Te pole kindel, valige Ei. Faili allesj�tmine ei tekita probleeme, kustutamine v�ib aga tekitada.
SharedFileNameLabel=Faili nimi:
SharedFileLocationLabel=Asukoht:
WizardUninstalling=Kustutamise staatus
StatusUninstalling=%1 - kustutan
[CustomMessages]
NameAndVersion=%1 versioon %2.
AdditionalIcons=T�iendavad ikoonid:
CreateDesktopIcon=Loo &t��laua ikoon
CreateQuickLaunchIcon=Loo &kiirk�ivituse ikoon
ProgramOnTheWeb=% veebis.
UninstallProgram=Eemalda %1
LaunchProgram=K�ivita %1
AssocFileExtension=&Seosta %1 faili %2 laiendiga.
AssocingFileExtension=Seostan %1 faili %2 laiendiga...
