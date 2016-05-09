#include "stdafx.h"

#define NUM_NETWORKS 10

HWND wHandle;
HINSTANCE wInstance;
HWND ssid, pass, conpass;
HWND networkList;
HWND iNetCB, iFCB;
HWND startBTN, stopBTN, minBTN, exBTN, refreshBTN;
HWND tabHandle;
HWND shareMsg, ssidLabel, passLabel, conpassLabel, warnMsg;
HWND statusArea;
int setIcon, clIcon, stIcon;
HIMAGELIST imageList;
NOTIFYICONDATA nID = {};

WlanMgr *man = new WlanMgr();
UIControl *ui = new UIControl();
wchar_t WinAdhoc::className[] = L"winAdhoc";
wchar_t WinAdhoc::classTitle[] = L"winAdhoc :: By Orobogenius";

WinAdhoc::WinAdhoc(HINSTANCE inst)
{
	hInstance = inst;
	initWnd();
}

WinAdhoc::WinAdhoc()
{

}

WinAdhoc::~WinAdhoc()
{
}

void WinAdhoc::initWnd()
{
	//Create and set
	window.cbSize = sizeof(WNDCLASSEX);
	window.hInstance = hInstance;
	window.hbrBackground = (HBRUSH) WHITE_BRUSH;
	window.style = CS_VREDRAW | CS_HREDRAW;
	window.lpfnWndProc = MainWndProc;
	window.lpszClassName = className;
	window.lpszMenuName = NULL;
	window.cbClsExtra = 0;
	window.cbWndExtra = 0;
	window.hCursor = LoadCursor(hInstance, IDC_ARROW);
	window.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	window.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	wInstance = hInstance;
}

bool WinAdhoc::Run(int nCmdShow)
{
	if (!RegisterClassEx(&window))
		return false;

	wndHandle = CreateWindowEx(0, className, classTitle, WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 400, 510, HWND_DESKTOP
						, NULL, hInstance, NULL);
	
	if (!wndHandle)
		return false;

	wHandle = wndHandle;

	SetWindowLong(wndHandle, GWL_EXSTYLE, GetWindowLong(wndHandle, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(wndHandle, RGB(255, 0, 0), 0, LWA_COLORKEY);

	CreateFonts();

	tabHandle = CreateTabControl(getWndHandle());

	CreateSettingsTab(tabHandle);

	CreateStatusTab(tabHandle);

	ShowWindow(wndHandle, nCmdShow);

	UpdateWindow(wndHandle);

	return true;
}

void WinAdhoc::CenterWindow(HWND hwnd) {
	RECT rc;
	GetWindowRect(hwnd, &rc);
	SetWindowPos(hwnd, 0, (GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2, (GetSystemMetrics(SM_CYSCREEN) - rc.bottom)/2, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

HWND WinAdhoc::CreateTabControl(HWND hwndParent)
{
	RECT rClient;
	INITCOMMONCONTROLSEX icex;
	HWND hwndTab;
	TCITEM tie;
	TCHAR buffer[256];

	//Initializez common controls
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_TAB_CLASSES;
	InitCommonControlsEx(&icex);

	//Create tab control of parent's window size
	GetClientRect(hwndParent, &rClient);
	hwndTab = CreateWindowEx(NULL, WC_TABCONTROL, L"", WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE, 0, 0,
		rClient.right+2, rClient.bottom+2, hwndParent, NULL, hInstance, NULL);

	if (!hwndTab)
		return NULL;

	SendMessage(hwndTab, WM_SETFONT, (WPARAM)labelFont, NULL);
	TabCtrl_SetItemSize(hwndTab, 20, 25);
	TabCtrl_SetImageList(hwndTab, CreateImageList());

	RECT tabRect;
	GetClientRect(hwndTab, &tabRect);

	SendMessage(hwndTab, TCM_ADJUSTRECT, FALSE, (LPARAM)&tabRect);
	
	for (int i = 0; i < 3; i++)
	{
		//Add tabs
		tie.mask = TCIF_IMAGE | TCIF_TEXT;
		tie.iImage = i;
		tie.pszText = buffer;
		//Load string from string resource
		LoadString(hInstance, IDS_SETTINGS + i, buffer, sizeof(buffer) / sizeof(buffer[0]));
		if (TabCtrl_InsertItem(hwndTab, i, &tie) == -1)
		{
			DestroyWindow(hwndTab);
			return NULL;
		}
	}
	return hwndTab;
}

HIMAGELIST WinAdhoc::CreateImageList()
{
	HIMAGELIST imageList;
	HICON icon;

	imageList = ImageList_Create(16, 16, ILC_MASK, 3, 0);

	icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON2));
	setIcon = ImageList_AddIcon(imageList, icon);
	
	icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	clIcon = ImageList_AddIcon(imageList, icon);

	icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON3));
	stIcon = ImageList_AddIcon(imageList, icon);

	return imageList;

}

void ShowNetworkInfo(INetwork *iNet, HWND hwnd)
{
	BSTR name = NULL;
	BSTR cate = L"";
	if (SUCCEEDED(iNet->GetName(&name)))
	{
		NLM_NETWORK_CATEGORY cat;
		if (SUCCEEDED(iNet->GetCategory(&cat)))
		{
			switch (cat)
			{
			case NLM_NETWORK_CATEGORY_PUBLIC:
				cate = L" (PUBLIC)";
				break;
			case NLM_NETWORK_CATEGORY_PRIVATE:
				cate = L" (PRIVATE)";
				break;
			case NLM_NETWORK_CATEGORY_DOMAIN_AUTHENTICATED:
				cate = L" (DOMAIN AUTHENTICATED)";
				break;
			default:
				break;
			}
			//lstrcatA(name, cate);

			//wcscpy(value, name);
			//wcscpy(value, cate);
			WinAdhoc *wA = new WinAdhoc();
			SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)wA->ConcatBSTR(name, cate));
			SendMessage(networkList, (UINT)CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			delete wA;
			wA = NULL;
			//	::SysFreeString(value);
		}
	}
}


