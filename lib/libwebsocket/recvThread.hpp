// 轉換
fd_and_mutex fdm=*( (fd_and_mutex*)fd );
int socketFileDescriptor=fdm.newSocketFD;
// int socketFileDescriptor=*((int*)fd);

char buffer[BUFFSIZE];					// 第一圈等於 header 第二圈等於 body
bzero(buffer, sizeof(buffer));

int received = -1;

bool websocket_handle=true;

int offset=7;

wsh twsh;

string header("");			// 數據 header
string body("");			// 數據累積 body

while(true){
	// mutex_lock
	// pthread_mutex_lock( &(fdm.mutex) );

	if (received > 0) {

		cout << "recv bytes size is: " << hexValue received << endl;

		if(websocket_handle){
			// WebSocket 通信協定實作
			header+=buffer;
			cout << header << endl;
			cout << header.size() << endl;

			size_t s=header.find("Sec-WebSocket-Key: ")+19;
			size_t e=header.find("\n",s)-1;
			string key( header.substr(s,e-s)+"258EAFA5-E914-47DA-95CA-C5AB0DC85B11" );
			cout << key << endl;
			cout << sha1_then_base64(key) << endl;

			string res(
				"HTTP/1.1 101 Switching Protocols\r\n"
				"Upgrade: websocket\r\n"
				"Connection: Upgrade\r\n"
				"Sec-WebSocket-Accept: "+sha1_then_base64(key)+"\r\n"
				"Sec-WebSocket-Origin: null\r\n\r\n"
			);

			cout << res << endl;

			send(socketFileDescriptor,res.c_str(),res.size(),0);

			body.clear();

			websocket_handle=false;
		}
		else if(websocket_handle==false){
			// WebSocket 連線成功這邊開始解讀 Data Frame

			// 數據累積
			body+=buffer;

			offset=7;	// set offset, 要注意 pow() 內部要代 double 型態, 不然會拋錯( 參考：http://www.cplusplus.com/reference/cmath/pow/ )
			cout << "fin:  " << ( twsh.fin=( (body.c_str()[0] & hexValue pow(2,(double)offset))>>offset ) ) << endl;offset-=1; //  fin, 還需要判斷是不是最後一個包
			cout << "rsv1: " << ( twsh.rsv1=( (body.c_str()[0] & hexValue pow(2,(double)offset))>>offset ) ) << endl;offset-=1; // rsv1
			cout << "rsv2: " << ( twsh.rsv2=( (body.c_str()[0] & hexValue pow(2,(double)offset))>>offset ) ) << endl;offset-=1; // rsv2
			cout << "rsv3: " << ( twsh.rsv3=( (body.c_str()[0] & hexValue pow(2,(double)offset))>>offset ) ) << endl;offset-=1; // rsv3
			cout << "opcode: " << ( twsh.opcode=( (body.c_str()[0] & 15 ) ) ) << endl;offset-=1; // opcode

			offset=7;
			cout << "mask: " << ( twsh.mask=(body.c_str()[1] & (unsigned int)pow(2,(double)offset))>>offset ) << endl;offset-=1; // mask
			twsh.payload_len.asNumber=(unsigned int)(body.c_str()[1] & 127); // payload len

			// 小型資料量調用
			if(twsh.payload_len.asNumber<=125){
				cout << "payload len: <= 125: " << twsh.payload_len.asNumber << endl;
				// data store
				if(twsh.mask==1){
					unsigned char *mask_key=new unsigned char[4];
					unsigned char *data=new unsigned char[twsh.payload_len.asNumber];
					unsigned char *data_converted=new unsigned char[twsh.payload_len.asNumber+1];
					data_converted[twsh.payload_len.asNumber]='\0'; 	// 記得再補上一個 nil 當作結束(不然很容易領便當)

					int offset=2;
					int idx=0;
					int mask_key_offset=0;

					// 重新計算長度(here)
					twsh.cal_payload_len(offset,idx,mask_key_offset,const_cast<char*>( body.c_str() ));

					// 解析資料
					twsh.data_parser(data,const_cast<char*>( body.c_str() ),offset,idx,mask_key_offset,data_converted);

					body.clear();
				};
			}
			// 中等資料量調用(會有問題) 65536<=
			else if(twsh.payload_len.asNumber==126){
				cout << "payload len: == 126: " << twsh.payload_len.asNumber << endl;

				if(twsh.mask==1){
					cout << "recal_payload_len: " << twsh.payload_len.asNumber << endl;

					int offset=2;
					int idx=0;
					int mask_key_offset=2;

					// 先重新計算Payload Size, 要注意Intel是小端序排列, 下兩個 Bytes
					twsh.cal_payload_len(offset,idx,mask_key_offset,const_cast<char*>( body.c_str() ));

					// 當資料準備完成後再 Parser, 計算 Payload length vs buffer
					unsigned char *mask_key=new unsigned char[4];
					unsigned char *data=new unsigned char[twsh.payload_len.asNumber];
					unsigned char *data_converted=new unsigned char[twsh.payload_len.asNumber+1];
					data_converted[twsh.payload_len.asNumber]='\0'; 	// 記得再補上一個 nil 當作結束(不然很容易領便當)

					// 解析資料
					twsh.data_parser(data,const_cast<char*>( body.c_str() ),offset,idx,mask_key_offset,data_converted);

					body.clear();
				};
			}
			// 當資料非常大的時候調用(有問題) >65536
			else if(twsh.payload_len.asNumber==127){
				cout << "payload len: == 127: " << twsh.payload_len.asNumber << endl;

				if(twsh.mask==1){
					cout << "recal_payload_len: " << twsh.payload_len.asNumber << endl;

					int offset=2;
					int idx=0;
					int mask_key_offset=8;

					// 先重新計算Payload Size, 要注意Intel是小端序排列, 下兩個 Bytes
					twsh.cal_payload_len(offset,idx,mask_key_offset,buffer);//const_cast<char*>( body.c_str() ));

					cout << twsh.payload_len.asNumber << endl;

					// 當資料準備完成後再 Parser, 計算 Payload length vs buffer
					unsigned char *mask_key=new unsigned char[4];
					unsigned char *data=new unsigned char[twsh.payload_len.asNumber];
					unsigned char *data_converted=new unsigned char[twsh.payload_len.asNumber+1];
					data_converted[twsh.payload_len.asNumber]='\0'; 	// 記得再補上一個 nil 當作結束(不然很容易領便當)

					// 解析資料
					twsh.data_parser(data,const_cast<char*>( body.c_str() ),offset,idx,mask_key_offset,data_converted);

					body.clear();
				};
			}
		};
	}
	else{
		// system("sleep 0.01");		// 避免系統爆掉
	}

	// 清空 Buffer
	bzero(buffer, sizeof(buffer));
	received = recv(socketFileDescriptor, buffer, BUFFSIZE, 0);

	// mutex unlock
	// pthread_mutex_unlock( &(fdm.mutex) );
}

pthread_exit(NULL);	// 退出thread
