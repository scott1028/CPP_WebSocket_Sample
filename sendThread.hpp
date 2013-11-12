fd_and_mutex fdm=*( (fd_and_mutex*)fd );
int socketFileDescriptor=fdm.newSocketFD;

string res("123");

while(true){
	pthread_mutex_lock( &(fdm.mutex) );
	
	send(socketFileDescriptor,res.c_str(),res.size(),0);

	pthread_mutex_unlock( &(fdm.mutex) );
	sleep(1);
	cout << "send a msg" << endl;
}