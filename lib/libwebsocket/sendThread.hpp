// 轉換
fd_and_mutex fdm=*( (fd_and_mutex*)fd );
int socketFileDescriptor=fdm.newSocketFD;
// int socketFileDescriptor=*((int*)fd);

while(true){
	string msg("我測試再試試!吧");

	cout << msg << endl;

	char msg2[msg.size()+2];
	msg2[0]=0x81;
	msg2[1]=msg.size();
	for(int i=2;i<msg.size()+2;i++) msg2[i]=msg[i-2];

	// pthread_mutex_lock( &(fdm.mutex) );
	send(socketFileDescriptor,msg2,sizeof(msg2),0);
	// pthread_mutex_unlock( &(fdm.mutex) );

	sleep(1);
}

pthread_exit(NULL);	// 退出thread
