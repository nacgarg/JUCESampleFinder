#define Version "0.0.1"

[Setup]
AppName=SampleFinder
AppVersion={#Version}
DefaultDirName={pf}\SampleFinder
DefaultGroupName=SampleFinder     
Compression=lzma2               
SolidCompression=yes
OutputBaseFilename=SampleFinderx64Setup-{#Version}
; "ArchitecturesAllowed=x64" specifies that Setup cannot run on
; anything but x64.
ArchitecturesAllowed=x64
; "ArchitecturesInstallIn64BitMode=x64" requests that the install be
; done in "64-bit mode" on x64, meaning it should use the native
; 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64

;[Tasks]
;Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}";  GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "Builds\VisualStudio2017\x64\Release\App\SampleFinder.exe"; DestDir: "{app}"; DestName: "SampleFinder.exe"

[Dirs]
Name: "{app}"; Flags: uninsalwaysuninstall                    

[Icons]
Name: "{group}\SampleFinder"; Filename: "{app}\SampleFinder.exe"; WorkingDir: "{app}"