; Inno Setup 5.1.11+ Serbian (Latin) messages (PeerProject: sr)
;
; Vladimir Stefanovic, antivari@sbb.co.yu, 30.09.2007
;
; http://www.jrsoftware.org/files/istrans/ (user-contributed translations) 
;
; Note: Do not add periods (.) to messages that don't have them already,
; because for those messages Inno Setup adds the periods automatically.
; (appending a period will result in two periods being displayed.)

[LangOptions]
LanguageName=Srpski
LanguageID=$081A
LanguageCodePage=1250
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
SetupAppTitle=Instalacija
SetupWindowTitle=Instalacija - %1
UninstallAppTitle=Deinstalacija
UninstallAppFullTitle=%1 Deinstalacija

; *** Misc. common
InformationTitle=Informacije
ConfirmTitle=Potvrda
ErrorTitle=Gre�ka

; *** SetupLdr messages
SetupLdrStartupMessage=Ovim programom �ete instalirati %1. Da li �elite da nastavite?
LdrCannotCreateTemp=Nije mogu�e napraviti privremenu datoteku. Instalacija je prekinuta
LdrCannotExecTemp=Nije mogu�e pokrenuti datoteku u privremenom direktorijumu. Instalacija je prekinuta

; *** Startup error messages
LastErrorMessage=%1.%n%nGre�ka %2: %3
SetupFileMissing=Datoteka %1 nedostaje u instalacionom direktorijumu. Molimo Vas ispravite problem ili nabavite novu kopiju programa.
SetupFileCorrupt=Instalacione datoteke su neispravne. Molimo Vas da nabavite novu kopiju programa.
SetupFileCorruptOrWrongVer=Instalacione datoteke su neispravne, ili nisu usagla�ene sa ovom verzijom instalacije. Molimo Vas ispravite problem ili nabavite novu kopiju programa.
NotOnThisPlatform=Ovaj program se ne�e pokrenuti na %1.
OnlyOnThisPlatform=Ovaj program se mora pokrenuti na %1.
OnlyOnTheseArchitectures=Ovaj program se mo�e instalirati samo na verzijama Windowsa projektovanim za slede�e procesorske arhitekture:%n%n%1
MissingWOW64APIs=Verzija Windows-a koju koristite ne sadr�i mogu�nosti potrebne za instalacionu proceduru da uradi 64-bitnu instalaciju. Da bi re�ili ovaj problem, molimo instalirajte Service Pack %1.
WinVersionTooLowError=Ovaj program zahteva %1 verziju %2 ili noviju.
WinVersionTooHighError=Ovaj program se ne mo�e instalirati na %1 verziji %2 ili novijoj.
AdminPrivilegesRequired=Morate biti prijavljeni kao administrator da bi ste instalirali ovaj program.
PowerUserPrivilegesRequired=Morate biti prijavljeni kao administrator ili kao �lan Power Users grupe kada instalirate ovaj program.
SetupAppRunningError=Instalacija je otkrila da se %1 trenutno izvr�ava.%n%nMolimo da odmah zatvorite sve njegove instance, a zatim pritisnite OK za nastavak, ili Cancel da odustanete.
UninstallAppRunningError=Deinstalacija je otkrila da se %1 trenutno izvr�ava.%n%nMolimo da odmah zatvorite sve njegove instance, a zatim pritisnite OK za nastavak, ili Cancel da odustanete.

; *** Misc. errors
ErrorCreatingDir=Instalacija nije mogla da napravi direktorijum "%1"
ErrorTooManyFilesInDir=Nije mogu�e napraviti datoteku u direktorijumu "%1" zato �to sadr�i previ�e datoteka

; *** Setup common messages
ExitSetupTitle=Prekidanje instalacije
ExitSetupMessage=Instalacija nije zavr�ena. Ako sada prekinete Instalaciju, program ne�e biti instaliran.%n%nInstalaciju mo�ete pokrenuti i dovr�iti nekom dugom prilikom.%n%nPrekid instalacije?
AboutSetupMenuItem=&O instalaciji...
AboutSetupTitle=O instalaciji
AboutSetupMessage=%1 verzija %2%n%3%n%n%1 mati�na stranica:%n%4
AboutSetupNote=
TranslatorNote=

; *** Buttons
ButtonBack=< &Nazad
ButtonNext=&Dalje >
ButtonInstall=&Instaliraj
ButtonOK=OK
ButtonCancel=Odustani
ButtonYes=&Da
ButtonYesToAll=Da za &Sve
ButtonNo=&Ne
ButtonNoToAll=N&e za Sve
ButtonFinish=&Zavr�etak
ButtonBrowse=&Izaberi...
ButtonWizardBrowse=I&zaberi...
ButtonNewFolder=&Napravi novi direktorijum

