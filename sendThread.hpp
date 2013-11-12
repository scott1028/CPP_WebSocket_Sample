fd_and_mutex fdm=*( (fd_and_mutex*)fd );
int socketFileDescriptor=fdm.newSocketFD;

while(true){
	// char msg[]={ 0xe6, 0x88, 0x91, 0xe6, 0xb8, 0xac, 0xe8, 0xa9, 0xa6 };
	// 我測試 (UTF-8)

	string msg("我測試再試試!吧");

	cout << msg << endl;

	char msg2[msg.size()+2];
	msg2[0]=0x81;
	msg2[1]=msg.size();
	for(int i=2;i<msg.size()+2;i++) msg2[i]=msg[i-2];

	pthread_mutex_lock( &(fdm.mutex) );
	send(socketFileDescriptor,msg2,sizeof(msg2),0);
	pthread_mutex_unlock( &(fdm.mutex) );
	
	sleep(1);
}
