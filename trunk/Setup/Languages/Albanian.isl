; Inno Setup 5.1.11+ Albanian Messages (PeerProject: sq)
;
; Original Author: Besmir Godole (bgodole@gmail.com)
;
; http://www.jrsoftware.org/files/istrans/ (user-contributed translations) 
;
; Note: Do not add periods (.) to messages that don't have them already,
; because for those messages Inno Setup adds the periods automatically.
; (appending a period will result in two periods being displayed.)
; 

[LangOptions]
; The following three entries are very important. Be sure to read and 
; understand the '[LangOptions] section' topic in the help file.
LanguageName=Albanian
LanguageID=$041C
LanguageCodePage=1252
; If the language you are translating to requires special font faces or
; sizes, uncomment any of the following entries and change them accordingly.
;DialogFontName=
;DialogFontSize=8
;WelcomeFontName=Verdana
;WelcomeFontSize=12
;TitleFontName=Arial
;TitleFontSize=29
;CopyrightFontName=Arial
;CopyrightFontSize=8

[Messages]

; *** Application titles
SetupAppTitle=Nd�rtoni
SetupWindowTitle=Nd�rtoni - %1
UninstallAppTitle=�instaloni
UninstallAppFullTitle=�instaloni %1

; *** Misc. common
InformationTitle=Informacion
ConfirmTitle=Miratimi
ErrorTitle=Gabim

; *** SetupLdr messages
SetupLdrStartupMessage=Kjo do t� instaloj� %1. A doni t� vijoni?
LdrCannotCreateTemp=Nuk mund t� krijoj nj� sked� t� p�rkohshme. Nd�rtimi u nd�rpre
LdrCannotExecTemp=Nuk mund t� ekzekutoj sked�n n� direktorin� e p�rkohshme. Nd�rtimi u nd�rpre

; *** Startup error messages
LastErrorMessage=%1.%n%nGabim %2: %3
SetupFileMissing=Skeda %1 mungon nga direktori e instalimit. Ju lutemi korrigjoni problemin ose merrni nj� kopje t� re t� programit.
SetupFileCorrupt=Skedat e nd�rtimit jan� korruptuar. Ju lutemi merrni nj� kopje t� re t� programit.
SetupFileCorruptOrWrongVer=Skedat e nd�rtimit jan� korruptuar, ose nuk pajtohen me k�t� version t� Nd�rtimit. Ju lutemi korrigjoni problemin ose merrni nj� kopje t� re t� programit.
NotOnThisPlatform=Ky program nuk do t� veproj� n� %1.
OnlyOnThisPlatform=Ky program duhet t� veproj� n� %1.
OnlyOnTheseArchitectures=Ky program mund t� instalohet vet�m n� versionet e Windows t� modeluar me arkitektur�n e procesor�ve m� posht�:%n%n%1
MissingWOW64APIs=Versioni i Windows ku po punoni nuk e p�rfshin funksionin e k�rkuar nga Nd�rtimi p�r t� kryer nj� instalim 64-bit. Q� t� korrigjoni problemin, ju lutemi instaloni Service Pack %1.
WinVersionTooLowError=Ky program k�rkon %1 version %2 e m� tej.
WinVersionTooHighError=Ky program nuk mund t� instalohet n� %1 version %2 e m� tej.
AdminPrivilegesRequired=Ju duhet t� hyni si administrues kur instaloni k�t� program.
PowerUserPrivilegesRequired=Ju duhet t� hyni si administrues ose si an�tar i grupit P�rdorues n� Fuqi kur instaloni k�t� program.
SetupAppRunningError=Nd�rtimi diktoi se %1 po vepron aktualisht.%n%nJu lutemi mbyllni tani gjith� rastet e tij, pastaj klikoni OK p�r t� vijuar, ose Anulo q� t� dilni.
UninstallAppRunningError=�instalimi diktoi se %1 po vepron aktualisht.%n%nJu lutem mbyllni tani gjith� rastet e tij, pastaj klikoni OK p�r t� vijuar, ose Anulo q� t� dilni.