; *** "Select Language" dialog messages
SelectLanguageTitle=Izaberite jezik instalacije
SelectLanguageLabel=Izaberite jezik koji �elite da koristite prilikom instalacije:

; *** Common wizard text
ClickNext=Pritisnite Dalje da nastavite, ili Odustani da napustite instalaciju.
BeveledLabel=
BrowseDialogTitle=Izaberite direktorijum
BrowseDialogLabel=Izaberite jedan od ponu�enih direktorijuma iz liste, a zatim pritisnite OK.
NewFolderName=Novi Direktorijum

; *** "Welcome" wizard page
WelcomeLabel1=Dobrodo�li u [name] instalacionu proceduru
WelcomeLabel2=Sada �e se instalirati [name/ver] na Va� ra�unar.%n%nPreporu�uje se da zatvorite sve druge programe pre nastavka.

; *** "Password" wizard page
WizardPassword=�ifra
PasswordLabel1=Ova instalacija je za�ti�ena �ifrom.
PasswordLabel3=Molimo upi�ite �ifru, a zatim pritisnite Dalje za nastavak. Vodite ra�una da su velika i mala slova u �ifri bitna.
PasswordEditLabel=&�ifra:
IncorrectPassword=�ifra koju ste upisali nije ispravna. Molimo poku�ajte ponovo.

; *** "License Agreement" wizard
WizardLicense=Ugovor o kori��enju
LicenseLabel=Molimo pro�itajte pa�ljivo slede�e va�ne informacije pre nastavka.
LicenseLabel3=Molimo pro�itajte Ugovor o kori��enju, koji sledi. Morate prihvatiti uslove ovog ugovora pre nastavka instalacije.
LicenseAccepted=&Prihvatam ugovor
LicenseNotAccepted=&Ne prihvatam ugovor

; *** "Information" wizard pages
WizardInfoBefore=Informacije
InfoBeforeLabel=Molimo pro�itajte pa�ljivo slede�e va�ne informacije pre nastavka.
InfoBeforeClickLabel=Kada budete spremni da nastavite instalaciju, pritisnite Dalje.
WizardInfoAfter=Informacije
InfoAfterLabel=Molimo Vas pro�itajte pa�ljivo slede�e va�ne informacije pre nastavka.
InfoAfterClickLabel=Kada budete spremni da nastavite instalaciju, pritisnite Dalje.

; *** "User Information" wizard page
WizardUserInfo=Podaci o korisniku
UserInfoDesc=Molimo unesite Va�e podatke.
UserInfoName=&Korisnik:
UserInfoOrg=&Organizacija:
UserInfoSerial=&Serijski broj:
UserInfoNameRequired=Morate upisati ime.

; *** "Select Destination Location" wizard page
WizardSelectDir=Izaberite odredi�nu lokaciju
SelectDirDesc=Gde [name] treba da se instalira?
SelectDirLabel3=Instalacija �e postaviti [name] u slede�i direktorijum.
SelectDirBrowseLabel=Da nastavite, pritisnite Dalje. Ako �elite da izaberete neki drugi direktorijum, pritisnite Izaberi.
DiskSpaceMBLabel=Potrebno je najmanje [mb] MB slobodnog prostora na disku.
ToUNCPathname=Putanja za instalaciju ne sme biti u UNC obliku. Ako poku�avate da instalirate program na mre�u, mora�ete prethodno da mapirate mre�ni disk.
InvalidPath=Morate upisati punu putanju sa obele�jem diska; na primer:%n%nC:\APP%n%nili UNC putanja u obliku:%n%n\\server\share
InvalidDrive=Disk ili UNC putanja koju ste izabrali ne postoje ili nisu dostupni. Molimo izaberite ne�to drugo.
DiskSpaceWarningTitle=Nema dovoljno prostora na disku
DiskSpaceWarning=Instalacija zahteva najmanje %1 KB slobodnog prostora, a izabrani disk ima samo %2 KB na raspolaganju.%n%nDa li ipak �elite da nastavite?
DirNameTooLong=Naziv direktorijuma ili putanja su preduga�ki.
InvalidDirName=Naziv direktorijuma nije ispravan.
BadDirName32=Nazivi direktorijuma ne smeju imati bilo koje od slede�ih slova:%n%n%1
DirExistsTitle=Direktorijum postoji
DirExists=Direktorijum:%n%n%1%n%nve� postoji. Da li ipak �elite da program instalirate u njemu?
DirDoesntExistTitle=Direktorijum ne postoji
DirDoesntExist=Direktorijum:%n%n%1%n%nne postoji. Da li �elite da ga napravim?

