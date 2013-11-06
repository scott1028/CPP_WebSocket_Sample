#include "lib/sha1_then_base64.hpp"

using namespace std;
using namespace network;

// #include "lib/sha1_then_base64.hpp"
// gcc-win.exe test_sha1_base64.cpp -lstdc++ && ./a.exe test_string
// 		or
// #include <lib/sha1_then_base64.hpp>
// gcc-win.exe test_sha1_base64.cpp -lstdc++ -l./ && ./a.exe test_string

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