; *** Misc. errors
ErrorCreatingDir=Nd�rtimi nuk arriti t� krijoj� direktorin� "%1"
ErrorTooManyFilesInDir=Nuk mund t� krijoj nj� sked� n� direktorin� "%1" sepse p�rmban shum� skeda

; *** Setup common messages
ExitSetupTitle=Dilni nga Nd�rtimi
ExitSetupMessage=Nd�rtimi nuk ka p�rfunduar. N�se dilni tani, programi nuk do t� instalohet.%n%nJu mund ta ngarkoni p�rs�ri nj� her� tjet�r q� instalimi t� p�rfundoj�.%n%nDoni t� dilni nga Nd�rtimi?
AboutSetupMenuItem=&Mbi Nd�rtimin...
AboutSetupTitle=Mbi Nd�rtimin
AboutSetupMessage=%1 version %2%n%3%n%n%1 faqe zyrtare:%n%4
AboutSetupNote=
TranslatorNote=Albanian translation by Besmir Godole (bgodole@gmail.com)

; *** Buttons
ButtonBack=< &Pas
ButtonNext=&Tjet�r >
ButtonInstall=&Instalo
ButtonOK=OK
ButtonCancel=Anulo
ButtonYes=&Po
ButtonYesToAll=Po, &Gjith�ka
ButtonNo=&Jo
ButtonNoToAll=J&o, Asgj�
ButtonFinish=&P�rfundo
ButtonBrowse=&Shfleto...
ButtonWizardBrowse=S&hfleto...
ButtonNewFolder=&Krijo Dosje t� Re

; *** "Select Language" dialog messages
SelectLanguageTitle=Zgjidhni Gjuh�n e Nd�rtimit
SelectLanguageLabel=Zgjidhni gjuh�n q� do t� p�rdorni gjat� instalimit:

; *** Common wizard text
ClickNext=Klikoni Tjet�r p�r t� vijuar, ose Anulo q� t� dilni nga Nd�rtimi.
BeveledLabel=
BrowseDialogTitle=Shfletoni Dosjen
BrowseDialogLabel=Zgjidhni nj� dosje n� k�t� list�, pastaj klikoni OK.
NewFolderName=Dosje e Re

; *** "Welcome" wizard page
WelcomeLabel1=Mir� se erdh�t n� Uizardin Nd�rtues t� [name]
WelcomeLabel2=[name/ver] do t� instalohet n� kompjuter.%n%nPara se t� vijoni rekomandohet t� mbyllni aplikimet e tjera.

; *** "Password" wizard page
WizardPassword=Fjal�kalimi
PasswordLabel1=Ky instalim mbrohet me fjal�kalim.
PasswordLabel3=Ju lutemi shkruani fjal�kalimin, pastaj klikoni Tjet�r p�r t� vijuar. Fjal�kalimet jan� rast i ndjesh�m.
PasswordEditLabel=&Fjal�kalimi:
IncorrectPassword=Fjal�kalimi q� shkruat nuk �sht� korrekt. Ju lutemi provoni p�rs�ri.

; *** "License Agreement" wizard page
WizardLicense=Marr�veshje License
LicenseLabel=Ju lutemi lexoni informacionin e r�nd�sish�m m� posht� para se t� vijoni.
LicenseLabel3=Ju lutemi lexoni Marr�veshjen e Licens�s m� posht�. Para se t� vijoni, duhet t� pranoni termat e k�saj marr�veshjeje.
LicenseAccepted=Un� &pranoj marr�veshjen
LicenseNotAccepted=Un� &nuk pranoj marr�veshjen

; *** "Information" wizard pages
WizardInfoBefore=Informacion
InfoBeforeLabel=Ju lutemi lexoni informacionin e r�nd�sish�m m� posht� para se t� vijoni.
InfoBeforeClickLabel=Kur t� jeni gati p�r t� vijuar me Nd�rtimin, klikoni Tjet�r.
WizardInfoAfter=Informacion
InfoAfterLabel=Ju lutemi lexoni informacionin e r�nd�sish�m m� posht� para se t� vijoni.
InfoAfterClickLabel=Kur t� jeni gati p�r t� vijuar me Nd�rtimin, klikoni Tjet�r.

