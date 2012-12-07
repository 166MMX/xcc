Name "Yuri's Revenge"
Outfile "Yuri's-Revenge-Multiplayer.exe"
InstallDir "$PROGRAMFILES\Red Alert 2"
InstallDirRegKey HKLM "Software\Westwood\Red Alert 2" "InstallDir"
XPStyle on
Page directory
Page instfiles

Section "Install"
	SetShellVarContext all
	SetOutPath "$INSTDIR"

	File "conquermd.dat"
	File "expandmd01.mix"
	File "langmd.mix"
	File "mapsmd03.mix"
	File "movmd03.mix"
	File "mphmd.exe"
	File "multimd.mix"
	File "ra2md.mix"
	File "xyr.dll"
	File "yuri's revenge.exe"
	CreateShortCut "$SMPROGRAMS\Yuri's Revenge.lnk" "$INSTDIR\Yuri.exe"
	WriteRegStr HKLM "Software\Westwood\Red Alert 2" "InstallDir" "$INSTDIR"
	WriteRegStr HKLM "Software\Westwood\Yuri's Revenge" "InstallPath" "$INSTDIR\Yuri.exe"
SectionEnd