; *** "Select Components" wizard page
WizardSelectComponents=Izaberite komponente
SelectComponentsDesc=Koje komponente �ete instalirati?
SelectComponentsLabel2=Izaberite komponente koje �elite da instalirate; obri�ite komponente koje ne �elite da instalirate. Pritisnite Dalje kada budete spremni da nastavite.
FullInstallation=Puna instalacija
; if possible don't translate 'Compact' as 'Minimal' (I mean 'Minimal' in your language)
CompactInstallation=Kompaktna instalacija
CustomInstallation=Instalacija samo �eljenih komponenti
NoUninstallWarningTitle=Komponente postoje
NoUninstallWarning=Instalacija je otkrila da slede�e komponente ve� postoje na Va�em ra�unaru:%n%n%1%n%nNeodabiranje ovih komponenti ih ne�e ukloniti.%n%nDa li ipak �elite da nastavite?
ComponentSize1=%1 KB
ComponentSize2=%1 MB
ComponentsDiskSpaceMBLabel=Trenutno odabrane stavke zahtevaju najmanje [mb] MB prostora na disku.

; *** "Select Additional Tasks" wizard page
WizardSelectTasks=Izaberite dodatne zadatke
SelectTasksDesc=Kakve dodatne zadatke je jo� potrebno obaviti?
SelectTasksLabel2=Izaberite dodatne zadatke koje �elite da Instalacija [name] obavi, a zatim pritisnite Dalje.

; *** "Select Start Menu Folder" wizard page
WizardSelectProgramGroup=Izaberite direktorijum za Start meni
SelectStartMenuFolderDesc=Gde �elite da instalacija postavi pre�ice za program?
SelectStartMenuFolderLabel3=Instalacija �e postaviti pre�ice za program u slede�em direktorijumu Start menija.
SelectStartMenuFolderBrowseLabel=Da nastavite, pritisnite Dalje. Ako �elite da izaberete neki drugi direktorijum, pritisnite Izaberi.
MustEnterGroupName=Morate upisati naziv direktorijuma.
GroupNameTooLong=Naziv direktorijuma ili putanja su preduga�ki.
InvalidGroupName=Naziv direktorijuma nije ispravan.
BadGroupName=Naziv direktorijuma ne sme imati bilo koje od slede�ih slova:%n%n%1
NoProgramGroupCheck2=&Nemoj da pravi� direktorijum u Start meniju

; *** "Ready to Install" wizard page
WizardReady=Instalacija je spremna
ReadyLabel1=Instalacija je spremna da postavi [name] na Va� ra�unar.
ReadyLabel2a=Pritisnite Instaliraj da nastavite sa instalacijom, ili pritisnite Nazad ako �elite da ponovo pregledate ili promenite neka pode�avanja.
ReadyLabel2b=Pritisnite Instaliraj da nastavite sa instalacijom.
ReadyMemoUserInfo=Podaci o korisniku:
ReadyMemoDir=Odredi�na lokacija:
ReadyMemoType=Tip instalacije:
ReadyMemoComponents=Izabrane komponente:
ReadyMemoGroup=Direktorijum Start menija:
ReadyMemoTasks=Dodatni poslovi:

; *** "Preparing to Install" wizard page
WizardPreparing=Priprema za instalaciju
PreparingDesc=Instalacija se priprema da postavi [name] na Va� ra�unar.
PreviousInstallNotCompleted=Instalacija/uklanjanje prethodnog programa nije zavr�ena. Potrebno je da restartujete Va� ra�unar da bi se instalacija zavr�ila.%n%nNakon restartovanja ra�unara, pokrenite ponovo Instalaciju [name] da bi ste je dovr�ili.
CannotContinue=Instalacija se ne mo�e nastaviti. Molimo pritisnite Odustani da iza�ete.

; *** "Installing" wizard page
WizardInstalling=Instaliranje
InstallingLabel=Molimo sa�ekajte dok se [name] instalira na Va� ra�unar.

