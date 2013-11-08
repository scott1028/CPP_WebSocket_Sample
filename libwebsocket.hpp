//
// Created by Igor Mel. on 8/17/13.
// Copyright (c) 2013 ___FULLUSERNAME___. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//

#define hexValue (unsigned int)(unsigned char)

#ifndef __TCPServer_H_
#define __TCPServer_H_

#include "lib/sha1_then_base64.hpp"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <math.h>		// 指數函數

namespace network {

	// 定義一個 WebSocket Header
	union payload_len_converter { unsigned long long int asNumber;unsigned char asBytes[8]; };
	typedef struct webSocketHeader {
		unsigned int fin;
		unsigned int rsv1;
		unsigned int rsv2;
		unsigned int rsv3;
		unsigned int opcode;
		unsigned int mask;
		// unsigned long long int payload_len;
		payload_len_converter payload_len;
		unsigned char mask_key[4];
		
	} wsh;

	using namespace std;

	class TCPServer{
		public:
			TCPServer(u_short portNumb);
			void run();
		private:
			u_short portNumber;
			static void* threadFunc(void *argc);
			static void clientTaker(int socketFileDescriptor);// root of the WebServer used to deal with client connections
			static int currentSocketDescriptor;
	};

	const int BUFFSIZE = 65536;
	int TCPServer::currentSocketDescriptor;

	// 實作對話內的調用方法
	std::string answer(){
		std::string str;
		str = "HTTP/1.1 200 OK\r\n"
				"Server: YarServer/2009-09-09\r\n"
				"Content-Type: text/html\r\n"
				"Content-Length: 100 \r\n"
				"Connection: close\r\n\r\n";
		std::string result=str+="<html><body><h1>Hello from Habrahabr</h1></body></html>";
		return result;

	}