void PopulateNetworkList(HWND hwnd)
{
	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
	{
		INetworkListManager *iNetList;
		if (SUCCEEDED(CoCreateInstance(CLSID_NetworkListManager, NULL, CLSCTX_ALL, IID_INetworkListManager, (void**)&iNetList)))
		{
			IEnumNetworks *iEnum;
			if (SUCCEEDED(iNetList->GetNetworks(NLM_ENUM_NETWORK_CONNECTED, &iEnum)))
			{
				INetwork *iNet[NUM_NETWORKS];
				BOOL bDone = FALSE;
				ULONG cFetched = NULL;
				BOOL noNet = TRUE;
				SendMessage(networkList, (UINT)CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
				while (!bDone)
				{
					if (SUCCEEDED(iEnum->Next(sizeof(NUM_NETWORKS), iNet, &cFetched)) && cFetched > 0)
					{
						noNet = FALSE;
						for (ULONG i = 0; i < cFetched; i++)
						{
							ShowNetworkInfo(iNet[i], hwnd);
							iNet[i]->Release();
						}
					}
					else {
						bDone = TRUE;
					}
				}
				if (noNet) {
					SendMessage(hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"No Network Available");
					SendMessage(hwnd, (UINT)CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
					EnableWindow(hwnd, false);
				}
				else {
					EnableWindow(hwnd, true);
				}
			}
			iEnum->Release();
		}
		iNetList->Release();
	}
	CoUninitialize();
}

void WinAdhoc::CreateSettingsTab(HWND hwndTab)
{		
	stopBTN = CreateButton(210, 400, STOP_BTN, L"Stop", hwndTab);

	EnableWindow(stopBTN, false);

	startBTN = CreateButton(30, 400, START_BTN, L"Start", hwndTab);

	iFCB = CreateCheckbox(10, 330, IFILE_CHECKBOX, L"Enable File Sharing", hwndTab);

	iNetCB = CreateCheckbox(10, 300, INET_CHECKBOX, L"Enable Internet Sharing", hwndTab);

	refreshBTN = CreateButton(320, 255, REFRESH, L"", hwndTab);

	networkList = CreateCombobox(10, 260, COMBOBOX, hwndTab);

	PopulateNetworkList(networkList);

	shareMsg = CreateLabel(10, 240, 500, 100, L"Select a Network to Share", tabHandle, labelFont);

	ssidLabel = CreateLabel(10, 90, 300, 100, L"NETWORK NAME (SSID): ", tabHandle, labelFont);

	passLabel = CreateLabel(10, 140, 500, 100, L"ENTER PASSWORD: ", tabHandle, labelFont);

	warnMsg = CreateLabel(175, 165, 300, 100, L"*Should not be less than 8 characters*", tabHandle, warnLabel);

	conpassLabel = CreateLabel(10, 190, 500, 100, L"CONFIRM PASSWORD: ", tabHandle, labelFont);

	conpass = CreateEditPassword(175, 185, PASS, hwndTab);

	pass = CreateEditPassword(175, 135, PASS, hwndTab);

	ssid = CreateEdit(175, 85, SSID, 0x00, hwndTab);

}

void WinAdhoc::CreateClientsTab(HWND hwndTab)
{

}

void WinAdhoc::CreateStatusTab(HWND hwndTab)
{
	RECT rect;
	GetClientRect(hwndTab, &rect);

	statusArea = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL
							, 0, 25, rect.right, rect.bottom, hwndTab, (HMENU)STATUSAREA, hInstance, NULL);

	SendMessage(statusArea, WM_SETFONT, (WPARAM)labelFont, NULL);
	SetParent(statusArea, hwndTab);
	//Edit_Enable(statusArea, false);
}

void WinAdhoc::CreateFonts()
{
	labelFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
				DEFAULT_PITCH, L"Comic Sans MS");

	warnLabel = CreateFont(14, 0, 0, 0, 700, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
				DEFAULT_PITCH, L"Comic Sans MS");
}

