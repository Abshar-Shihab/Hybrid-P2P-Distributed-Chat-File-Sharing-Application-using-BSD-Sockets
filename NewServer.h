#include "NewClient.h"

void* send_msg_to(void *connfd1){

	char buff[1024];
	long connfd = (long) connfd1;
	while(true){
		bzero(buff,1024);
		cin.getline(buff,1024);
		send(connfd, buff, strlen(buff), 0);
		if(buff[0] == '1'){
			break;
		}
	}
	return NULL;
}

void* rec_msg_from_client(void *connfd1){

	string send_data = "";
	long connfd = (long) connfd1;
	char buffer[1024];
	
	while(send_data != "1")	
	{
		// Recieve Data From Client
		bzero(buffer, 100);
		recv(connfd, buffer, 100, 0);
		if(strlen(buffer)<=0){
			cout << " Connection error, please reconnect " <<endl;
			sleep(5);
			break;
		}
		if(buffer[0] == '1' && buffer[1]=='3')
		{
			cout << " Downloading File" << endl;
			fstream handlez;
			handlez.open("rec.txt",ios::out);
		label_con:
			bzero(buffer, 100);
			recv(connfd, buffer, 100, 0);
			if(buffer[0] == '1' && buffer[1]=='4'){
				goto end_if;
			}
			handlez << buffer;
			goto label_con;
		end_if:
			cout << "file download complete" <<endl;
			handlez.close();
			char fin[]="1";
			send(connfd,fin,strlen(fin),0);
			break;
		}
		int len_in_data = strlen(buffer);
		send_data = "";
		for(int i = 0; i < len_in_data; i++){
			send_data += buffer[i];
		}
		if(send_data == "1"){
			send(connfd, buffer, len_in_data, 0);
			break;
		}

		cout << buffer << endl;
	}
	return NULL;
}

class chat_server{
	long fdc;
	struct sockaddr_in addr;
	int portt_num;
public:
    chat_server(){
		fdc = socket(AF_INET, SOCK_STREAM, 0);
		if (fdc == -1) {
	    	perror("Socket Creation failed\n");
	    }
		
		srand(time(0));
 		int min = 10000;
		int max = 60000;
		portt_num = min + rand() % (max - min + 1);
	    
		addr.sin_addr.s_addr	= INADDR_ANY;
	    addr.sin_family	= AF_INET;
	    addr.sin_port		= htons(portt_num);
	};   
	int start_listening(){
        
        if (bind(fdc, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		perror("Bind failed on socket\n");	
	    }
		return portt_num;
        }
	int accept_conn(){
		int backlog = 10;
	    if (listen(fdc, backlog) >= -1) {
	    }

		struct sockaddr_in cliaddr;
	    socklen_t cliaddr_len;
		label1:
		cout << "Start accepting\n";
	    long connfdc = accept(fdc, (struct sockaddr *) &cliaddr, &cliaddr_len);
	    if (connfdc <= 0 ){
	    		perror("accept failed on socket: ");
				sleep(1);
				return -1;
	    }else
		{
	    	cout << "Connection established " <<endl;
			sleep(2);
			pthread_t hand[2];
        	// create thread for recieving msges from multiple clients and move on
	    	pthread_create(&hand[0],NULL,send_msg_to,(void*) connfdc);
	    	pthread_create(&hand[1],NULL,rec_msg_from_client,(void*) connfdc);

	    	pthread_join(hand[0],NULL);
			pthread_join(hand[1],NULL);
		}
		return 01;
	}
	~chat_server(){
		close(fdc);
	};
};