; *** "User Information" wizard page
WizardUserInfo=Informacion P�rdoruesi
UserInfoDesc=Ju lutemi shkruani informacionin tuaj.
UserInfoName=&P�rdoruesi:
UserInfoOrg=&Organizata:
UserInfoSerial=Numri i &Seris�:
UserInfoNameRequired=Duhet t� shkruani nj� em�r.

; *** "Select Destination Location" wizard page
WizardSelectDir=Zgjidhni Vend-Destinacionin
SelectDirDesc=Ku doni t� instaloni [name]?
SelectDirLabel3=Nd�rtimi do t� instaloj� [name] n� k�t� dosje.
SelectDirBrowseLabel=P�r t� vijuar, klikoni Tjet�r. N�se doni t� zgjidhni nj� dosje ndryshe, klikoni Shfleto.
DiskSpaceMBLabel=K�rkohet jo m� pak se [mb] MB hap�sir� e lir� e diskut.
ToUNCPathname=Nd�rtimi nuk mund t� instaloj� n� nj� em�r-shteg UNC. N�se po provoni t� instaloni n� rrjet, duhet t� zbuloni diskun e rrjetit.
InvalidPath=Ju duhet t� shkruani shtegun e plot� me shkronj�n e diskut; p�r shembull:%n%nC:\APP%n%nose nj� shteg UNC sipas form�s:%n%n\\server\share
InvalidDrive=Disku ose shp�rndar�si UNC q� zgjodh�t nuk ekziston ose nuk mund t� hapet. Ju lutemi zgjidhni nj� tjet�r.
DiskSpaceWarningTitle=Hap�sir� Disku e Pamjaftueshme
DiskSpaceWarning=Nd�rtimi k�rkon jo m� pak se %1 KB hap�sir� t� lir� q� t� instaloj�, por disku i zgjedhur ka vet�m %2 KB t� vlefsh�m.%n%nGjithsesi, a doni t� vijoni?
DirNameTooLong=Emri ose shtegu i dosjes �sht� tep�r i gjat�.
InvalidDirName=Emri i dosjes nuk �sht� i vlefsh�m.
BadDirName32=Emrat e dosjes nuk mund t� p�rmbajn� asnj� nga k�to shkronja:%n%n%1
DirExistsTitle=Dosja Ekziston
DirExists=Dosja:%n%n%1%n%nekziston q� m� par�. Gjithsesi, a doni t� instaloni n� k�t� dosje?
DirDoesntExistTitle=Dosja Nuk Ekziston
DirDoesntExist=Dosja:%n%n%1%n%nnuk ekziston. A doni t� krijoni dosjen?

; *** "Select Components" wizard page
WizardSelectComponents=Zgjidhni P�rb�r�sit
SelectComponentsDesc=Cil�t p�rb�r�s doni t� instaloni?
SelectComponentsLabel2=Zgjidhni p�rb�r�sit q� doni t� instaloni; pastroni p�rb�r�sit q� nuk doni t� instaloni. Klikoni Tjet�r kur t� jeni gati t� vijoni.
FullInstallation=Instalim i Plot�
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Instalim Kompakt
CustomInstallation=Instalim Porosit�s
NoUninstallWarningTitle=P�rb�r�sit Ekzistojn�
NoUninstallWarning=Nd�rtimi diktoi se k�ta p�rb�r�s jan� instaluar q� m� par� n� kompjuter:%n%n%1%n%nL�shimi i k�tyre p�rb�r�sve nuk do t� �instaloj� ata.%n%nGjithsesi, a doni t� vijoni?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Zgjedhja aktuale k�rkon jo m� pak se [mb] MB hap�sir� disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Zgjidhni Detyrat Shtes�
SelectTasksDesc=Cilat detyra shtes� doni t� kryeni?
SelectTasksLabel2=Zgjidhni detyrat shtes� q� doni t� kryej� Nd�rtimi gjat� instalimit t� [name], pastaj klikoni Tjet�r.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Zgjidhni Dosjen n� Menun� Start
SelectStartMenuFolderDesc=Ku duhet t� vendos� Nd�rtimi shkurtesat e programit?
SelectStartMenuFolderLabel3=Nd�rtimi do t� krijoj� shkurtesat e programit n� k�t� dosje t� Menus� Start.
SelectStartMenuFolderBrowseLabel=P�r t� vijuar, klikoni Tjet�r. N�se doni t� zgjidhni nj� dosje ndryshe, klikoni Shfleto.
MustEnterGroupName=Duhet t� shkruani emrin e dosjes.
GroupNameTooLong=Emri ose shtegu i dosjes �sht� tep�r i gjat�.
InvalidGroupName=Emri i dosjes �sht� i pavlefsh�m.
BadGroupName=Emri i dosjes nuk mund t� p�rmbaj� asnj� nga k�to shkronja:%n%n%1
NoProgramGroupCheck2=&Mos krijo Dosje n� Menun� Start