HWND WinAdhoc::CreateLabel(int x, int y, int width, int height, LPCWSTR text, HWND hwnd, HFONT font)
{

	HWND label = CreateWindowEx(WS_EX_TRANSPARENT, L"static", L"", WS_CHILD | WS_VISIBLE, x, y, width, height, hwnd, (HMENU)IDL_LABELS, GetModuleHandle(NULL), NULL);

	if (!label)
	{
		MessageBox(hwnd, L"Could not create label", L"Window Error", MB_OK | MB_ICONINFORMATION);
		GetLastError();
	}

	//SetParent(label, hwnd);
	SetWindowText(label, text);
	SendMessage(label, WM_SETFONT, (WPARAM)font, NULL);
	SetParent(label, wndHandle);

	return label;
}

HWND WinAdhoc::CreateEditPassword(int x, int y, int ID, HWND hwnd)
{
	HWND edit;

	edit = CreateWindowEx(NULL, L"EDIT", L"", ES_PASSWORD | WS_BORDER | WS_VISIBLE | WS_CHILD | WS_TABSTOP, x, y, 200,

		30, hwnd, (HMENU)ID, GetModuleHandle(NULL),
		NULL);

	if (!edit)
	{
		MessageBox(hwnd, L"Could not create text field", L"Window Error", MB_OK | MB_ICONINFORMATION);
		GetLastError();
		return NULL;
	}

	SendDlgItemMessage(hwnd, ID, EM_SETPASSWORDCHAR, (WPARAM) '+', (LPARAM)0);
	SendMessage(edit, WM_SETFONT, (WPARAM)labelFont, MAKELPARAM(FALSE, 0));
	SendMessage(edit, WM_SETTEXT, NULL, (LPARAM)"");
	SetParent(edit, wndHandle);

	return edit;
}

HWND WinAdhoc::CreateEdit(int x, int y, int ID, long style, HWND hwnd)
{
	HWND edit;

	edit = CreateWindowEx(NULL, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP, x, y, 200, 30, hwnd,
						(HMENU)ID, GetModuleHandle(NULL), NULL);

	if (!edit)
	{
		MessageBox(hwnd, L"Could not create text field", L"Window Error", MB_OK | MB_ICONINFORMATION);
		GetLastError();
		return NULL;
	}

	//HGDIOBJ hfDefault = GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(edit, WM_SETFONT, (WPARAM)labelFont, MAKELPARAM(FALSE, 0));
	if (ID == SSID) {
		SendMessage(edit, WM_SETTEXT, NULL, (LPARAM)L"GEN_NETWORK");
	}
	SetParent(edit, wndHandle);

	return edit;
}

HWND WinAdhoc::CreateCombobox(int x, int y, int ID, HWND hwnd)
{
	HWND box = CreateWindowEx(WS_EX_TRANSPARENT, L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y,

		300, 30, hwnd, (HMENU)COMBOBOX,
		GetModuleHandle(NULL), NULL);
	if (!box)
	{
		MessageBox(hwnd, L"Could not create ComboBox", L"Window Error", MB_OK | MB_ICONINFORMATION);
		GetLastError();
		return NULL;
	}

	SendMessage(box, WM_SETFONT, (WPARAM)labelFont, NULL);
	SetParent(box, wndHandle);

	return box;
}

