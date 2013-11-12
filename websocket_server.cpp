//
//  main.cpp
//  server
//
//  Created by Igor Mel. on 8/17/13.
//  Copyright (c) 2013 Igor Mel.. All rights reserved.
//

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
