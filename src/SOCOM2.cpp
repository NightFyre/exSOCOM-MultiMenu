///	NightFyres Public SOCOM-MultiMenu
///	Currently Supports SOCOM 2 - r0004
///	The following Includes file is only available to patreon / discord / youtube supporters
#include "../../../EXTERNAL/includes.h"

///	Follow the Guided Hacking Bible Book 1 if you want to compile this / make any changes yourself
///	You may request to join the reddit community which will get you access to the discord community
/// The discord community is where you will find various offsets for SOCOM 2 r0001 - r0004

/// <summary>
/// INITIAL COMMIT
/// :SOCOM 2 Brightness Patch
/// :PATCH r0004
/// :NOT ONLINE SAFE
/// </summary>

/// Here we will store all the process information
///	We use a namespace to keep things organized
namespace Process {
	const wchar_t* PROCESS = L"PlayStation2.exe";
	DWORD procID;
	DWORD exitCode;
	HANDLE hProc;
	uintptr_t moduleBase;
	m1 Memory;
}

//	Future offsets may be included within "includes.h" unless they have been made public
//	Many SOCOM codes can be found online although they will be for various patches
//	Porting codes from patch to patch is a fairly simple task if you have done the Game Hacking Bible Book 1 on GuidedHacking
namespace offsets {
	uintptr_t pBRIGHTNESS = 0x204B858Cl;
}

//	Storing game related variables here
//	again , using a namespace is just preference
namespace vars {
	//	MENU
	bool bMENU = false;
	const char* M1 = " ";

	//	BOOLS (CHEATS)
	bool bBRIGHTNESS;
}

int main()
{
	//	Set Console Environment
	cMENU::_setWindow(35, 7);
	cMENU::_setWindow(37, 8);
	SetConsoleTitle(L"exSOCOM | CONSOLE");
LABEL1:
	printf("[+] SEARCHING FOR PROCESS . . .\n");
	Sleep(1000);
	Process::procID = proc::GetProcId(Process::PROCESS);

	//	Get process id
	if (Process::procID)
	{
		cMENU::_clearConsole();
		printf("[+] PROCESS FOUND . . .\n");
		Sleep(500);
		printf("[+] GATHERING INFORMATION . . .\n");
		Sleep(500);

		//	Open Process
		Process::hProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, Process::procID);

		//	Get modulebase (this really isn't needed here. but it's good practice)
		Process::moduleBase = proc::GetModuleBaseAddress(Process::procID, Process::PROCESS);
		Process::Memory = m1(Process::hProc);		//	Memory Library (hint: simplification of ReadProcessMemory() and WriteProcessMemory())
		
		std::cout << "[+] PROC ID: " << Process::procID << "\n";
		Sleep(250);
		std::cout << "[+] moduleBase: " << Process::moduleBase << "\n";
		Sleep(250);
		std::cout << "[+] BRIGHTNESS OFFSET: " << offsets::pBRIGHTNESS << "\n";
		Sleep(250);

		printf("[+] LOADING MENU . . .");
		Sleep(1000);
	}
	else
	{
		cMENU::_clearConsole();
		printf("[+] PROCESS NOT FOUND\n");
		Sleep(1000);
		printf("[+] RETRYING IN 5 SECONDS\n");
		Sleep(5000);
		cMENU::_clearConsole();
		goto LABEL1;
	}

	Process::exitCode = 0;
	while (GetExitCodeProcess(Process::hProc, &Process::exitCode) && Process::exitCode == STILL_ACTIVE)
	{
		//	Display console selections
		if (!vars::bMENU)
		{
			cMENU::_clearConsole();
			cMENU::_setConsole();
			std::cout << " _______________________________ \n";
			std::cout << "|----SOCOM 2---------------r0004|\n";
			std::cout << "|[1] BRIGHTNESS             [" << vars::M1 << "] |\n";
			std::cout << "|[+] INCREASE                   |\n";
			std::cout << "|[-] DECREASE                   |\n";
			std::cout << "|[END] QUIT                     |\n";
			std::cout << "|--------------------NightFyre--|" << std::endl;
			vars::bMENU = true;
		}

		//	listen for hotkeys
		if (GetAsyncKeyState(VK_END) & 1) break;	//END

		//	Hack 1
		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			//	Toggle bool
			vars::bBRIGHTNESS = !vars::bBRIGHTNESS;

			//	When bool is toggled active
			if (vars::bBRIGHTNESS)
			{
				//	Apply our patch
				//	Example of byte patching , bytes must be entered in reverse order
				//	for instance 
				mem::PatchEx((BYTE*)offsets::pBRIGHTNESS, (BYTE*)"\x00\x00\x20\x41", 4, Process::hProc);

				//	toggle menu bool to apply changed string
				//	Sound effect for indication
				vars::M1 = "X";
				vars::bMENU = false;
				Beep(300, 300);
			}
			else //	When bool is toggled inactive
			{
				//	Disable our patch
				mem::PS2NopEx((BYTE*)offsets::pBRIGHTNESS, 4, Process::hProc);

				//	toggle menu bool to apply changed string
				//	Sound effect for indication
				vars::M1 = " ";
				vars::bMENU = false;
				Beep(200, 400);
			}

		}

		//	Loops for active bools
		if (vars::bBRIGHTNESS)
		{
			//	Since many factors will change the map brightness back to defualt
			//	We will just revert any changes the game tries to make
			if (Process::Memory.Read<float>(offsets::pBRIGHTNESS) == 0)
				mem::PatchEx((BYTE*)offsets::pBRIGHTNESS, (BYTE*)"\x00\x00\x20\x41", 4, Process::hProc);

			//	At this point we can get a little creative and include extra keybinds
			//	these will only be accessible while brightness mod is active
			//	here we can increase and decrease the brightness at will
			if (GetAsyncKeyState(VK_ADD) & 1)
			{
				//	Read our current value and assign it to a temporary variable
				float orig = Process::Memory.Read<float>(offsets::pBRIGHTNESS);
				
				//	We don't want our brightness to exceed 30 , this just doesn't look good
				if (orig != 30)
					Process::Memory.Write<float>(offsets::pBRIGHTNESS, (orig + 1));
			}

			//	Decreasing our brightness , our patch brings us to a float value of 10
			if (GetAsyncKeyState(VK_SUBTRACT) & 1)
			{
				//	Read our current value and assign it to a temporary variable
				float orig = Process::Memory.Read<float>(offsets::pBRIGHTNESS);
				
				//	Do not want to touch 0 , likewise , we also do not want to go below our patche value
				if (orig != 10)
					Process::Memory.Write<float>(offsets::pBRIGHTNESS, (orig - 1));
			}
		}
	}

	//	End the process
	if (vars::bBRIGHTNESS)
		mem::PS2NopEx((BYTE*)offsets::pBRIGHTNESS, 4, Process::hProc);
	return 0;
}