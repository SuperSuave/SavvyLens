; ============================================================
; SavvyLens — Windows installer
; File: src/packaging/windows/SavvyLens.iss
;
; Built by .github/workflows/build.yml after windeployqt creates
; the repository-root "package\" directory.
; ============================================================

#ifndef MyAppVersion
  #define MyAppVersion "0.0.0"
#endif

#define MyAppName "SavvyLens"
#define MyAppPublisher "SuaveEV"
#define MyAppURL "https://github.com/SuperSuave/SavvyLens"
#define MyAppExeName "SavvyLens.exe"

[Setup]
; Deterministic UUID recipe:
; Version 5 UUID, DNS namespace,
; name = "savvylens.supersuave.github.io"
; identifier = "62135bc0-9914-11f1-976e-325096b39f47"
; Replace the placeholder below with the generated UUID.
; Keep the doubled opening brace required by Inno Setup.
AppId={{c3f18ad6-975b-58b3-84b2-775037c630db}

AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}

VersionInfoVersion={#MyAppVersion}
VersionInfoProductVersion={#MyAppVersion}
VersionInfoCompany=SuaveEV
VersionInfoDescription=SavvyLens Installer
VersionInfoProductName=SavvyLens

AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}/issues
AppUpdatesURL={#MyAppURL}/releases

DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}

WizardImageFile=assets\installer-wizard.bmp
WizardSmallImageFile=assets\installer-header.bmp

UninstallDisplayName={#MyAppName} {#MyAppVersion}
UninstallDisplayIcon={app}\{#MyAppExeName}

OutputDir=Output
OutputBaseFilename=SavvyLens_Setup

; The script is in src/packaging/windows/, so four levels up
; is the repository root, where CI creates package\.
SourceDir=..\..\..

Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern

; Qt 5.15 supports Windows 10+.
MinVersion=10.0

ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; \
  Description: "Create a &desktop shortcut"; \
  GroupDescription: "Additional icons:"; \
  Flags: unchecked

Name: "fileassoc_savvy"; \
  Description: "Associate .savvy capture files with {#MyAppName}"; \
  GroupDescription: "File associations:"

Name: "fileassoc_blf"; \
  Description: "Associate .blf log files with {#MyAppName}"; \
  GroupDescription: "File associations:"; \
  Flags: unchecked

[Files]
; The entire windeployqt-created package, including Qt DLLs,
; plugins, translations, help, examples, and default templates.
Source: "package\*"; \
  DestDir: "{app}"; \
  Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; \
  Filename: "{app}\{#MyAppExeName}"

Name: "{group}\Uninstall {#MyAppName}"; \
  Filename: "{uninstallexe}"

Name: "{autodesktop}\{#MyAppName}"; \
  Filename: "{app}\{#MyAppExeName}"; \
  Tasks: desktopicon

[Registry]
; ------------------------------------------------------------
; .savvy capture file association
; ------------------------------------------------------------

Root: HKA; \
  Subkey: "Software\Classes\.savvy"; \
  ValueType: string; \
  ValueName: ""; \
  ValueData: "SavvyLens.CaptureFile"; \
  Flags: uninsdeletevalue; \
  Tasks: fileassoc_savvy

Root: HKA; \
  Subkey: "Software\Classes\SavvyLens.CaptureFile"; \
  ValueType: string; \
  ValueName: ""; \
  ValueData: "SavvyLens Capture File"; \
  Flags: uninsdeletekey; \
  Tasks: fileassoc_savvy

Root: HKA; \
  Subkey: "Software\Classes\SavvyLens.CaptureFile\DefaultIcon"; \
  ValueType: string; \
  ValueName: ""; \
  ValueData: "{app}\{#MyAppExeName},0"; \
  Tasks: fileassoc_savvy

Root: HKA; \
  Subkey: "Software\Classes\SavvyLens.CaptureFile\shell\open\command"; \
  ValueType: string; \
  ValueName: ""; \
  ValueData: """{app}\{#MyAppExeName}"" ""%1"""; \
  Tasks: fileassoc_savvy

; ------------------------------------------------------------
; .blf Vector Binary Log Format association
; Optional because other CAN applications commonly use BLF too.
; ------------------------------------------------------------

Root: HKA; \
  Subkey: "Software\Classes\.blf"; \
  ValueType: string; \
  ValueName: ""; \
  ValueData: "SavvyLens.BLFFile"; \
  Flags: uninsdeletevalue; \
  Tasks: fileassoc_blf

Root: HKA; \
  Subkey: "Software\Classes\SavvyLens.BLFFile"; \
  ValueType: string; \
  ValueName: ""; \
  ValueData: "SavvyLens BLF Log File"; \
  Flags: uninsdeletekey; \
  Tasks: fileassoc_blf

Root: HKA; \
  Subkey: "Software\Classes\SavvyLens.BLFFile\DefaultIcon"; \
  ValueType: string; \
  ValueName: ""; \
  ValueData: "{app}\{#MyAppExeName},0"; \
  Tasks: fileassoc_blf

Root: HKA; \
  Subkey: "Software\Classes\SavvyLens.BLFFile\shell\open\command"; \
  ValueType: string; \
  ValueName: ""; \
  ValueData: """{app}\{#MyAppExeName}"" ""%1"""; \
  Tasks: fileassoc_blf

[Run]
Filename: "{app}\{#MyAppExeName}"; \
  Description: "Launch {#MyAppName}"; \
  Flags: nowait postinstall skipifsilent

[Code]

var
  WorkspacePage: TOutputMsgWizardPage;

procedure InitializeWizard;
begin
  WorkspacePage :=
    CreateOutputMsgPage(
      wpWelcome,
      'Your SavvyLens workspace',
      'A home for captures, definitions, and exports',
      'SavvyLens creates a workspace at:' + #13#10 + #13#10 +
      'Documents\SavvyLens' + #13#10 + #13#10 +
      'It contains folders for CAN captures, DBC files, definitions, exports, ' +
      'and reusable script templates. You can organize or back up this folder ' +
      'independently of the application.' + #13#10 + #13#10 +
      'The workspace is not removed when SavvyLens is uninstalled.'
    );
end;