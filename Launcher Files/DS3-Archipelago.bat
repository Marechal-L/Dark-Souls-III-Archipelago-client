@echo off
IF NOT EXIST ./DarkSoulsIII_1.15.exe (
	CHOICE /C AB /N /M "A: Patch with original DS3 v1.15 executable or B: Patch with speedrunning CrashFix DS3 v1.15 executable?
	IF ERRORLEVEL 1 (start /W ./xdelta/xdelta.exe -d -s "./DarkSoulsIII.exe" "./xdelta/DS3_1151_to_115_original.xdelta" "./DarkSoulsIII_1.15.exe")
	IF ERRORLEVEL 2 (start /W ./xdelta/xdelta.exe -d -s "./DarkSoulsIII.exe" "./xdelta/DS3_1151_to_115_crashfix.xdelta" "./DarkSoulsIII_1.15.exe")
)

IF EXIST "./_dinput8.dll" ren "./_dinput8.dll" "./dinput8.dll"
echo 374320 > ./steam_appid.txt
ren "./DarkSoulsIII.exe" "./_DarkSoulsIII.exe"
ren "./DarkSoulsIII_1.15.exe" "./DarkSoulsIII.exe"
start /W ./DarkSoulsIII.exe
ren "./DarkSoulsIII.exe" "./DarkSoulsIII_1.15.exe" 
ren "./_DarkSoulsIII.exe" "./DarkSoulsIII.exe"
IF EXIST "./steam_appid.txt" del /F "./steam_appid.txt"
IF EXIST "./dinput8.dll" ren "./dinput8.dll" "./_dinput8.dll"