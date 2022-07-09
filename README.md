# DarkSouls3-Archipelago-client
Welcome to the beta of Dark Souls 3 made for the multiworld randomizer Archipelago!  
Please keep in mind that it is a beta and it does not represent the final state of this mod :)  

## Client :  
- Setup documentation : https://github.com/Marechal-L/Archipelago/blob/add_dark_souls_III/worlds/dark_souls_3/docs/setup_en.md  
- Some commands, all client commands start with "/" and archipelago commands start with "!" :  
	- !help : to retrieve all archipelago commands
	- /connect {SERVER_IP}:{SERVER_PORT} : To connect to a server
	- /kill : Because why not!
	- /itemGib {ITEM_ID_DECIMAL_BASE} : Cheat command to give you an item, make sure to use the DECIMAL value and NOT HEXADECIMAL

## Troubleshooting :  
- The game crashes at startup #1 :  
	Make sure the file AP.json is present on the root folder of your game aside DarkSoulsIII.exe and dinput8.dll  

- The game crashes at startup #2 :  
	The provided dll requires other dependencies. So if you encounter a crash when launching the game,   
	installing the latest Microsoft Visual C++ Redistributable version will do the trick : https://aka.ms/vs/17/release/vc_redist.x64.exe  

- The game crashes during a run and does not restart :   
	Retrieve the "seed" from AP.json and check the generated file {SEED_NUMBER}.json,  
	it must contain the list of received locations and not be empty.  
	If it is empty, delete it. It should restart the game properly to continue your run, this will also force the server to send all the received items again, ignore or use them.  

- I received the same item twice in a row :  
	When you are receiving multiple items in a row, check them in the console to make sure that there is no a key item in the list because this could be a blocking issue.  
	I am working on it and will address a fix as soon as possible, but for now I suggest you send yourself the item through the server command prompt ( /send {Player} Small Doll )
	Or use the cheat command from the client ( e.g. for the Small Doll : /itemGib 1073743829 ) according to the list of all items https://github.com/Marechal-L/Archipelago/blob/add_dark_souls_III/worlds/dark_souls_3/data/items_data.py  

- The client does not log any information :   
	The client command prompt seems to lose the focus and the thread set itself into a pending mode. Just ALT+TAB, click on the client, press enter and see if it wake it up!    


## Credits :  
Ludovic "Marech" Marechal  
https://github.com/Marechal-L/DarkSouls3-Archipelago-client  