; *** "Ready to Install" wizard page
WizardReady=Gati t� Instaloni
ReadyLabel1=Nd�rtimi tani �sht� gati t� nis� t� instaloj� [name] n� kompjuter.
ReadyLabel2a=Klikoni Instalo p�r t� vijuar me instalimin, ose klikoni Pas q� t� rishikoni apo ndryshoni ndonj� kuadro.
ReadyLabel2b=Klikoni Instalo p�r t� vijuar me instalimin.
ReadyMemoUserInfo=Informacion i P�rdoruesit:
ReadyMemoDir=Vend-Destinacioni:
ReadyMemoType=Lloji i Nd�rtimit:
ReadyMemoComponents=P�rb�r�sit e Zgjedhur:
ReadyMemoGroup=Dosja n� Menun� Start:
ReadyMemoTasks=Detyra shtes�:

; *** "Preparing to Install" wizard page
WizardPreparing=P�rgatitje p�r Instalim
PreparingDesc=Nd�rtimi po p�rgatitet t� instaloj� [name] n� kompjuter.
PreviousInstallNotCompleted=Instalimi/heqja e nj� programi t� m�parsh�m nuk p�rfundoi. Ju duhet t� rinisni kompjuterin q� t� plot�soni instalimin.%n%nPas rinisjes s� kompjuterit, ngarkoni p�rs�ri Nd�rtimin q� t� plot�soni instalimin e [name].
CannotContinue=Nd�rtimi nuk mund t� vijoj�. Ju lutemi klikoni Anulo q� t� dilni.

; *** "Installing" wizard page
WizardInstalling=Po Instaloj
InstallingLabel=Ju lutemi prisni nd�rkoh� q� Nd�rtimi instalon [name] n� kompjuter.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=Po Plot�soj Uizardin Nd�rtues t� [name]
FinishedLabelNoIcons=Nd�rtimi p�rfundoi s� instaluari [name] n� kompjuter.
FinishedLabel=Nd�rtimi p�rfundoi s� instaluari [name] n� kompjuter. Aplikimi mund t� l�shohet duke zgjedhur ikonat e instaluara.
ClickFinish=Klikoni P�rfundo p�r t� dal� nga Nd�rtimi.
FinishedRestartLabel=P�r t� plot�suar instalimin e [name], Nd�rtimi duhet t� rinis� kompjuterin. A doni t� rinisni tani?
FinishedRestartMessage=P�r t� plot�suar instalimin e [name], Nd�rtimi duhet t� rinis� kompjuterin.%n%nA doni t� rinisni tani?
ShowReadmeCheck=Po, dua t� shikoj sked�n README
YesRadio=&Po, rinise kompjuterin tani
NoRadio=&Jo, do e nis kompjuterin m� von�
; used for example as 'Run MyProg.exe'
RunEntryExec=Vepro %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Shiko %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Nd�rtimit i Nevojitet Disku Tjet�r
SelectDiskLabel2=Ju lutemi vendosni Diskun %1 dhe klikoni OK.%n%nN�se skedat n� k�t� disk mund t� gjenden n� nj� dosje ndryshe nga ajo q� shfaqet m� posht�, shkruani shtegun korrekt ose klikoni Shfleto.
PathLabel=&Shtegu:
FileNotInDir2=Skeda "%1" nuk mund t� gjendej n� "%2". Ju lutemi vendosni diskun korrekt ose zgjidhni nj� dosje tjet�r.
SelectDirectoryLabel=Ju lutemi p�rcaktoni vendin e diskut tjet�r.

