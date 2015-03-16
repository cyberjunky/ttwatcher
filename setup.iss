; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "TTWatcher"
#define MyAppURL "https://github.com/altera2015/ttwatcher"
#define MyAppExeName "ttwatcher.exe"

#define Executable "build32\src\release\ttwatcher.exe"
#define ApplicationVersion GetFileVersion(Executable)


[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{CF7217FF-24C4-44C1-890A-DC3C61FA0015}
AppName={#MyAppName}
AppVersion={#ApplicationVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=LICENSE
OutputBaseFilename=TTWatcherSetup_x86_{#ApplicationVersion}
Compression=lzma
SolidCompression=yes
OutputDir=.


[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "build32\src\release\ttwatcher.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\icudt52.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\icuin52.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\icuio52.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\icule52.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\iculx52.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\icutu52.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\icuuc52.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\libeay32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\msvcp120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\msvcr120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\Qt5Xml.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\ssleay32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "dist\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "dist\imageformats\qjpeg.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

