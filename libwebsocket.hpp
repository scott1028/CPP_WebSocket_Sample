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
#define BUFFSIZE 32768

namespace network {
	using namespace std;

	// 定義需要的 struct
	#include "dataFrame_and_mutexFD_definer.hpp"

	// WebSocket 的主類別實作與定義
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
				while(true) {
					int newSocketFD = accept(socketFD,(sockaddr *) &clientAdress, &clientLength);
					if (newSocketFD < 0){
						printf("Error acepting socket");
						continue;
					}

					pthread_t thread_recv,thread_send;
					fd_and_mutex kk;
					kk.newSocketFD=newSocketFD;

					// 產生執行緒, recv/send handle, // (void*)&newSocketFD
					if( 0 != pthread_create(&thread_recv, NULL, Websocket::recvThread, (void*)&kk ) ){	perror("Error creating thread"); };
					if( 0 != pthread_create(&thread_send, NULL, Websocket::sendThread, (void*)&kk ) ){	perror("Error creating thread"); };
				}
				close(socketFD);
			}
		private:
			// 伺服器端口
			int port;

			static void* recvThread(void *fd){
				// pthread_create 必須要是 static 或是 global 才能使用 pthread, 因為 instance method 會先 pass a hidden this pointer
				// 所以裡面就寫通用結構即可

				#include "recvThread.hpp"		// 實作很長寫在這吧, 神奇的寫法, include 就當作純文字輸入吧
			}

			// 要注意如果寄送非 WebSocket 封包給瀏覽器，將造成對方關閉連線這個執行續也會跳錯並將 fb 關閉, 導致另一個 recv Thread 也一併結束, 接著整個程式結束
			static void* sendThread(void *fd){
				// pthread_create 必須要是 static 或是 global 才能使用 pthread, 因為 instance method 會先 pass a hidden this pointer
				// 所以裡面就寫通用結構即可

				#include "sendThread.hpp"		// 實作很長寫在這吧, 神奇的寫法, include 就當作純文字輸入吧
			}
	};
}
