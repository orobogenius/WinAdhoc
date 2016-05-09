#include <iostream>
using namespace std;

class Profile {
public:
	Profile(string user, string pass);
	Profile(string user);
	const string getUsername();
	const string getPassword();
private:
	string username;
	string password;
};

class Profile_Error{
	public :
		string what() throw();
};
