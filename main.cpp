#include "stdafx.h"


/**
* WinMain() to register our window class
*
* This continues into the more detailed description of the function.
*
* @param [in]   hInstance:- Handle to the programs executable module (the .exe file in memory)
* @param [in]   hPrevInstance:- Always NULL for Win32 programs.Handle to the previous instance of the application
* @param [in]   cmdArg:- Pointer to a null-terminated string of 8-bit characters.specifying the command line
for the application,excluding the program name
* @param [in]   nCmdShow:- How the window is to be shown
*
* @return    msg.wParam    it returns wParam value of each message.
*
*/

int WINAPI wWinMain(HINSTANCE thisInstance, HINSTANCE prevInstance, LPTSTR cmdArg, int nCmdShow)
{
	MSG msg;
	WinAdhoc *winAdhoc = new WinAdhoc(thisInstance);
	if (!winAdhoc->Run(nCmdShow))
	{
		MessageBox(NULL, L"Unable to create window", L"Handle Error", MB_OK | MB_ICONINFORMATION);
		GetLastError();
		delete winAdhoc;
		winAdhoc = NULL;
		return 1;
	}


	/*
	* Message loop for this hinstance.
	*/
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!IsDialogMessage(winAdhoc->getWndHandle(), &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete winAdhoc;
	winAdhoc = NULL;
	return msg.wParam;
}