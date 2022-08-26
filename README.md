# Dark-Souls-III-Archipelago-client

Dark Souls III client made for Archipelago multiworld randomizer. See [archipelago.gg](https://archipelago.gg/)

## How to install
- Download the lastest version available [here](https://github.com/Marechal-L/Dark-Souls-III-Archipelago-client/releases).
- Copy the three following files (or extract DS3-Archipelago.zip) into your game folder :
	- _dinput8.dll
	- DarkSoulsIII_1.15.exe
	- DS3-Archipelago.bat
- Generate a game using Archipelago.
- Download the file from the room.
- Rename it into "AP.json".
- Copy the "AP.json" file into your game folder.
- Launch game by running "DS3-Archipelago.bat"
## Commands
- All client commands start with "/" and archipelago commands start with "!" :
	- /help : Prints this help message.
	- !help : to retrieve all archipelago commands
	- /connect {SERVER_IP}:{SERVER_PORT} : To connect to a server
	- /connect : Connect to the localhost:38281 server	
	
## Troubleshoots
- The provided dll requires other dependencies so if you encounter a crash when launching the game,
installing the latest Microsoft Visual C++ Redistributable version could fix it : https://aka.ms/vs/17/release/vc_redist.x64.exe.
- The Windows console tends to freeze preventing you from sending or receiving any items. You must Alt+Tab, click on the console and press enter to refresh it.

## Credits
https://github.com/LukeYui/DS3-Item-Randomiser-OS by LukeYui  
https://github.com/black-sliver/apclientpp by black-sliver



