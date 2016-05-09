#pragma once
#include <iostream>
using namespace std;

class ConnProfile {
public:
	ConnProfile(string ssid, string pass);
	ConnProfile(string pass);
	ConnProfile();
	~ConnProfile();
	const string getSSID();
	const string getPassword();
private:
	string ssid;
	string password;
};

class Profile_Error {
public:
	string what() throw();
};