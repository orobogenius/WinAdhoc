#include "UI.h"

HWND settings_tab;
HWND ssid;
HWND pass;
HWND conpass;

int createUI(HINSTANCE thisInstance, HINSTANCE prevInstance, LPSTR cmdArg, int cmdShow)
{
    HWND hwnd; // Window Handle
    MSG message; // Window Events
    WNDCLASSEX window;// Window Object

    //Create and set
    window.hInstance = thisInstance;
    window.cbSize = sizeof(WNDCLASSEX);
    window.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    window.style = CS_DBLCLKS;
    window.lpfnWndProc = WndProc;
    window.lpszClassName = PROG_NAME;
    window.lpszMenuName = NULL;
    window.cbClsExtra = 0;
    window.cbWndExtra = 0;
    window.hCursor = LoadCursor(thisInstance, IDC_ARROW);
    window.hIcon = LoadIcon(thisInstance, IDI_APPLICATION);
    window.hIconSm = LoadIcon(thisInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&window))
        return 0;

    hwnd = CreateWindowEx(0, PROG_NAME, "winAdhoc :: By Orobogenius", WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 400, 510, HWND_DESKTOP
                          ,NULL, thisInstance, NULL);

    ShowWindow(hwnd, cmdShow);

    while (GetMessage (&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    return message.lParam;
}

void centerWindow(HWND hwnd) {
    RECT rc;
    GetWindowRect(hwnd, &rc);
    SetWindowPos(hwnd, 0, (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2, (GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2,
                 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT messages, WPARAM wParam, LPARAM lParam)
{

    //WC_TABCONTROL icex;
    switch(messages){
        case WM_CREATE: {
            //icex.dwsize = sizeof(InitCommonControlsEx);
            centerWindow(hwnd);

            ssid = CreateWindowEx(WS_EX_CLIENTEDGE,
                                   "EDIT",
                                   "",
                                   WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                                   145,
                                   85,
                                   230,
                                    30,
                                   hwnd,
                                   (HMENU) SSID,
                                   GetModuleHandle(NULL),
                                   NULL);
                        HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
                        SendMessage(ssid, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM(FALSE, 0));
                        SendMessage(ssid,
                                    WM_SETTEXT,
                                    NULL,
                                    (LPARAM)"");

        pass = CreateWindowEx(WS_EX_CLIENTEDGE,
                                   "EDIT",
                                   "",
                                   WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                                   145,
                                   135,
                                   230,
                                    30,
                                   hwnd,
                                   (HMENU) PASS,
                                   GetModuleHandle(NULL),
                                   NULL);
                        HGDIOBJ hfDefault2 = GetStockObject(DEFAULT_GUI_FONT);
                        SendMessage(pass, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM(FALSE, 0));
                        SendMessage(pass,
                                    WM_SETTEXT,
                                    NULL,
                                    (LPARAM)"");

        conpass = CreateWindowEx(WS_EX_CLIENTEDGE,
                                   "EDIT",
                                   "",
                                   WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                                   145,
                                   185,
                                   230,
                                    30,
                                   hwnd,
                                   (HMENU) CONPASS,
                                   GetModuleHandle(NULL),
                                   NULL);
                        HGDIOBJ hfDefaulte = GetStockObject(DEFAULT_GUI_FONT);
                        SendMessage(conpass, WM_SETFONT, (WPARAM) hfDefault, MAKELPARAM(FALSE, 0));
                        SendMessage(conpass,
                                    WM_SETTEXT,
                                    NULL,
                                    (LPARAM)"");
        }
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, messages, wParam, lParam);
    }
    return 0;
}
