#pragma once
#include "stdafx.h"

#pragma comment(lib, "wlanapi.lib")

class WlanStation
{
private:
	DOT11_MAC_ADDRESS m_MacAddress;

	// Don't allow to create an empty CWlanStation object
	WlanStation() {};

public:

	WlanStation(const WlanStation&);
	WlanStation(const WLAN_HOSTED_NETWORK_PEER_STATE&);
	~WlanStation();

	BOOL operator==(const WlanStation&);
	BOOL operator==(const DOT11_MAC_ADDRESS);

	VOID GetMacAddress(DOT11_MAC_ADDRESS&) const;
};

class WlanMgr
{
private:
	HANDLE wlanHandle;
	bool initialized;
	bool allowed;
	DWORD version;
	WLAN_HOSTED_NETWORK_CONNECTION_SETTINGS connSettings;
	WLAN_HOSTED_NETWORK_SECURITY_SETTINGS secSettings;
	WLAN_HOSTED_NETWORK_STATUS netStatus;
	WLAN_HOSTED_NETWORK_STATE netState;
public:
	WlanMgr();

	~WlanMgr();

	HRESULT Init();

	HRESULT SetHostedNetworkName(CString&);

	HRESULT GetHostedNetworkName(LPCWSTR&);

	HRESULT SetHostedNetworkKey(CString&);

	HRESULT GetHostedNetworkKey(LPCWSTR&);
	
	HRESULT StartHostedNetwork();

	HRESULT ForceStopHostedNetwork();

	HRESULT ForceStartHostedNetwork();

	void OnHostedNetworkStarted();

	HRESULT StopHostedNetwork();

	void OnHostedNetworkStopped();

	//HRESULT IsHostedNetworkStarted(bool &);

	DWORD StringToSsid(__in LPCWSTR strSsid, __out PDOT11_SSID pDot11Ssid);

	DWORD ConvertPassPhraseKeyStringToBuffer(
			__in_ecount(dwLength) LPCWSTR strPassKeyString,     // Unicode string 
			__in DWORD dwLength,
			__in DOT11_AUTH_ALGORITHM dot11Auth,
			__out_ecount_opt(*pdwBufLen) UCHAR* strPassKeyBuf,  // NULL to get length required
			__inout DWORD *pdwBufLen                            // in: length of buffer; out: chars copied/required
			);
};