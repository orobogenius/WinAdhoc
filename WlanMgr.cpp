#include "stdafx.h"
#include "WlanMgr.h"


WlanMgr::WlanMgr()
{
	if (wlanHandle != NULL)
	{
		WlanCloseHandle(wlanHandle, NULL);
		wlanHandle = NULL;
	}
}

WlanMgr::~WlanMgr()
{
}

HRESULT WlanMgr::Init()
{
	HRESULT hr = S_OK;
	DWORD dwError = ERROR_SUCCESS;
	DWORD dSize = 0;
	BOOL *dData = NULL;

	PWLAN_HOSTED_NETWORK_CONNECTION_SETTINGS pConnSettings = NULL;
	PWLAN_HOSTED_NETWORK_SECURITY_SETTINGS pSecSettings = NULL;
	PWLAN_HOSTED_NETWORK_STATUS pStatus = NULL;
	WLAN_OPCODE_VALUE_TYPE valueType;


	dwError = WlanOpenHandle(WLAN_API_VERSION, NULL, &version, &wlanHandle);

	if (dwError != ERROR_SUCCESS)
	{
		MessageBox(NULL, L"Could not open handle", L"Handle Error", MB_OK);
		return hr = ERROR_INVALID_STATE;
	}

	if (initialized)
	{
		goto error;
	}

	dwError = WlanHostedNetworkInitSettings(wlanHandle, NULL, NULL);

	if (dwError != ERROR_SUCCESS)
	{
		MessageBox(NULL, L"Could not init settings", L"Init Error", MB_OK);
		hr = ERROR_INVALID_STATE;
		goto error;
	}

	dwError = WlanHostedNetworkQueryProperty(wlanHandle, wlan_hosted_network_opcode_enable, &dSize, (PVOID*)&dData, &valueType, NULL);

	if (!dData || dSize < sizeof(BOOL))
	{
		MessageBox(NULL, L"Unable to query property", L"Property Error", MB_OK);
		hr = ERROR_INVALID_DATA;
		goto error;
	}

	dwError = WlanHostedNetworkQueryProperty(wlanHandle, wlan_hosted_network_opcode_connection_settings, &dSize, (PVOID*)&pConnSettings, &valueType, NULL);

	if (!pConnSettings || dSize < sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS))
	{
		MessageBox(NULL, L"Unable to query property", L"Property Error", MB_OK);
		hr = ERROR_INVALID_DATA;
		goto error;
	}

	dwError = WlanHostedNetworkQueryProperty(wlanHandle, wlan_hosted_network_opcode_security_settings, &dSize, (PVOID*)&pSecSettings, &valueType, NULL);

	if (!pSecSettings || dSize < sizeof(WLAN_HOSTED_NETWORK_SECURITY_SETTINGS))
	{
		MessageBox(NULL, L"Unable to query property", L"Property Error", MB_OK);
		hr = ERROR_INVALID_DATA;
		goto error;
	}

	dwError =
		WlanHostedNetworkQueryStatus(wlanHandle, &pStatus, NULL);

	connSettings = *pConnSettings;
	secSettings = *pSecSettings;
	netState = pStatus->HostedNetworkState;
	allowed = *dData;
	netStatus = *pStatus;

	initialized = true;

error:

	if (dwError != ERROR_SUCCESS && wlanHandle != NULL)
	{
		//
		// Close WLAN handle in failure cases
		//
		WlanCloseHandle(wlanHandle, NULL);
		wlanHandle = NULL;
	}

	if (dData != NULL)
	{
		WlanFreeMemory(dData);
		dData = NULL;
	}

	if (pConnSettings != NULL)
	{
		WlanFreeMemory(pConnSettings);
		pConnSettings = NULL;
	}

	if (pSecSettings != NULL)
	{
		WlanFreeMemory(pSecSettings);
		pSecSettings = NULL;
	}

	if (pStatus != NULL)
	{
		WlanFreeMemory(pStatus);
		pStatus = NULL;
	}

	return hr;
}

HRESULT WlanMgr::SetHostedNetworkName(CString& ssidStr)
{
	HRESULT hr = S_OK;
	DWORD dwError = ERROR_SUCCESS;
	//ULONG ssidSize = sizeof(ssidStr)/sizeof(wchar_t);
	DOT11_SSID ssid, oldSsid;

	//ssid.uSSIDLength = ssidSize;
	//ssid.ucSSID = ssidStr;
	//wcstombs(ssid.ucSSID, ssidStr, ssid.uSSIDLength);

	dwError = StringToSsid(ssidStr, &ssid);

	if (initialized)
	{
		oldSsid = connSettings.hostedNetworkSSID;
		connSettings.hostedNetworkSSID = ssid;

		dwError = WlanHostedNetworkSetProperty(wlanHandle, wlan_hosted_network_opcode_connection_settings, sizeof(WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS), (PVOID)&connSettings, NULL, NULL);

		if (dwError != ERROR_SUCCESS)
		{
			connSettings.hostedNetworkSSID = oldSsid;
			hr = HRESULT_FROM_WIN32(dwError);
		}
	}
	else {
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
	}


	return hr;
}

