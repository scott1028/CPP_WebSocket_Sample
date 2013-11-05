#include "base64/base64.h"
#include <iostream>
#include "TinySHA1/TinySHA1.hpp"
#include "openssl/sha.h"
#include <string>

using namespace std;
// gcc-win.exe cpp_base64_sha1.cpp base64/base64.cpp -lstdc++ && ./a.exe

int main(int argc, char const *argv[])
{
	// sha1
	const std::string val = "scott";
 	sha1::SHA1 s;
	s.processBytes(val.c_str(), val.size());
	uint32_t digest[5];
	s.getDigest(digest);
	char buffer [48];
	sprintf(buffer,"%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
	string sha1_result(buffer);
	cout << sha1_result << endl;

	int len=21;
	int idx=0;
	unsigned char sha1[len];
	sha1[20]='\0';

	// 將各單位 4 Bytes 的陣列轉換為各單位 1 Bytes的陣列
	cout << endl;
	for(int i=0;i<5;i++){
		// cout << hex << digest[i] << endl;
		// 用 & 跟 >> 計算
		cout << hex << (unsigned)( ( digest[i] & (unsigned)4278190080 )>>24 ) << " ";
		cout << hex << (unsigned)( ( digest[i] & 16711680 )>>16 ) << " ";
		cout << hex << (unsigned)( ( digest[i] & 65280 )>>8 ) << " ";
		cout << hex << (unsigned)( ( digest[i] & 255 )>>0 ) << " ";
		sha1[idx]=(unsigned)( ( digest[i] & 4278190080 )>>24 );
		sha1[++idx]=(unsigned)( ( digest[i] & 16711680 )>>16 );
		sha1[++idx]=(unsigned)( ( digest[i] & 65280 )>>8 );
		sha1[++idx]=(unsigned)( ( digest[i] & 255 )>>0 );
	}
	cout << endl;

	// base64
	// string s2(sha1);
	string encoded = base64_encode(reinterpret_cast<const unsigned char*>( sha1 ), len-1 );
	// // string encoded = base64_encode(reinterpret_cast<const unsigned char*>( sha1 ), 20 );
	cout << "encoded: " << encoded << endl;

	// const string tt(sha1);
	// cout << base64_encode(reinterpret_cast<const unsigned char*>(tt.c_str()), 21 ) << endl;

	return 0;
}