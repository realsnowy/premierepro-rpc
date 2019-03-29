#include <Windows.h>
#include <iostream>
#include <string>
#include <discord_rpc.h>
#include <time.h>
using namespace std;

HWND hWnd = FindWindow("Premiere Pro", NULL); //Finds a window with class name %s
char* text = new char[256];
string project;

void updateProject() //Updates the project name when you change projects
{
	GetWindowText(hWnd, text, 256); //Gets the window title text and dumps it to a buffer
	project = string(text); //Converts the buffer to a string

	if (project == "Adobe Premiere Pro CC 2018") //Change the version if necessary
	{
		project = "None"; //If no project is open, set project name to %s
		return;
	}
	project.erase(0,59);	//Erases the beginning part of the window title until it reaches the project name
							//Works based on how many characters the "Adobe Premiere Pro CC 20XX - " + "%YOUR_PATH%" has, so count all the characters until you reach the project name
							//If you do not change it the application will probably just crash or your text will not look as it should
	project.erase(project.end() - 7, project.end()); //Remove the extension from the project name
}

static void updatePresence() //Updates the RPC on Discord when you change projects
{
	char text[256];
	strcpy_s(text, "Project: "); //Adds %s infront of the project name
	strcat_s(text, project.c_str());
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = text; //Sets the RPC state to the project name
	discordPresence.details = "Adobe Premiere Pro CC"; //Sets the RPC name to %s
	discordPresence.startTimestamp = project == "None" ? NULL : time(0); //If project name = %s then remove 'Elapsed Time', otherwise just start from 0 seconds
	discordPresence.largeImageKey = "large"; //Sets the large image to %s
	discordPresence.smallImageKey = "small"; //Sets the small image to %s
	Discord_UpdatePresence(&discordPresence); //And finally, update the RPC on Discord
}

void initDiscord() //Initializes the RPC application
{
	Discord_Initialize("XXXXXXXXXXXXXXXXXX", NULL, NULL, NULL); //Replace the X's with your app's client ID
}

void winEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime) //This gets called when our hook detects a change in the window title
{
	updateProject();
	updatePresence();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	unsigned long proc;
	GetWindowThreadProcessId(hWnd, &proc);
	auto hook = SetWinEventHook(EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE, NULL, winEventProc, proc, 0, WINEVENT_OUTOFCONTEXT); //Set an event hook that gets triggered when the window title changes

	initDiscord();
	updateProject();
	updatePresence();

	MSG msg;
	BOOL bRet;

	while (1)
	{
		bRet = GetMessage(&msg, NULL, 0, 0);

		if (bRet > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if (bRet < 0)
		{
			
			
		}
		else
		{
			break;
		}
	}
	Discord_Shutdown(); //Shutdown the RPC
	UnhookWinEvent(hook); //Unhook the project name change hook
	return msg.wParam;
}

// © A lot of credits go to ewoudje#7551
// He helped me make most (all) of the stuff in this code

// I know this is a really shitty way to get a RPC from Premiere
// If you can make a better version of this, feel free to make a Pull Request
// I just suck at C++ :(

//     $$$$         $$$$
//   $$$$$$$$     $$$$$$$$
// $$$$$$$$$$$$ $$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$$$$
//  $$$$$$$$$$$$$$$$$$$$$$
//    $$$$$$$$$$$$$$$$$$$
//       $$$$$$$$$$$$$
//          $$$$$$$
//            $$$
//             $