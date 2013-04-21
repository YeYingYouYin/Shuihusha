; �ű��� Inno Setup �ű��� ���ɣ�
; �йش��� Inno Setup �ű��ļ�����ϸ��������İ����ĵ���

#define MyAppName "ˮ�ɱ���˽��ع���"
#define MyAppVersion "F4.5"
#define MyAppPublisher "���ӻṤ����"
#define MyAppURL "http://weibo.com/tianzihui"
#define MyAppExeName "Shuihusha.exe"

[Setup]
; ע: AppId��ֵΪ������ʶ��Ӧ�ó���
; ��ҪΪ������װ����ʹ����ͬ��AppIdֵ��
; (�����µ�GUID����� ����|��IDE������GUID��)
AppId={{AA1F59C0-5169-48A5-AD1E-14543EF03130}
AppName={#MyAppName}
AppVersion={#MyAppVersion}AppVerName=ˮ�ɱ�����ս�� {#MyAppVersion} ���˽��ع�
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
;AppUpdatesURL={#MyAppURL}

AppCopyright=?2011-2013 TianziClub QSanguosha Software
VersionInfoVersion=4.5.0.0
VersionInfoCompany=TianziClub Software
VersionInfoDescription=���ӻ�ˮ�ɱ
VersionInfoTextVersion=4, 4, 0, 0

DefaultDirName={pf}\Shuihusha
;DisableDirPage=yes
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=readme1.txt
InfoBeforeFile=readme2.txt
InfoAfterFile=readme3.txtOutputDir=.
OutputBaseFilename=Shuihusha{#MyAppVersion}-Setup
SetupIconFile=package.ico
WizardImageFile=border.bmp
WizardSmallImageFile=logo.bmp
Compression=lzma
SolidCompression=yes

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";
; OnlyBelowVersion: 0,6.1
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Components]
Name: main; Description: "������(��ѡ)"; Types: full compact custom; Flags: fixed
Name: lua; Description: "ʾ��LUA";
;Name: lua\sanguosha; Description: "ɡ��ɱ";
Name: zhangong; Description: "ս��ϵͳ";
Name: doc; Description: "�ο��ĵ�"; Types: full;

[Files]
Source: "..\Shuihusha.exe"; DestDir: "{app}"; Flags: ignoreversion 
;Source: "..\ˮ�ɱ����&����Ⱥ��.txt"; DestDir: "{app}"; Flags: isreadme

Source: "..\*"; DestDir: "{app}"; Excludes: "\inno\*,\swig\*,\extension*\*,lua5*.dll,\config.ini"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: main
Source: "..\extensions\customcards.*"; DestDir: "{app}\extensions"; Flags: ignoreversion ; Components: main
 
Source: "..\extensions\zhangong\*"; DestDir: "{app}\extensions\zhangong"; Excludes: "zhangong.data"; Flags: ignoreversion recursesubdirs createallsubdirs ; Components: zhangong
Source: "..\extensions\*sqlite*"; DestDir: "{app}\extensions"; Flags: ignoreversion ; Components: zhangong
Source: "..\extensions\zhangong.*"; DestDir: "{app}\extensions"; Flags: ignoreversion ; Components: zhangong 
Source: "..\lua5*.dll"; DestDir: "{app}"; Flags: ignoreversion ; Components: zhangong
 
Source: "..\extensions\ai\*"; DestDir: "{app}\extensions\ai"; Flags: ignoreversion recursesubdirs createallsubdirs ; Components: lua
Source: "..\extensions\audio\*"; DestDir: "{app}\extensions\audio"; Flags: ignoreversion recursesubdirs createallsubdirs ; Components: lua
Source: "..\extensions\generals\*"; DestDir: "{app}\extensions\generals"; Flags: ignoreversion recursesubdirs createallsubdirs ; Components: lua
Source: "..\extensions\draggon.*"; DestDir: "{app}\extensions"; Flags: ignoreversion ; Components: lua
Source: "..\extensions\personal.*"; DestDir: "{app}\extensions"; Flags: ignoreversion ; Components: lua
Source: "..\extensions\sanguosha.*"; DestDir: "{app}\extensions"; Flags: ignoreversion ; Components: lua  

Source: "..\extension-doc\*"; DestDir: "{app}\extension-doc"; Flags: ignoreversion recursesubdirs createallsubdirs ; Components: doc
Source: "..\swig\*"; DestDir: "{app}\swig"; Flags: ignoreversion recursesubdirs createallsubdirs ; Components: doc
; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon
Name: "{commondesktop}\�鿴ˮ�ɱս��"; Filename: "{app}\extensions\zhangong.hta"; Tasks: desktopicon; Components: zhangong

[Run]
Filename: "{app}\extensions\reg_sqlite.bat"; Flags: shellexec; Components: zhangong
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

