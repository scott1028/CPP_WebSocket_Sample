#include "sha1_then_base64/base64.hpp"
#include "sha1_then_base64/TinySHA1.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

namespace network {
	using namespace std;

	string sha1_then_base64(string val){
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
		for(int i=0;i<5;i++){
			// 用 & 跟 >> 計算
			// cout << setfill('0') << setw(2) << hex << (unsigned)( ( digest[i] & 4278190080UL )>>24 ) << " ";
			// cout << setfill('0') << setw(2) << hex << (unsigned)( ( digest[i] & 16711680 )>>16 ) << " ";
			// cout << setfill('0') << setw(2) << hex << (unsigned)( ( digest[i] & 65280 )>>8 ) << " ";
			// cout << setfill('0') << setw(2) << hex << (unsigned)( ( digest[i] & 255 )>>0 ) << " ";
			sha1[idx]=(unsigned)( ( digest[i] & 4278190080UL )>>24 );//cout << idx;
			sha1[++idx]=(unsigned)( ( digest[i] & 16711680 )>>16 );//cout << idx;
			sha1[++idx]=(unsigned)( ( digest[i] & 65280 )>>8 );//cout << idx;
			sha1[++idx]=(unsigned)( ( digest[i] & 255 )>>0 );//cout << idx;
			idx++;
		}

		// base64
		string encoded = base64_encode(reinterpret_cast<const unsigned char*>( sha1 ), len );
		return encoded;
	}
}