; *** Installation phase messages
SetupAborted=Nd�rtimi nuk p�rfundoi.%n%nJu lutemi korrigjoni problemin dhe ngarkoni p�rs�ri Nd�rtimin.
EntryAbortRetryIgnore=Klikoni Riprovo p�r t� provuar p�rs�ri, P�rbuze q� gjithsesi t� vijoni, ose Nd�rprite q� t� anuloni instalimin.

; *** Installation status messages
StatusCreateDirs=Po krijoj direktorit�...
StatusExtractFiles=Po nxjerr skedat...
StatusCreateIcons=Po krijoj shkurtesat...
StatusCreateIniEntries=Po krijoj hyrjet INI...
StatusCreateRegistryEntries=Po krijoj hyrjet e regjistrit...
StatusRegisterFiles=Po regjistroj skedat...
StatusSavingUninstall=Po ruaj informacionin e �instalimit...
StatusRunProgram=Po p�rfundoj instalimin...
StatusRollback=Po kthej ndryshimet e b�ra...

; *** Misc. errors
ErrorInternal2=Gabim i brendsh�m: %1
ErrorFunctionFailedNoCode=%1 d�shtoi
ErrorFunctionFailed=%1 d�shtoi; kodi %2
ErrorFunctionFailedWithMessage=%1 d�shtoi; kodi %2.%n%3
ErrorExecutingProgram=Nuk mund t� ekzekutoj sked�n:%n%1

; *** Registry errors
ErrorRegOpenKey=Gabim gjat� hapjes s� kodit n� regjist�r:%n%1\%2
ErrorRegCreateKey=Gabim gjat� krijimit t� kodit n� regjist�r:%n%1\%2
ErrorRegWriteKey=Gabim gjat� krijimit t� kodit n� regjist�r:%n%1\%2

; *** INI errors
ErrorIniEntry=Gabim gjat� krijimit t� hyrjes INI n� sked�n "%1".

; *** File copying errors
FileAbortRetryIgnore=Klikoni Riprovo q� t� provoni p�rs�ri, P�rbuze q� t� kaloni sked�n (nuk rekomandohet), ose Nd�rprite q� t� anuloni instalimin.
FileAbortRetryIgnore2=Klikoni Riprovo p�r t� provuar p�rs�ri, P�rbuze q� gjithsesi t� vijoni (nuk rekomandohet), ose Nd�rprite q� t� anuloni instalimin.
SourceIsCorrupted=Skeda burimore �sht� korruptuar
SourceDoesntExist=Skeda burimore "%1" nuk ekziston
ExistingFileReadOnly=Skeda ekzistuese �sht� e shenjuar vet�m p�r lexim.%n%nKlikoni Riprovo q� t� hiqni atributin vet�m p�r lexim dhe provoni p�rs�ri, P�rbuze q� t� kaloni sked�n, ose Nd�rprite q� t� anuloni instalimin.
ErrorReadingExistingDest=Ndodhi nj� gabim gjat� prov�s p�r t� lexuar sked�n ekzistuese:
FileExists=Skeda ekziston q� m� par�.%n%nA doni q� Nd�rtimi t� mbishkruaj� at�?
ExistingFileNewer=Skeda ekzistuese �sht� m� e re sesa ajo q� Nd�rtimi po provon t� instaloj�. Rekomandohet q� t� mbani sked�n ekzistuese.%n%nA doni t� mbani sked�n ekzistuese?
ErrorChangingAttr=Ndodhi nj� gabim gjat� prov�s p�r t� ndryshuar atributet e sked�s ekzistuese:
ErrorCreatingTemp=Ndodhi nj� gabim gjat� prov�s p�r t� krijuar nj� sked� n� direktorin� destinacion:
ErrorReadingSource=Ndodhi nj� gabim gjat� prov�s p�r t� lexuar sked�n burimore:
ErrorCopying=Ndodhi nj� gabim gjat� prov�s p�r t� kopjuar sked�n:
ErrorReplacingExistingFile=Ndodhi nj� gabim gjat� prov�s p�r t� z�vend�suar sked�n ekzistuese:
ErrorRestartReplace=RinisjeZ�vend�simo d�shtoi:
ErrorRenamingTemp=Ndodhi nj� gabim gjat� prov�s p�r t� riem�rtuar sked�n n� direktorin� destinacion:
ErrorRegisterServer=Nuk mund t� regjistroj DLL/OCX: %1
ErrorRegSvr32Failed=RegSvr32 d�shtoi me kod dalje %1
ErrorRegisterTypeLib=Nuk mund t� regjistroj librarin� e llojit: %1

