/*
	base C++ WebSocket Server for GCC 3.3+
	compiler: gcc websocket_server.cpp -lstdc++ -lpthread -Wall
	Author: Scott
*/

#include <iostream>
#include "lib/libwebsocket.hpp"

using namespace std;
using namespace network;

int main(int argc, const char * argv[])
{
	int portNo=8888;
	Websocket app=Websocket(portNo);
	app.run();
	return 0;
}
