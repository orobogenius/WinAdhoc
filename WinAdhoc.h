#pragma once

//winAdhoc.h

/*
*	This is the base class for winAdhoc program
*/

#pragma once

#include "stdafx.h"

class WinAdhoc
{
public:

	/**
	* WinAdhoc() constructor for this class
	*
	* @param [in]   hInstance:- Handle to the programs executable module (the .exe file in memory)
	*
	*/
	WinAdhoc(HINSTANCE hInstance);
	WinAdhoc();
	~WinAdhoc(); //destructor
	void initWnd();
	bool Run(int nCmdShow);
	static void CenterWindow(HWND hwnd);
	HWND CreateTabControl(HWND hwndParent);
	HIMAGELIST CreateImageList();
	void CreateSettingsTab(HWND hwndTab);
	void CreateClientsTab(HWND hwndTab);
	void CreateStatusTab(HWND hwndTab);
	void CreateFonts();
	HWND CreateLabel(int x, int y, int width, int height, LPCWSTR text, HWND hwnd, HFONT font);
	HWND CreateEdit(int x, int y, int ID, long style, HWND hwnd);
	HWND CreateEditPassword(int x, int y, int ID, HWND hwnd);
	HWND CreateCombobox(int x, int y, int ID, HWND hwnd);
	HWND CreateCheckbox(int x, int y, int ID, LPCWSTR text, HWND hwnd);
	HWND CreateButton(int x, int y, int ID, LPCWSTR text, HWND hwnd);
	static void ShowSettingsTab();
	static void HideSettingsTab();
	static void ShowClientsTab();
	static void HideClientsTab();
	static void ShowStatusTab();
	static void HideStatusTab();
	static char toLower(char ch);
	HWND getWndHandle() { return wndHandle; }
	HINSTANCE getWndInstance() { return hInstance; }
	static std::string ToString(CString& text);
	BSTR ConcatBSTR(BSTR str1, BSTR str2);
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	WNDCLASSEX window;
	HINSTANCE hInstance;
	HWND wndHandle;
	HFONT labelFont, warnLabel;
	static wchar_t className[];
	static wchar_t classTitle[];
};