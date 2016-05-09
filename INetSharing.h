#pragma once
//Base class for the INetSharing interface
#include <NetCon.h>
#include <vector>
using namespace std;

class INetSharing
{
public:
	INetSharing();
	~INetSharing(); //Kill COM interface
	HRESULT CreateCOM();
	vector<string> GetConnections();
	vector<string> GetPrivateConnections();
	vector<string> GetPublicConnections();
	HRESULT GetConConfig();
	HRESULT GetSharingEnabled();
	HRESULT GetSharingInstalled();
	HRESULT GetSSIDOfConn();
	HRESULT EnableICS();
	HRESULT DisableICS();

private:
	INetSharingManager *iNetManager;
	INetConnection **connection;
	INetSharingConfiguration *iNetConfig;
	vector<string> connectionsList;
	vector<string> privateConnectionsList;
	vector<string> publicConnectionsList;
	INetSharingEveryConnectionCollection **LPConnections;
	INetSharingPrivateConnectionCollection **LPPrivate;
	INetSharingPublicConnectionCollection **LPPublic;
	bool initialized;
};