HWND WinAdhoc::CreateCheckbox(int x, int y, int ID, LPCWSTR text, HWND hwnd)
{
	HWND check = CreateWindowEx(WS_EX_TRANSPARENT, L"BUTTON", text, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX | WS_TABSTOP, x, y, 300, 30, hwnd, (HMENU)ID, GetModuleHandle(NULL), NULL);

	if (!check)
	{
		MessageBox(hwnd, L"Could not create CheckBox", L"Window Error", MB_OK | MB_ICONINFORMATION);
		GetLastError();
		return NULL;
	}

	SendMessage(check, WM_SETFONT, (WPARAM)labelFont, NULL);
	SetParent(check, wndHandle);
	
	return check;
}

HWND WinAdhoc::CreateButton(int x, int y, int ID, LPCWSTR text, HWND hwnd)
{
	HWND button;
	HICON icon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON5));
	if (ID == REFRESH) {
		button = CreateWindowEx(NULL, L"BUTTON", L"", BS_ICON | WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON, x, y, 50, 33, hwnd,
			(HMENU)ID, GetModuleHandle(NULL), NULL);

		SendMessage(button, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)icon);
	}
	else
	{
		button = CreateWindowEx(NULL, L"BUTTON", text, WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, x, y, 130, 40, hwnd,
			(HMENU)ID, GetModuleHandle(NULL), NULL);
	}

	if (!button)
	{
		MessageBox(hwnd, L"Could not create Button", L"Window Error", MB_OK | MB_ICONINFORMATION);
		GetLastError();
		return NULL;
	}

	SendMessage(button, WM_SETFONT, (WPARAM)labelFont, NULL);
	SetParent(button, wndHandle);
	return button;
}

void WinAdhoc::ShowSettingsTab()
{
	ShowWindow(ssidLabel, SW_SHOW);
	ShowWindow(passLabel, SW_SHOW);
	ShowWindow(conpassLabel, SW_SHOW);
	ShowWindow(ssid, SW_SHOW);
	ShowWindow(pass, SW_SHOW);
	ShowWindow(conpass, SW_SHOW);
	ShowWindow(warnMsg, SW_SHOW);
	ShowWindow(shareMsg, SW_SHOW);
	ShowWindow(networkList, SW_SHOW);
	ShowWindow(refreshBTN, SW_SHOW);
	ShowWindow(iNetCB, SW_SHOW);
	ShowWindow(iFCB, SW_SHOW);
	ShowWindow(startBTN, SW_SHOW);
	ShowWindow(stopBTN, SW_SHOW);
}

void WinAdhoc::HideSettingsTab()
{
	ShowWindow(ssidLabel, SW_HIDE);
	ShowWindow(passLabel, SW_HIDE);
	ShowWindow(conpassLabel, SW_HIDE);
	ShowWindow(ssid, SW_HIDE);
	ShowWindow(pass, SW_HIDE);
	ShowWindow(conpass, SW_HIDE);
	ShowWindow(warnMsg, SW_HIDE);
	ShowWindow(shareMsg, SW_HIDE);
	ShowWindow(networkList, SW_HIDE);
	ShowWindow(refreshBTN, SW_HIDE);
	ShowWindow(iNetCB, SW_HIDE);
	ShowWindow(iFCB, SW_HIDE);
	ShowWindow(startBTN, SW_HIDE);
	ShowWindow(stopBTN, SW_HIDE);
}

void WinAdhoc::ShowClientsTab()
{
	//Show the clients tab
}

void WinAdhoc::HideClientsTab()
{
	//Hide the clients tab
}

void WinAdhoc::ShowStatusTab()
{
	//Show the status tab
	ShowWindow(statusArea, SW_SHOW);
}

void WinAdhoc::HideStatusTab()
{
	//Hide the status tab
	ShowWindow(statusArea, SW_HIDE);
}


std::string WinAdhoc::ToString(CString& text) {
	std::wstring ws(text);
	std::string str(ws.begin(), ws.end());
	return str;
}

BSTR WinAdhoc::ConcatBSTR(BSTR str1, BSTR str2)
{
	_bstr_t _bstr1(str1);
	_bstr_t _bstr2(str2);
	_bstr_t res = _bstr1 + _bstr2;
	return res.copy();
}

char WinAdhoc::toLower(char ch) {
	if (ch <= 'Z' && ch >= 'A')
		return ch - ('Z' - 'z');
	return ch;
}