; *** "Setup Completed" wizard page
FinishedHeadingLabel=[name] - zavr�etak instalacije
FinishedLabelNoIcons=Instalacija programa [name] na Va� ra�unar je zavr�ena.
FinishedLabel=Instalacija programa [name] na Va� ra�unar je zavr�ena. Program mo�ete pokrenuti preko postavljenih ikona.
ClickFinish=Pritisnite Zavr�etak da iza�ete.
FinishedRestartLabel=Da bi se instalacija [name] dovr�ila, mora se restartovati ra�unar. Da li �elite da ga restartujete odmah?
FinishedRestartMessage=Da bi se instalacija [name] dovr�ila, mora se restartovati ra�unar.%n%nDa li �elite da ga restartujete odmah?
ShowReadmeCheck=Da, �elim da pogledam README datoteku
YesRadio=&Da, restartuj ra�unar odmah
NoRadio=&Ne, restartova�u ra�unar kasnije
; used for example as 'Run MyProg.exe'
RunEntryExec=Pokreni %1
; used for example as 'View Readme.txt'
RunEntryShellExec=Pogledaj %1

; *** "Setup Needs the Next Disk" stuff
ChangeDiskTitle=Instalaciji je potreban naredni disk
SelectDiskLabel2=Molimo stavite Disk %1 i pritisnite OK.%n%nAko se datoteke na ovom disku mogu prona�i u nekom drugom direktorijumu nego �to je ispod prikazano, upi�ite odgovaraju�u putanju ili pritisnite Izaberi.
PathLabel=&Putanja:
FileNotInDir2=Datoteka "%1" se ne mo�e prona�i u "%2". Molimo stavite pravi disk ili izaberite drugi direktorijum.
SelectDirectoryLabel=Molimo izaberite lokaciju narednog diska.

; *** Installation phase messages
SetupAborted=Instalacija nije zavr�ena.%n%nMolimo ispravite problem i pokrenite Instalaciju ponovo.
EntryAbortRetryIgnore=Pritisnite Retry da probate ponovo, Ignore da nastavite u svakom slu�aju, ili Abort da prekinete instalaciju.

; *** Installation status messages
StatusCreateDirs=Pravljenje direktorijuma...
StatusExtractFiles=Raspakivanje datoteka...
StatusCreateIcons=Postavljanje pre�ica...
StatusCreateIniEntries=Postavljanje INI upisa...
StatusCreateRegistryEntries=Postavljanje registry upisa...
StatusRegisterFiles=Prijavljivanje datoteka...
StatusSavingUninstall=Bele�enje podataka za deinstalaciju...
StatusRunProgram=Zavr�avanje instalacije...
StatusRollback=Poni�tavanje izvr�enih izmena i vra�anje na prethodno stanje...

; *** Misc. errors
ErrorInternal2=Interna gre�ka: %1
ErrorFunctionFailedNoCode=%1 nije uspelo
ErrorFunctionFailed=%1 nije uspelo; kod %2
ErrorFunctionFailedWithMessage=%1 nije uspelo; kod %2.%n%3
ErrorExecutingProgram=Nije mogu�e pokrenuti datoteku:%n%1

; *** Registry errors
ErrorRegOpenKey=Gre�ka pri otvaranju registry klju�a:%n%1\%2
ErrorRegCreateKey=Gre�ka pri postavljanju registry klju�a:%n%1\%2
ErrorRegWriteKey=Gre�ka pri upisu registry klju�a:%n%1\%2

; *** INI errors
ErrorIniEntry=Gre�ka pri upisu u INI datoteku "%1".

