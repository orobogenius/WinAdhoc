#ifndef UI_H
#define UI_H

#include <windows.h>
#include <commctrl.h>
#include <string>

#define SETTINGSTAB 101
#define IDC_MAIN_BUTTON 102
#define SSID 103
#define PASS 104
#define CONPASS 105


const char PROG_NAME[] = "winAdhoc";

int createUI(HINSTANCE thisInstance, HINSTANCE prevInstance, LPSTR cmdArg, int cmdShow);

void centerWindow(HWND hwnd);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif // UI_H
