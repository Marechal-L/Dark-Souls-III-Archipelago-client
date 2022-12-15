# Dark-Souls-III-Archipelago-client

Dark Souls III client made for Archipelago multiworld randomizer. See [archipelago.gg](https://archipelago.gg/)

## How to install

This mod works with a single dll file but we also provide a way to downpatch the game.

- Download the lastest version available [here](https://github.com/Marechal-L/Dark-Souls-III-Archipelago-client/releases).
- Extract DS3-Archipelago.zip into your game folder.
- Generate a game using Archipelago.
- Launch game by running "DS3-Archipelago.bat"

## Commands
- All client commands start with "/" and archipelago commands start with "!" :
	- /help : Prints this help message.
	- !help : to retrieve all archipelago commands
	- /connect {SERVER_IP}:{SERVER_PORT} {USERNAME} [password:{PASSWORD}]  
	Connect by entering SERVER_IP, SERVER_PORT and USERNAME. You can additionaly add a PASSWORD if requested by the server.
	
## Troubleshoots
- The provided dll requires other dependencies so if you encounter a crash when launching the game,
installing the latest Microsoft Visual C++ Redistributable version could fix it : https://aka.ms/vs/17/release/vc_redist.x64.exe.
- The Windows console tends to freeze preventing you from sending or receiving any items. You must Alt+Tab, click on the console and press enter to refresh it.

## Credits
https://github.com/LukeYui/DS3-Item-Randomiser-OS by LukeYui  
https://github.com/black-sliver/apclientpp by black-sliver