; *** File copying errors
FileAbortRetryIgnore=Pritisnite Retry da probate ponovo, Ignore da presko�ite ovu datoteku (nije preporu�ljivo), ili Abort da prekinete instalaciju.
FileAbortRetryIgnore2=Pritisnite Retry da probate ponovo, Ignore da nastavite u svakom slu�aju (nije preporu�ljivo), ili Abort da prekinete instalaciju.
SourceIsCorrupted=Izvorna datoteka je neispravna
SourceDoesntExist=Izvorna datoteka "%1" ne postoji
ExistingFileReadOnly=Postoje�a datoteka je ozna�ena 'samo za �itanje'.%n%nPritisnite Retry da uklonite atribut 'samo za �itanje' i probate ponovo, Ignore da presko�ite ovu datoteku, ili Abort da prekinete instalaciju.
ErrorReadingExistingDest=Do�lo je do gre�ke prilikom poku�aja �itanja slede�e datoteke:
FileExists=Datoteka ve� postoji.%n%nDa li �elite da je Instalacija zameni?
ExistingFileNewer=Postoje�a datoteka je novija od one koju Instalacija treba da postavi. Preporu�uje se da zadr�ite postoje�u datoteku.%n%nDa li �elite da sa�uvate postoje�u datoteku?
ErrorChangingAttr=Do�lo je do gre�ke prilikom poku�aja promene atributa za slede�u datoteku:
ErrorCreatingTemp=Do�lo je do gre�ke prilikom poku�aja pravljenja datoteke u odredi�nom direktorijumu:
ErrorReadingSource=Do�lo je do gre�ke prilikom poku�aja �itanja izvorne datoteke:
ErrorCopying=Do�lo je do gre�ke prilikom poku�aja kopiranja datoteke:
ErrorReplacingExistingFile=Do�lo je do gre�ke prilikom poku�aja zamene postoje�e datoteke:
ErrorRestartReplace=RestartReplace nije uspeo:
ErrorRenamingTemp=Do�lo je do gre�ke prilikom poku�aja promene naziva datoteke u odredi�nom direktorijumu:
ErrorRegisterServer=Nije mogu�e registrovati DLL/OCX: %1
ErrorRegSvr32Failed=RegSvr32 nije uspe�no izvr�en, gre�ka %1
ErrorRegisterTypeLib=Nije uspelo registrovanje 'type library': %1

; *** Post-installation errors
ErrorOpeningReadme=Do�lo je do gre�ke prilikom otvaranja README datoteke.
ErrorRestartingComputer=Instalacija nije uspela da restartuje ra�unar. Molimo Vas uradite to sami.

; *** Uninstaller messages
UninstallNotFound=Datoteka "%1" ne postoji. Deinstaliranje nije uspelo.
UninstallOpenError=Datoteka "%1" se ne mo�e otvoriti. Deinstaliranje nije uspelo
UninstallUnsupportedVer=Deinstalaciona log datoteka "%1" je u obliku koji ne prepoznaje ova verzija deinstalera. Deinstaliranje nije uspelo
UninstallUnknownEntry=Nepoznat upis (%1) se pojavio u deinstalacionoj log datoteci
ConfirmUninstall=Da li ste sigurni da �elite da potpuno uklonite %1 i sve njegove komponente?
UninstallOnlyOnWin64=Ovaj program se mo�e deinstalirati samo na 64-bitnom Windowsu.
OnlyAdminCanUninstall=Ovaj program mo�e deinstalirati samo korisnik sa administratorskim pravima.
UninstallStatusLabel=Molimo sa�ekajte dok %1 ne bude uklonjen sa Va�eg ra�unara.
UninstalledAll=%1 je uspe�no uklonjen sa Va�eg ra�unara.
UninstalledMost=%1 deinstalacija je zavr�ena.%n%nNeki delovi se nisu mogli ukloniti. Oni se mogu ukloniti ru�no.
UninstalledAndNeedsRestart=Da dovr�ite deinstalaciju %1, Va� ra�unar se mora restartovati.%n%nDa li �elite da ga restartujete odmah?
UninstallDataCorrupted="%1" datoteka je o�te�ena. Deinstaliranje nije uspelo

; *** Uninstallation phase messages
ConfirmDeleteSharedFileTitle=Obri�i zajedni�ku datoteku?
ConfirmDeleteSharedFile2=Sistem pokazuje da slede�e zajedni�ke datoteke vi�e ne koristi ni jedan program. Da li �elite da deinstalacija ukloni ovu zajedni�ku datoteku?%n%nAko neki programi i dalje koriste ovu datoteku, mo�da ne�e ispravno funkcionisati. Ako niste sigurni, izaberite No. Ostavljanje ove datoteke ne�e prouzrokovati nikakvu �tetu sistemu.
SharedFileNameLabel=Naziv datoteke:
SharedFileLocationLabel=Putanja:
WizardUninstalling=Stanje deinstalacije
StatusUninstalling=Deinstaliranje %1...

; The custom messages below aren't used by Setup itself, but if you make
; use of them in your scripts, you'll want to translate them.

[CustomMessages]

NameAndVersion=%1 verzija %2
AdditionalIcons=Dodatne ikone:
CreateDesktopIcon=Postavi &Desktop ikonu
CreateQuickLaunchIcon=Postavi &Quick Launch ikonu
ProgramOnTheWeb=%1 na Internetu
UninstallProgram=Deinstalacija %1
LaunchProgram=Pokreni %1
AssocFileExtension=&Pridru�i %1 sa %2 tipom datoteke
AssocingFileExtension=Pridru�ivanje %1 sa %2 tipom datoteke...
