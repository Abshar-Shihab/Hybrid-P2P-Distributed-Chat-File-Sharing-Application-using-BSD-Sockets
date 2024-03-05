#include <string.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <signal.h>
#include <filesystem>
#include <list>
#include <iterator>
#include <fstream>

using namespace std;
list<string> file_name;
list<string> location;

void* recv_msg_from(void *fd1)
{
	long fd = (long) fd1;
	char buff[1024];
	while(true)	
		{
			// Start recieving data;
			bzero(buff, 1024);
			recv(fd, buff, 1024, 0);
			if(buff[0] == '1' && buff[1]=='2')
			{
				list<string>::iterator itr;
				list<string>::iterator itr2;
				
				string garb;
				for(int i = 2;i<strlen(buff);i++){
					garb+= buff[i];
				}
				//cout << "Grab= "<< garb << endl;
				for(itr=file_name.begin(),itr2=location.begin(); itr != file_name.end(); ++itr,++itr2)
				{
					if(*itr== garb)
					{
						char buz[]="13";
						send(fd,buz,strlen(buz),0);
						
						sleep(2);
						
						fstream hand;
						hand.open(*itr2 , ios::in);
						 if (hand.is_open()) 
						 {
							//string get_data;
   						   while (!hand.eof()) 
						   {  
							  //int len= get_data.length();
							  char buzz[10240];
							  bzero(buzz, 10240);
							  //strcpy(buzz,get_data.c_str());
   						      hand.getline(buzz, 10240);
							  //cout << buzz;
							  send(fd,buzz,strlen(buzz),0);
							  sleep(1);
   						   }
   						   hand.close();
						   char fin_mess[]="14";
						   send(fd,fin_mess,strlen(fin_mess),0);
						   cout << " Upload Complete " << endl;
   						}
						break;
					}
				}
			}

			// Check if server left or not
			if(buff[0] == '1')
			{
				send(fd, buff, strlen(buff), 0);
				break;
			}
			cout << buff << endl;
			if(strlen(buff) <= 0){
				cout << " Connection error, try reconnecting " <<endl;
				char bufff[] = "1";
				send(fd,bufff,strlen(bufff),0);
				break;
			}
		}
		return NULL;
}
void* send_msg_to_server(void *fd1)
{
	long fd = (long) fd1;	
	
	char buffer[1024];
	while(true)	
	{
		//char buffer[1024];
		// Take Input from User
		bzero(buffer,1024);
		
		cin.getline(buffer,1024);

		if(strlen(buffer) >= 1){
		send(fd, buffer, strlen(buffer), 0);
		}else{
			cout << " Connection error  "<<endl;
			break;
		}

		// Check if you want to break loop
		if(buffer[0] == '1')
		{
			break;
		}
	}
	return NULL;
}
class chat_client{
    int port;
	long fdd;
public:
chat_client(int p):port(p){};
int start_client(){
    	// Create Listening Socket
    	fdd = socket(AF_INET, SOCK_STREAM, 0);
    	if (fdd == -1) {
    		perror("Socket Creation failed\n");
    	}


    	// Assign address 
    	struct sockaddr_in s_addr;

    	s_addr.sin_family 	= AF_INET;
    	s_addr.sin_port	= htons(port);

    	// Convert to server Address
    	inet_aton("127.0.0.1", &s_addr.sin_addr);
		//cout << "connect to ser soc = " << s_addr.sin_port << endl;
    	label1:
		if (connect(fdd, (struct sockaddr *) &s_addr, sizeof(s_addr)) <= -1) {
			perror("Socket Connect failed\n");
			return -1;
    	}else
		{
    		//sleep(1);
			pthread_t handle;
    		pthread_create(&handle, NULL, send_msg_to_server, (void*)fdd);
    		pthread_create(&handle, NULL, recv_msg_from, (void*)fdd);		
    		
			//Join all created Pthreads
    		pthread_join(handle, NULL);
		}
			return 01;
    }
	~chat_client(){
			close(fdd);
		};
};
