fd_and_mutex fdm=*( (fd_and_mutex*)fd );
int socketFileDescriptor=fdm.newSocketFD;

while(true){
	pthread_mutex_lock( &(fdm.mutex) );

	unsigned char str[11];

	// 0xe6 0x88 0x91 0xe6 0xb8 0xac 0xe8 0xa9 0xa6 
	// 我測試 (UTF-8)

	str[0]=0x81;	// fin,rsv1,rsv2,rsv3,opcode
	str[1]=0x09;	// mask,payload-length, 代表後面 9 個 Bytes 是資料
	str[2]=0xe6;
	str[3]=0x88;
	str[4]=0x91;
	str[5]=0xe6;
	str[6]=0xb8;
	str[7]=0xac;
	str[8]=0xe8;
	str[9]=0xa9;
	str[10]=0xa6;

	// send(socketFileDescriptor,res.c_str(),res.size(),0);
	send(socketFileDescriptor,str,11,0);

	pthread_mutex_unlock( &(fdm.mutex) );
	sleep(1);
	cout << "send a msg" << endl;
}
