#include "stdafx.h"
#include "ConnProfile.h"

std::string Profile_Error::what() throw() {
	return "Password must be at least 8 characters";
}

ConnProfile::ConnProfile(string username, string password) {
	if (password.length()<8) throw Profile_Error();
	this->ssid = username;
	this->password = password;
}

ConnProfile::ConnProfile(string pass)
{
	ssid = "GEN_NETWORK";
	password = pass;
}

ConnProfile::~ConnProfile()
{

}

ConnProfile defaultProfile() {
	ConnProfile profile("GEN_NETWORK", "87654321");
	return profile;
}

ConnProfile::ConnProfile() :
	ssid(defaultProfile().ssid),
	password(defaultProfile().password)
{
}
const string ConnProfile::getSSID() {
	return ssid;
}
const string ConnProfile::getPassword() {
	return password;
}