#include "Profile.h"
using namespace std;

std::string Profile_Error::what() throw() {
	return "Password must be at least 8 characters";
}

Profile::Profile(string username, string password) {
	if (password.length()<8) throw Profile_Error();
	this->username = username;
	this->password = password;
}

Profile::Profile(string user){
	 this->username = user;
}

const string Profile::getUsername() {
	return username;
}
const string Profile::getPassword(){
	return password;
}