void CreateNotifyIconData(HINSTANCE hInstance, HWND hwnd)
{
	// {06D8B718-76D3-444E-969B-0C6610107782}
	//static const GUID notGUID = { 0x6d8b718, 0x76d3, 0x444e,{ 0x96, 0x9b, 0xc, 0x66, 0x10, 0x10, 0x77, 0x82 } };
	nID.uVersion = NOTIFYICON_VERSION_4;
	nID.cbSize = sizeof(NOTIFYICONDATA);
	nID.hWnd = hwnd;
	nID.uFlags = NIF_TIP | NIF_SHOWTIP | NIF_MESSAGE | NIF_GUID | NIF_ICON;
	//nID.dwInfoFlags = NIF_ICON | NIF_TIP | NIF_GUID | NIF_INFO;
	//nID.guidItem = notGUID;
	nID.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON4));
	nID.uID = 100;
	nID.uTimeout = 500;
	nID.uCallbackMessage = WM_ICONMSG;
	//nID.hBalloonIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON4));
	wcscpy_s(nID.szTip, L"WinAdhoc :: By Orobogenius");
	//StringCchCopy(nID.szInfoTitle, ARRAYSIZE(nID.szInfoTitle), L"");
	//StringCchCopy(nID.szInfo, ARRAYSIZE(nID.szInfo), L"WinAdhoc");
	//StringCchCopy(nID.szInfoTitle, ARRAYSIZE(nID.szInfoTitle), );
}

void RemoveNotIcon(HWND hwnd)
{
	nID.cbSize = sizeof(NOTIFYICONDATA);
	nID.hWnd = hwnd;
	nID.uID = 100;
	Shell_NotifyIcon(NIM_DELETE, &nID);
}

void RemoveTab()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		ITaskbarList *iTask;
		hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList, reinterpret_cast<void**>(&iTask));
		if (SUCCEEDED(hr))
		{
			hr = iTask->DeleteTab(wHandle);
			if (SUCCEEDED(hr))
			{
				MessageBox(NULL, L"winAdhoc has been minimized to tray!", L"Minimized", MB_OK);
			}
			iTask->Release();
		}
		CoUninitialize();
	}
}

void AddTab()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		ITaskbarList *iTask;
		hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList, reinterpret_cast<void**>(&iTask));
		if (SUCCEEDED(hr))
		{
			hr = iTask->AddTab(wHandle);
			if (SUCCEEDED(hr))
			{
				//Restore window
				RemoveNotIcon(wHandle);
				ShowWindow(wHandle, SW_RESTORE);
				SetActiveWindow(wHandle);
				SetForegroundWindow(wHandle);
			}
			iTask->Release();
		}
		CoUninitialize();
	}
}

