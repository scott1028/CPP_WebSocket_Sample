// 定義一個 WebSocket Header, struct 別名, 副程式
union payload_len_converter { unsigned long long int asNumber;unsigned char asBytes[8]; };
typedef struct webSocketHeader {
	unsigned int fin;
	unsigned int rsv1;
	unsigned int rsv2;
	unsigned int rsv3;
	unsigned int opcode;
	unsigned int mask;
	payload_len_converter payload_len;
	unsigned char mask_key[4];
	void cal_payload_len(int offset,int idx,int mask_key_offset,char *buffer){
		// 126 & 127 適用
		if(payload_len.asNumber>125){
			// 再度初始化
			bzero(payload_len.asBytes,sizeof(payload_len.asBytes));
			// 先重新計算Payload Size, 要注意Intel是小端序排列, 下兩個 Bytes
			for(int i=idx;i<mask_key_offset;i++){
				payload_len.asBytes[mask_key_offset-1-i]=hexValue buffer[offset+i];;
			}
		}
		else{
			// 不需要計算
		}
	};
	void data_parser(unsigned char *data,char *buffer,int offset,int idx,unsigned int mask_key_offset,unsigned char *data_converted){

		// get mask-key
		mask_key[idx]=buffer[offset+idx+mask_key_offset];idx++;
		mask_key[idx]=buffer[offset+idx+mask_key_offset];idx++;
		mask_key[idx]=buffer[offset+idx+mask_key_offset];idx++;
		mask_key[idx]=buffer[offset+idx+mask_key_offset];idx++;

		for(int i=0;i<payload_len.asNumber;i++){
			int j=i % 4;
			data[i]=buffer[offset+i+idx+mask_key_offset];
			// 根據 data frame 解析方式實作
			data_converted[i]=data[i]^mask_key[j];
		};

		cout << "資料長度: " << payload_len.asNumber+1 << endl;
		cout << "解析成功: " << data_converted << endl;
	};
	
} wsh;

// FD 與 Mutex 的 Struct
struct fd_and_mutex{
	pthread_mutex_t mutex;
	int newSocketFD;
};