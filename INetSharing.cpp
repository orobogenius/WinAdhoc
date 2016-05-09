#include "stdafx.h"
#include "INetSharing.h"

#define INIT_ERROR 104

INetSharing::INetSharing()
{
	INetSharingManager *iNetManager = NULL;
	INetConnection **connection = NULL;
	INetSharingConfiguration *iNetConfig = NULL;
	INetSharingEveryConnectionCollection **LPConnections = NULL;
	INetSharingPrivateConnectionCollection **LPPrivate = NULL;
	INetSharingPublicConnectionCollection **LPPublic = NULL;

	HRESULT hr;
	hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr))
	{
		initialized = true;
	}
}

INetSharing::~INetSharing() {}

HRESULT INetSharing::CreateCOM()
{
	HRESULT hr = S_OK;
	if (!initialized)
	{
		return hr = INIT_ERROR;
	}

	hr = CoCreateInstance(CLSID_NetSharingManager, NULL, CLSCTX_ALL, IID_INetSharingManager, reinterpret_cast<void**>(&iNetManager));

	return hr;
}

vector<string> INetSharing::GetConnections()
{
	return connectionsList;
}

vector<string> INetSharing::GetPrivateConnections()
{
	return privateConnectionsList;
}

vector<string> INetSharing::GetPublicConnections()
{
	return publicConnectionsList;
}

HRESULT INetSharing::GetConConfig()
{

}

HRESULT INetSharing::GetSharingEnabled()
{
}

HRESULT INetSharing::GetSharingInstalled()
{
	HRESULT hr = S_OK;
	
	VARIANT_BOOL pbInstalled;
	iNetManager->get_SharingInstalled(&pbInstalled);

	if (!pbInstalled)
	{
		return hr = E_FAIL;
	}

	return hr;
}

HRESULT INetSharing::GetSSIDOfConn()
{
}

HRESULT INetSharing::EnableICS()
{
}

HRESULT INetSharing::DisableICS()
{
}