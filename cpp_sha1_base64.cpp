#include "base64/base64.h"
#include <iostream>
#include "TinySHA1/TinySHA1.hpp"
#include <string>
#include <vector>
#include <iomanip>

using namespace std; 

// gcc-win.exe cpp_base64_sha1.cpp base64/base64.cpp -lstdc++ && ./a.exe

int main(int argc, char const *argv[])
{
	// sha1
	const string val = "scott";
	sha1::SHA1 s;
	s.processBytes(val.c_str(), val.size());
	uint32_t digest[5];
	s.getDigest(digest);
	char buffer [48];
	sprintf(buffer,"%08x%08x%08x%08x%08x", digest[0], digest[1], digest[2], digest[3], digest[4]);
	string sha1_result(buffer);

	int len=20;
	int idx=0;
	unsigned char sha1[len];

	// 將各單位 4 Bytes 的陣列轉換為各單位 1 Bytes的陣列
	// 4278190080 數字很大所以要加 UL, 不然會有 warning
	cout << endl;
	for(int i=0;i<5;i++){
		// 用 & 跟 >> 計算
		cout << setfill('0') << setw(2) << hex << (unsigned)( ( digest[i] & 4278190080UL )>>24 ) << " ";
		cout << setfill('0') << setw(2) << hex << (unsigned)( ( digest[i] & 16711680 )>>16 ) << " ";
		cout << setfill('0') << setw(2) << hex << (unsigned)( ( digest[i] & 65280 )>>8 ) << " ";
		cout << setfill('0') << setw(2) << hex << (unsigned)( ( digest[i] & 255 )>>0 ) << " ";
		sha1[idx]=(unsigned)( ( digest[i] & 4278190080UL )>>24 );//cout << idx;
		sha1[++idx]=(unsigned)( ( digest[i] & 16711680 )>>16 );//cout << idx;
		sha1[++idx]=(unsigned)( ( digest[i] & 65280 )>>8 );//cout << idx;
		sha1[++idx]=(unsigned)( ( digest[i] & 255 )>>0 );//cout << idx;
		idx++;
	}
	cout << endl;
	cout << endl << "以下為 C++: " << endl;	

	// base64
	string encoded = base64_encode(reinterpret_cast<const unsigned char*>( sha1 ), len );
	cout << "原先('scott' sha1 then base64): " << encoded << endl;

	cout << endl << "以下為 Python: " << endl;
	system("python python_base64_sha1.py");

	return 0;
}