LRESULT CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return true;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_MIN:
		{
			//minimize to tray
			EndDialog(hwnd, wParam);
			AnimateWindow(wHandle, 200, AW_SLIDE);
			ShowWindow(wHandle, SW_HIDE);
			RemoveTab();
			CreateNotifyIconData(wInstance ,wHandle);
			Shell_NotifyIcon(NIM_ADD, &nID);
			Shell_NotifyIcon(NIM_SETVERSION, &nID);
		}
			break;
		case ID_EXIT:
			EndDialog(hwnd, wParam);
			return TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hwnd, wParam);
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WinAdhoc::MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdcStatic = NULL;
	//WC_TABCONTROL icex;
	switch (msg) {
	case WM_CREATE:
		//icex.dwsize = sizeof(InitCommonControlsEx);			
		CenterWindow(hwnd);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case INET_CHECKBOX:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hwnd, INET_CHECKBOX, BM_GETCHECK, 0, 0))
				{
					//Set Internet sharing enabled flag
				}
				else {
					//Set Internet sharing disabled flag
				}
				break;
			}
			break;
		case IFILE_CHECKBOX:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				if (SendDlgItemMessage(hwnd, IFILE_CHECKBOX, BM_GETCHECK, 0, 0))
				{
					//Set File sharing enabled flag
				}
				else {
					//Set File sharing disabled flag
				}
				break;
			}
			break;			
		case START_BTN:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
			{
				CString ssidText, passText, conpassText;
				int nLen = GetWindowTextLength(ssid);
				GetWindowText(ssid, ssidText.GetBufferSetLength(nLen), nLen+1);
				nLen = GetWindowTextLength(pass);
				GetWindowText(pass, passText.GetBufferSetLength(nLen), nLen+1);
				nLen = GetWindowTextLength(conpass);
				GetWindowText(conpass, conpassText.GetBufferSetLength(nLen), nLen+1);
				if (ssidText == L"GEN_NETWORK" && lstrlen(passText)>=8 && lstrlen(conpassText) >=8 && (passText == conpassText))
				{
					//ConnProfile profile(ToString(passText));
				}
				else {
					if (lstrlen(ssidText) == 0)
					{
						MessageBox(NULL, L"SSID cannot be empty", L"Service Error", MB_ICONERROR);
						SetFocus(ssid);
						break;
					}
					else if (lstrlen(passText) == 0)
					{
						MessageBox(NULL, L"Password cannot be empty", L"Service Error", MB_ICONERROR);
						SetFocus(pass);
						break;
					}
					else if (lstrlen(conpassText) == 0)
					{
						MessageBox(NULL, L"Please confirm your password", L"Service Error", MB_ICONERROR);
						SetFocus(conpass);
						break;
					}
					else if (ToString(passText) != ToString(conpassText))
					{
						MessageBox(NULL, L"Passwords do not match", L"Service Error", MB_ICONERROR);
						break;
					}
					else if (lstrlen(passText) < 8)
					{
						MessageBox(NULL, L"Password length should not be less than 8 characters", L"Service Error", MB_ICONERROR);
						break;
					}
				}

				man->Init();
				man->SetHostedNetworkName(ssidText);
				man->SetHostedNetworkKey(passText);
				HRESULT hr = man->StartHostedNetwork();
				if (hr == S_OK)
				{
					MessageBox(hwnd, L"Service Started!", L"", MB_OK);
					ui->setStarted(true);
					man->OnHostedNetworkStarted();
				}

				if (ui->IsStarted())
				{
					DWORD length = GetWindowTextLength(statusArea);

					SendMessage(statusArea, EM_SETSEL, (WPARAM)length, (LPARAM)length);

					SendMessage(statusArea, EM_REPLACESEL, 0, (LPARAM)L"\r\nService started successfully");
					
					EnableWindow(startBTN, false);

					EnableWindow(stopBTN, true);
				}
				else {
					MessageBox(NULL, L"The service could not be started", L"Service Error", MB_ICONINFORMATION);
					break;
				}

			}
			break;
			}
			break;
		case REFRESH:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
				PopulateNetworkList(networkList);
				break;
			}
			break;
		case STOP_BTN:
			switch (HIWORD(wParam))
			{
			case BN_CLICKED:
			{
				//Check if instance is running
				if (ui->IsStarted())
				{
					//stop hotspot
					man->StopHostedNetwork();
					man->OnHostedNetworkStopped();
					EnableWindow(startBTN, true);
					EnableWindow(stopBTN, false);
					ui->setStarted(false);

					DWORD length = GetWindowTextLength(statusArea);

					SendMessage(statusArea, EM_SETSEL, (WPARAM)length, (LPARAM)length);

					SendMessage(statusArea, EM_REPLACESEL, 0, (LPARAM)L"\r\nService stopped successfully");
				}
				else {
					MessageBox(NULL, L"You have not started the service", L"Service Error", MB_OK | MB_ICONERROR);
				}
			}
			break;
			}
			break;
		}
		break;
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case TCN_SELCHANGING:
		{
			// Return FALSE to allow the selection to change.
			return FALSE;
		}

		case TCN_SELCHANGE:
		{
			int tab = TabCtrl_GetCurSel(tabHandle);
			switch (tab)
			{
			case 0:
				HideClientsTab();
				HideStatusTab();
				ShowSettingsTab();
				break;
			case 1:
				HideSettingsTab();
				HideStatusTab();
				ShowClientsTab();
				break;
			case 2:
				HideSettingsTab();
				HideClientsTab();
				ShowStatusTab();
				break;
			default:
				break;
			}
			break;
		}
		}
	}
	break;
	case WM_KEYDOWN:
	{
		if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'a')
		{
			SendMessage(ssid, EM_SETSEL, 0, -1);
		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		//Create exit dialog
		if (DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DLGEXIT), hwnd, reinterpret_cast<DLGPROC>(DlgProc))==ID_EXIT)
		{
			int response = MessageBox(hwnd, L"Exiting will end all connections. Continue?", L"Exit", MB_ICONEXCLAMATION | MB_OKCANCEL);
			if (response == IDOK)
			{
				//Close connections and exit
				delete man;
				man = NULL;
				DestroyWindow(hwnd);
			}
			break;
		}
		break;

	case WM_ICONMSG:
		switch (LOWORD(lParam))
		{
		case WM_LBUTTONDOWN:
			AddTab();
			break;
		}
		break;
	case WM_CTLCOLORSTATIC:
		hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(0, 0, 0));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LRESULT)GetStockObject(NULL_BRUSH);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}