HRESULT WlanMgr::SetHostedNetworkKey(CString& passStr)
{
	HRESULT hr = S_OK;
	DWORD dwError = ERROR_SUCCESS;
	UCHAR strKeyBuf[WLAN_MAX_NAME_LENGTH];
	DWORD dwStrKeyBufLen = WLAN_MAX_NAME_LENGTH;

	dwError = ConvertPassPhraseKeyStringToBuffer(
		passStr, passStr.GetLength(), DOT11_AUTH_ALGO_RSNA_PSK, strKeyBuf,
		&dwStrKeyBufLen);

	if (0 == dwStrKeyBufLen)
	{
		MessageBox(NULL, L"Too short", L"Pass Error", MB_OK);
	}

	dwError = WlanHostedNetworkSetSecondaryKey(wlanHandle, dwStrKeyBufLen, strKeyBuf, TRUE, TRUE, NULL, NULL);

	return hr;

}

HRESULT WlanMgr::StartHostedNetwork()
{
	HRESULT hr = S_OK;
	DWORD dwError = ERROR_SUCCESS;

	if (!initialized)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		return hr;
	}

	if (wlan_hosted_network_active == netState)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		return hr;
	}

	dwError = WlanHostedNetworkStartUsing(
		wlanHandle,
		NULL,
		NULL
		);

	if (dwError != ERROR_SUCCESS)
	{
		//MessageBox(NULL, L"FAILED", L"ASDF", MB_OK);
	}

	return hr;
}

HRESULT WlanMgr::ForceStopHostedNetwork()
{
	HRESULT hr = S_OK;

	DWORD dwError = ERROR_SUCCESS;

	if (!initialized)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		goto error;
	}

	if (netState != wlan_hosted_network_active)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		goto error;
	}

	//
	// Stop hosted network
	//
	dwError = WlanHostedNetworkForceStop(
		wlanHandle,
		NULL,
		NULL
		);
	
	if (dwError != ERROR_SUCCESS)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		goto error;
	}

error:

	return hr;

}

HRESULT WlanMgr::ForceStartHostedNetwork()
{
	HRESULT hr = S_OK;

	DWORD dwError = ERROR_SUCCESS;

	if (!initialized)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		goto error;
	}

	if (netState != wlan_hosted_network_active)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		goto error;
	}

	//
	// Stop hosted network
	//
	dwError = WlanHostedNetworkForceStart(
		wlanHandle,
		NULL,
		NULL
		);

	if (dwError != ERROR_SUCCESS)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		goto error;
	}

error:

	return hr;
}

void WlanMgr::OnHostedNetworkStarted()
{
	//CHostedNetworkNotificationSink * pSink = NULL;

	//Lock();

	//
	// Change hosted network state
	//
	netState = wlan_hosted_network_active;

	//pSink = GetNotificationSink();

	//Unlock();

	/*
	if (pSink != NULL)
	{
		//
		// Notify client
		//
		pSink->OnHostedNetworkStarted();

		_ASSERT(m_CallbackComplete != NULL);

		//
		// Signal callback complete
		//
		SetEvent(m_CallbackComplete);
	}
	*/
}

HRESULT WlanMgr::StopHostedNetwork()
{
	HRESULT hr = S_OK;

	DWORD dwError = ERROR_SUCCESS;

	if (!initialized)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		return hr;
	}

	if (netState != wlan_hosted_network_active)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_STATE);
		return hr;
	}

	dwError = WlanHostedNetworkStopUsing(
		wlanHandle,
		NULL,
		NULL
		);

	return hr;
}

void WlanMgr::OnHostedNetworkStopped()
{
	//CHostedNetworkNotificationSink * pSink = NULL;
	//CRefObjList<CWlanStation *> stationList;

	//Lock();
	/*
	// Save all stations that haven't left the network
	while (0 != m_StationList.GetCount())
	{
		CWlanStation* pStation = m_StationList.RemoveHead();

		stationList.AddTail(pStation);
	}

	*/
	netState = wlan_hosted_network_idle;
	/*
	pSink = GetNotificationSink();

	Unlock();

	if (pSink != NULL)
	{
		//
		// Notify client
		//

		// notify station leave
		while (0 != stationList.GetCount())
		{
			CWlanStation* pStation = stationList.RemoveHead();

			pSink->OnStationLeave(pStation);

			pStation->Release();

			pStation = NULL;
		}

		pSink->OnHostedNetworkStopped();

		_ASSERT(m_CallbackComplete != NULL);

		//
		// Signal callback complete
		//
		SetEvent(m_CallbackComplete);
	}
	*/
}

