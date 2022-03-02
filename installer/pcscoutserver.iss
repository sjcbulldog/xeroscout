; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "XeroCentral"
#define MyCoachName "XeroCoach"
#define MyScoutName "XeroScout"
#define MyViewerName "XeroView"
#define MyAppVersion "0.5.8"
#define MyAppPublisher "ErrorCodeXero"
#define MyAppURL "http://www.wilsonvillerobotics.com/"
#define MyAppExeName "PCScouter.exe"
#define MyScoutAppExeName "PCScoutApp.exe"
#define MyFormViewerExeName "PCFormViewer.exe"
#define MyAppSourceDir "d:\cygwin64\home\bwg\robottools\xeroscout"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{3FF7C27E-BF12-48A1-A349-5BE6EE0EA824}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableProgramGroupPage=yes
LicenseFile={#MyAppSourceDir}\LICENSE
; Remove the following line to run in administrative install mode (install for all users.)
PrivilegesRequired=lowest
OutputDir={#MyAppSourceDir}\installer
OutputBaseFilename=xeroscout-{#MyAppVersion}
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ChangesAssociations=yes

[CustomMessages]
CreateDesktopIcon=Create desktop icons

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "{#MyAppSourceDir}\PCScouter\x64\Release\*.*"; DestDir: "{app}"; Flags: ignoreversion 64bit recursesubdirs
Source: "{#MyAppSourceDir}\forms\*.*"; DestDir: "{userdocs}\forms"; Flags: ignoreversion 64bit recursesubdirs

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{autoprograms}\{#MyCoachName}"; Filename: "{app}\{#MyAppExeName}"; Parameters: "--coach"
Name: "{autodesktop}\{#MyCoachName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; Parameters: "--coach"
Name: "{autoprograms}\{#MyScoutName}"; Filename: "{app}\{#MyScoutAppExeName}"
Name: "{autodesktop}\{#MyScoutName}"; Filename: "{app}\{#MyScoutAppExeName}"; Tasks: desktopicon
Name: "{autoprograms}\{#MyViewerName}"; Filename: "{app}\{#MyFormViewerExeName}"
Name: "{autodesktop}\{#MyViewerName}"; Filename: "{app}\{#MyFormViewerExeName}"; Tasks: desktopicon

[Run]
Filename: {app}\VC_redist.x64.exe; \
    Parameters: "/q /passive /Q:a /c:""msiexec /q /i vcredist.msi"""; \
    StatusMsg: "Installing VC++ 2019 Redistributables..."

