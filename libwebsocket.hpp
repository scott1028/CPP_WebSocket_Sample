#define hexValue (unsigned int)(unsigned char)
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
#define BUFFSIZE 65536

namespace network {
	using namespace std;

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

	// fd_and_mutex
	struct fd_and_mutex{
		pthread_mutex_t mutex;
		int newSocketFD;
	};

	class Websocket{
		public:
			Websocket(int port){
				this->port=port;
				cout << "Open Socket Port: " << this->port << endl;
			}

			// 啟動 Socket Server
			void run(){
				int socketFD;
				socklen_t clientLength;
				sockaddr_in serverAddress,clientAdress;
				if((socketFD= socket(AF_INET, SOCK_STREAM, 0))<0){ perror("Error opening socket"); }

				bzero((char *)&serverAddress, sizeof(serverAddress));
				serverAddress.sin_family = AF_INET;
				serverAddress.sin_addr.s_addr = INADDR_ANY;
				serverAddress.sin_port = htons(this->port);

				if (bind(socketFD, (struct sockaddr *) &serverAddress,sizeof(serverAddress)) < 0){
					perror("ERROR on binding");
				}

				listen(socketFD,5);
				clientLength = sizeof(clientAdress);
				while (1) {
					int newSocketFD = accept(socketFD,(sockaddr *) &clientAdress, &clientLength);
					if (newSocketFD < 0){
						printf("Error acepting socket");
						continue;
					}

					pthread_t thread_recv,thread_send;
					fd_and_mutex kk;
					kk.newSocketFD=newSocketFD;

					// 產生執行續, recv/send handle, // (void*)&newSocketFD
					if( 0 != pthread_create(&thread_recv, NULL, Websocket::recvThread, (void*)&kk ) ){	perror("Error creating thread"); };
					if( 0 != pthread_create(&thread_send, NULL, Websocket::sendThread, (void*)&kk ) ){	perror("Error creating thread"); };

					// Websocket::createThread( (void*)&newSocketFD );

					// this->createThread( (void*)&newSocketFD );

					// this->createThread( (void*)newSocketFD );

					// // sender handler
					// if( 0 != pthread_create(&thread, NULL, TCPServer::threadFunc, NULL) ){
					// 	perror("Error creating thread");
					// };
				}
				close(socketFD);
			}
		private:
			int port;
			static void* recvThread(void *fd){
				// pthread_create 必須要是 static 或是 global 才能使用 pthread, 因為 instance method 會先 pass a hidden this pointer
				// 所以裡面就寫通用結構即可

				#include "recvThread.hpp"		// 實作很長寫在這吧, 神奇的寫法, include 就當作純文字輸入吧
			}

			static void* sendThread(void *fd){
				// pthread_create 必須要是 static 或是 global 才能使用 pthread, 因為 instance method 會先 pass a hidden this pointer
				// 所以裡面就寫通用結構即可

				#include "sendThread.hpp"		// 實作很長寫在這吧, 神奇的寫法, include 就當作純文字輸入吧
			}
	};
}