DWORD
WlanMgr::StringToSsid(__in LPCWSTR strSsid,__out PDOT11_SSID pDot11Ssid)
{
	DWORD dwError = NO_ERROR;
	BYTE  pbSsid[DOT11_SSID_MAX_LENGTH + 1];
	DWORD dwBytes;

	if (strSsid == NULL || pDot11Ssid == NULL || wcslen(strSsid) == 0)
	{
		dwError = ERROR_INVALID_PARAMETER;
		goto error;
	}

	dwBytes = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS,
		strSsid, -1, (LPSTR)pbSsid, sizeof(pbSsid), NULL, NULL);

	if (dwBytes == 0)
	{
		// Conversion failed.
		dwError = GetLastError();

		if (dwError == ERROR_INSUFFICIENT_BUFFER)
		{
			dwError = ERROR_BAD_LENGTH;
		}

		goto error;
	}
	else if (dwBytes == 1)
	{
		// Zero-length SSID.
		dwError = ERROR_BAD_LENGTH;
		goto error;
	}
	else
	{
		// Conversion succeeded and length valid.
		pDot11Ssid->uSSIDLength = dwBytes - 1;
		memcpy(pDot11Ssid->ucSSID, pbSsid, pDot11Ssid->uSSIDLength);
	}

error:

	return dwError;
}

DWORD WlanMgr::ConvertPassPhraseKeyStringToBuffer(
		LPCWSTR strPassKeyString, DWORD dwLength,
		DOT11_AUTH_ALGORITHM dot11Auth, UCHAR * strPassKeyBuf, DWORD * pdwBufLen)
{
	DWORD dwError = NO_ERROR;
	DWORD dwActualLen = dwLength;
	UCHAR *lpstrKeyMaterial = NULL;
	DWORD dwAllocLen = 0;
	DWORD dwKeyBytes = 0;
	BOOL bUnmappableChar = FALSE;
	DWORD dwReqdBytes = 0;
	BOOL bPassPhrase = FALSE;

	if (!strPassKeyString)
	{
		dwError = ERROR_INVALID_PARAMETER;
		//BAIL();
	}

	while ((dwActualLen != 0) && !strPassKeyString[dwActualLen - 1])
	{
		dwActualLen--;
	}

	if (dwActualLen == 0)
	{
		dwError = ERROR_BAD_FORMAT;
		//BAIL_ON_ERROR(dwError);
	}

	dwAllocLen = WideCharToMultiByte(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		strPassKeyString,
		dwLength,
		NULL,
		0,
		NULL,
		NULL
		);
	if (!dwAllocLen)
	{
		dwError = GetLastError();
		//BAIL_ON_ERROR(dwError);
	}

	lpstrKeyMaterial = (UCHAR *)WlanAllocateMemory(dwAllocLen);
	if (NULL == lpstrKeyMaterial)
	{
		dwError = GetLastError();
		//BAIL_ON_ERROR(dwError);
	}

	ZeroMemory(
		lpstrKeyMaterial,
		dwAllocLen
		);

	dwKeyBytes = WideCharToMultiByte(
		CP_ACP,
		WC_NO_BEST_FIT_CHARS,
		strPassKeyString,
		dwLength,
		(LPSTR)lpstrKeyMaterial,
		dwAllocLen,
		NULL,
		&bUnmappableChar
		);

	if (!dwKeyBytes)
	{
		dwError = GetLastError();
		//BAIL_ON_ERROR(dwError);
	}
	if (dwKeyBytes != dwAllocLen)
	{
		dwError = ERROR_INTERNAL_ERROR;
		//BAIL_ON_ERROR(dwError);
	}

	if (bUnmappableChar)
	{
		dwError = ERROR_BAD_FORMAT;
		//BAIL_ON_ERROR(dwError);
	}

	_ASSERT(DOT11_AUTH_ALGO_RSNA_PSK == dot11Auth);
	switch (dot11Auth)
	{
	case DOT11_AUTH_ALGO_RSNA_PSK:
		if ((dwKeyBytes > 63) ||    // Max length
			(dwKeyBytes < 8))       // Min length
		{
			dwError = ERROR_BAD_FORMAT;
			//BAIL_ON_ERROR(dwError);
		}

		// Include an extra byte for the NULL terminator in a passphrase
		dwReqdBytes = dwKeyBytes + 1;
		break;

	default:
		dwError = ERROR_INVALID_PARAMETER;
		//BAIL_ON_ERROR(dwError);
	}

	if (!strPassKeyBuf || (*pdwBufLen < dwReqdBytes))
	{
		dwError = ERROR_MORE_DATA;
		//BAIL_ON_ERROR(dwError);
	}

	ZeroMemory(
		strPassKeyBuf,
		dwReqdBytes
		);

	CopyMemory(
		strPassKeyBuf,
		lpstrKeyMaterial,
		dwKeyBytes
		);
error:
	if (pdwBufLen)
	{
		*pdwBufLen = dwReqdBytes;
	}

	if (lpstrKeyMaterial)
	{
		WlanFreeMemory(lpstrKeyMaterial);
	}

	return dwError;
}