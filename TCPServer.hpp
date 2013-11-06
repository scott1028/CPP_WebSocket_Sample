//
// Created by Igor Mel. on 8/17/13.
// Copyright (c) 2013 ___FULLUSERNAME___. All rights reserved.
//
// To change the template use AppCode | Preferences | File Templates.
//



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

namespace network {
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

	const int BUFFSIZE = 1024;
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

		string header("");
		while(true){
			if (received > 0) {
				header+=buffer;

				cout << header << endl;
				cout << header.size() << endl;

				if(websocket_handle){
					// WebSocket 通信協定實作

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
						"Sec-WebSocket-Origin: null\r\n"
						"Sec-WebSocket-Location: ws://127.0.0.1:1239/\r\n\r\n"
					);

					cout << res << endl;

					send(socketFileDescriptor,res.c_str(),res.size(),0);

					websocket_handle=false;
				}
				else if(websocket_handle==false){
					// WebSocket 連線成功這邊開始解讀 Data Frame

				};

				bzero(buffer, sizeof(buffer));
			}
			else{
				// system("sleep 0.01");		// 避免系統爆掉
			}
			received = recv(socketFileDescriptor, buffer, BUFFSIZE, 0);
		}

		// 	if (received >= 0) {
		// 		if(0 > (received = recv(socketFileDescriptor, buffer, BUFFSIZE, 0))){
		// 			//printf("Error getting info from FD");
		// 			//close(socketFileDescriptor);
		// 			//return ;
		// 		}
		// 		if(strcmp(buffer,"BYE")==0){
		// 			strcpy(msg,"BYE_BYE");
		// 			send(socketFileDescriptor,msg,BUFFSIZE,0);
		// 			//return ;
		// 		}
		// 		std::cout<<buffer;
		// 		bzero(buffer, sizeof(buffer));
		// 		if (send(socketFileDescriptor, msg, BUFFSIZE, 0)<0){
		// 			printf("Error writing to socket");
		// 			//close(socketFileDescriptor);
		// 			//return ;
		// 		}
		// 	}
		// }
		// close(socketFileDescriptor);
		
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
