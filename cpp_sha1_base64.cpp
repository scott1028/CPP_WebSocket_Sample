#include "lib/sha1_then_base64.hpp"

using namespace std;
using namespace network;

// gcc-win.exe cpp_base64_sha1.cpp -lstdc++ && ./a.exe

int main(int argc, char const *argv[])
{
	// sha1 then base64
	string val = argv[1];
	cout << sha1_then_base64(val) << endl;

	try{
		cout << endl << "以下為 Python: " << endl;
		system( ( "python python_base64_sha1.py "+string(argv[1]) ).c_str() ) ;
	}
	catch(const char* message){}

	return 0;
}
