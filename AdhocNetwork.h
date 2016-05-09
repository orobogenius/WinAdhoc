#pragma once
#include <adhoc.h>

class AdhocNetwork
{
public:
	AdhocNetwork();
	~AdhocNetwork();
	void SetAdHocNetwork();
	void SetAdHocInterface();
	void CreateAdHocNetwork();
	void CommitAdHocNetwork();
	void GetWirelessInterfaces();
private:
	void SetSecuritySettings();
	IDot11AdHocNetwork **pNetwork;
	IDot11AdHocInterface *pInterface;
	IDot11AdHocInterface** pInterfaces = new IDot11AdHocInterface*[5];
	IDot11AdHocSecuritySettings *pSecurity;
	Dot11AdHocManager *adHocManager;
	IEnumDot11AdHocInterfaces *adHocInterfaces;
};