	// 實作對話
	void TCPServer::clientTaker(int socketFileDescriptor){
		char buffer[BUFFSIZE];
		int received = -1;
		char msg[BUFFSIZE];
		strcpy(msg, answer().c_str());

		bool websocket_handle=true;

		int offset=7;

		wsh twsh;

		string header("");
		while(true){
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

					websocket_handle=false;
				}
				else if(websocket_handle==false){
					// WebSocket 連線成功這邊開始解讀 Data Frame

					offset=7;	// set offset
					cout << "fin:  " << ( twsh.fin=( (buffer[0] & hexValue pow(2,offset))>>offset ) ) << endl;offset-=1; //  fin, 還需要判斷是不是最後一個包
					cout << "rsv1: " << ( twsh.rsv1=( (buffer[0] & hexValue pow(2,offset))>>offset ) ) << endl;offset-=1; // rsv1
					cout << "rsv2: " << ( twsh.rsv2=( (buffer[0] & hexValue pow(2,offset))>>offset ) ) << endl;offset-=1; // rsv2
					cout << "rsv3: " << ( twsh.rsv3=( (buffer[0] & hexValue pow(2,offset))>>offset ) ) << endl;offset-=1; // rsv3
					cout << "Opcode: " << ( twsh.opcode=( (buffer[0] & 15 ) ) ) << endl;offset-=1; // opcode

					offset=7;
					cout << "mask: " << ( twsh.mask=(buffer[1] & (unsigned int)pow(2,offset))>>offset ) << endl;offset-=1; // mask
					twsh.payload_len.asNumber=(unsigned int)(buffer[1] & 127); // payload len

					// 
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
							mask_key[idx]=buffer[offset+idx];idx++;
							mask_key[idx]=buffer[offset+idx];idx++;
							mask_key[idx]=buffer[offset+idx];idx++;
							mask_key[idx]=buffer[offset+idx];idx++;

							for(int i=0;i<twsh.payload_len.asNumber;i++){
								int j=i % 4;
								data[i]=buffer[offset+i+idx];
								// 根據 data frame 解析方式實作
								data_converted[i]=data[i]^mask_key[j];
							};

							// converted data
							cout << "資料長度: " << twsh.payload_len.asNumber+1 << endl;
							cout << "解析成功: " << data_converted << endl;
						};
					}
					// 
					else if(twsh.payload_len.asNumber==126){
						cout << "payload len: == 126: " << twsh.payload_len.asNumber << endl;

						// 避免初始值導致錯誤
						bzero(twsh.payload_len.asBytes,sizeof(twsh.payload_len.asBytes));

						int offset=2;
						int idx=0;

						// 先重新計算Payload Size, 要注意Intel是小端序排列, 下兩個 Bytes
						for(int i=idx;i<2;i++){
							twsh.payload_len.asBytes[1-i]=hexValue buffer[offset+i];;
						}

						unsigned char *mask_key=new unsigned char[4];
						unsigned char *data=new unsigned char[twsh.payload_len.asNumber];
						unsigned char *data_converted=new unsigned char[twsh.payload_len.asNumber+1];
						data_converted[twsh.payload_len.asNumber]='\0'; 	// 記得再補上一個 nil 當作結束(不然很容易領便當)

						// get mask-key
						mask_key[idx]=buffer[offset+idx+2];idx++;
						mask_key[idx]=buffer[offset+idx+2];idx++;
						mask_key[idx]=buffer[offset+idx+2];idx++;
						mask_key[idx]=buffer[offset+idx+2];idx++;

						for(int i=0;i<twsh.payload_len.asNumber;i++){
							int j=i % 4;
							data[i]=buffer[offset+i+idx+2];
							// 根據 data frame 解析方式實作
							data_converted[i]=data[i]^mask_key[j];
						};

						// converted data
						cout << "資料長度: " << twsh.payload_len.asNumber+1 << endl;
						cout << "解析成功: " << data_converted << endl;
					}
					// 
					else if(twsh.payload_len.asNumber==127){
						cout << "payload len: == 127: " << twsh.payload_len.asNumber << endl;

						// 避免初始值導致錯誤
						bzero(twsh.payload_len.asBytes,sizeof(twsh.payload_len.asBytes));

						int offset=2;
						int idx=0;

						// 先重新計算Payload Size, 要注意Intel是小端序排列, 下兩個 Bytes
						for(int i=idx;i<8;i++){
							twsh.payload_len.asBytes[7-i]=hexValue buffer[offset+i];;
						}

						cout << "recal_payload_len: " << twsh.payload_len.asNumber << endl;

						unsigned char *mask_key=new unsigned char[4];
						unsigned char *data=new unsigned char[twsh.payload_len.asNumber];
						unsigned char *data_converted=new unsigned char[twsh.payload_len.asNumber+1];
						data_converted[twsh.payload_len.asNumber]='\0'; 	// 記得再補上一個 nil 當作結束(不然很容易領便當)

						// get mask-key
						mask_key[idx]=buffer[offset+idx+8];idx++;
						mask_key[idx]=buffer[offset+idx+8];idx++;
						mask_key[idx]=buffer[offset+idx+8];idx++;
						mask_key[idx]=buffer[offset+idx+8];idx++;

						for(int i=0;i<twsh.payload_len.asNumber;i++){
							int j=i % 4;
							data[i]=buffer[offset+i+idx+8];
							// 根據 data frame 解析方式實作
							data_converted[i]=data[i]^mask_key[j];
						};

						// converted data
						cout << "資料長度: " << twsh.payload_len.asNumber+1 << endl;
						cout << "解析成功: " << data_converted << endl;
					}
				};
			}
			else{
				// system("sleep 0.01");		// 避免系統爆掉
			}

			bzero(buffer, sizeof(buffer));
			received = recv(socketFileDescriptor, buffer, BUFFSIZE, 0);
		}
	}
	void* TCPServer::threadFunc(void *argc){
		clientTaker(TCPServer::currentSocketDescriptor);
	}

	TCPServer::TCPServer(u_short portNumb) {
		  this->portNumber=portNumb;
	}

	void TCPServer::run(){
		int socketFD;
		socklen_t clientLength;
		sockaddr_in serverAddress,clientAdress;
		if((socketFD= socket(AF_INET, SOCK_STREAM, 0))<0){ perror("Error opening socket"); }

		bzero((char *)&serverAddress, sizeof(serverAddress));
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = INADDR_ANY;
		serverAddress.sin_port = htons(portNumber);

		if (bind(socketFD, (struct sockaddr *) &serverAddress,sizeof(serverAddress)) < 0){
			perror("ERROR on binding");
		}

		listen(socketFD,5);
		clientLength = sizeof(clientAdress);
		while (1) {
			int newsocketFD = accept(socketFD,(sockaddr *) &clientAdress, &clientLength);
			if (newsocketFD < 0){
				printf("Error acepting socket");
				continue;
			}

			pthread_t thread;

			TCPServer::currentSocketDescriptor=newsocketFD;

			if( 0 != pthread_create(&thread, NULL, TCPServer::threadFunc, NULL) ){
				perror("Error creating thread");
			};
		}

		close(socketFD);
		return ;
	}
}

#endif //__TCPServer_H_
