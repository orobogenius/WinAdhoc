// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "ole32.lib")

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Uxtheme.h>
#include <NetCon.h>
#include <shellapi.h>
#include <Shobjidl.h>
#include <netlistmgr.h>
#include <comutil.h>
#include <wlanapi.h>
#include <atlstr.h>

//User Header Files
#include "ConnProfile.h"
#include "winAdhoc.h"
#include "UIControl.h"
#include "AdhocNetwork.h"
#include "INetSharing.h"
#include "Resource.h"
#include "WlanMgr.h"


// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <tchar.h>
#include <strsafe.h>
#include <string.h>
#include <memory.h>
#include <algorithm>
#include <string>


// TODO: reference additional headers your program requires here
