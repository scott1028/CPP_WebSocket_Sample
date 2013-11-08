//
//  main.cpp
//  server
//
//  Created by Igor Mel. on 8/17/13.
//  Copyright (c) 2013 Igor Mel.. All rights reserved.
//

#include <iostream>
#include "libwebsocket.hpp"

using namespace std;
using namespace network;

int main(int argc, const char * argv[])
{
	int portNo;
	portNo = 8888;
	TCPServer server(portNo);
	server.run();
	cout<<"Test";
	return 0;
}