; *** Post-installation errors
ErrorOpeningReadme=Ndodhi nj� gabim gjat� prov�s p�r t� hapur sked�n README.
ErrorRestartingComputer=Nd�rtimi nuk mundi t� rinis� kompjuterin. Ju lutemi b�jeni vet�.

; *** Uninstaller messages
UninstallNotFound=Skeda "%1" nuk ekziston. Nuk mund t� �instaloj.
UninstallOpenError=Skeda "%1" nuk mund t� hapet. Nuk mund t� �instaloj
UninstallUnsupportedVer=Sked�-ditari i �instalimit "%1" �sht� n� format t� panjohur nga ky version i �instaluesit. Nuk mund t� �instaloj
UninstallUnknownEntry=Nj� hyrje e panjohur (%1) u ndesh n� ditarin e �instalimit
ConfirmUninstall=A jeni i sigurt se doni t� hiqni plot�sisht %1 dhe gjith� p�rb�r�sit e tij?
UninstallOnlyOnWin64=Ky instalim mund t� �instalohet vet�m n� Windows 64-bit.
OnlyAdminCanUninstall=Ky instalim mund t� �instalohet vet�m nga nj� p�rdorues me privilegje administrimi.
UninstallStatusLabel=Ju lutemi prisni nd�rkoh� q� %1 hiqet nga kompjuteri.
UninstalledAll=%1 u hoq me sukses nga kompjuteri.
UninstalledMost=%1 p�rfundoi s� �instaluari.%n%nDisa elemente nuk mund t� hiqeshin. K�to mund t� hiqen nga ju.
UninstalledAndNeedsRestart=Q� t� plot�soni �instalimin e %1, kompjuteri duhet t� rinis�.%n%nA doni t� rinisni tani?
UninstallDataCorrupted="%1" �sht� sked� e korruptuar. Nuk mund t� �instaloj

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Doni t� Hiqni Skedat e Shp�rndara?
ConfirmDeleteSharedFile2=Sistemi tregon se kjo sked� e shp�rndar� nuk p�rdoret m� nga asnj� program. A doni q� �instalimi t� heq� k�t� sked� t� shp�rndar�?%n%nN�se ndonj� program ende po p�rdor sked�n, kur ajo t� hiqet programi mund t� mos punoj� si� duhet. N�se jeni i pasigurt, zgjidhni Jo. L�nia e sked�n n� sistem nuk do t� shkaktoj� d�me.
SharedFileNameLabel=Emri i sked�s:
SharedFileLocationLabel=Vendi:
WizardUninstalling=Statusi i �instalimit
StatusUninstalling=Po �instaloj %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 version %2
AdditionalIcons=Ikona shtes�:
CreateDesktopIcon=Krijo ikon� n� &desktop
CreateQuickLaunchIcon=Krijo ikon� n� &Quick Launch
ProgramOnTheWeb=%1 n� Rrjet
UninstallProgram=�instalo %1
LaunchProgram=L�sho %1
AssocFileExtension=&Shoq�ro %1 me shtojc�n e sked�s %2
AssocingFileExtension=Po shoq�roj %1 me shtojc�n e sked�s %2...
