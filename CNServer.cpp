#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <iterator>
#include <list>
#include <sstream>
#include <ctype.h>
#include <filesystem>
#include <fstream>

using namespace std;

namespace fs = filesystem;

#define SERVER_PORT_NO 80

list<int> sockets;
list<int> ports;
list<string> names;
list<string> sharedFiles;
list<string> totalFiles;
list<string> fileClients;
list<string> filesLocation;
list<string> temp;

struct sockaddr_in cliaddr;
socklen_t cliaddr_len;

void* RecieveMessage(void *connfd);


int main() {

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("Socket Creation failed\n");
		return -1;
	}

	struct sockaddr_in addr;

	addr.sin_addr.s_addr	= INADDR_ANY;
	addr.sin_family	= AF_INET;
	addr.sin_port		= htons(SERVER_PORT_NO);

	if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
		perror("Bind failed on socket\n");
		return -1;	
	}


	int backlog = 5;
	if (listen(fd, backlog) == -1) {
		perror("Listen Failed on server: \n");
		return -1;
	} 

    pthread_t client_handle;
    pthread_t client_handle2;
	int connfd;
	while (true) {	
		connfd = accept(fd, (struct sockaddr *) &cliaddr, &cliaddr_len);

		if (connfd <=0 ) {
			perror("accept failed on socket: ");
		}

		sockets.push_back(connfd);
		ports.push_back(cliaddr.sin_port);
		long connfd2 = (long) connfd;
		pthread_create(&client_handle, NULL, RecieveMessage, (void*) connfd2);
	}

	pthread_join(client_handle, NULL);

	cout<<"\n______\n\n";
	
	return 0;
}

void* RecieveMessage(void *connfd) {
	long connfd2 = (long) connfd;
	int connfd3 = (int) connfd2;
    list<int>::iterator itr;
	list<string>::iterator itr2;
	string name = "";
	for (itr = sockets.begin(), itr2 = names.begin(); itr != sockets.end(); ++itr, ++itr2) {
		if (connfd3 == *itr) {
			name = *itr2;
		}
	}
	int count = 0;
	char buffer[100];
    for  (int i = 0;; i++)
    {

		if (count == 0) {
			count++;
			for (itr = sockets.begin(), itr2 = names.begin(); itr != sockets.end(); ++itr, ++itr2) {
				if (connfd3 == *itr) {
					name = *itr2;
				}
			}
		}
	    bzero(buffer, 100);
	    recv(connfd3, buffer, 100, 0);
		if (buffer[0] == '0' && buffer[1] == '0') {
			if (names.size() > 0) {
				list<string>::iterator itr;
				name = "";
				for (int i = 2; i < strlen(buffer); i++)
				{
					name = name + buffer[i];
				}
				int count = 0;
				for (itr = names.begin(); itr != names.end(); ++itr) {
					if (name == *itr) {
						count++;
						break;
					}
				}
				if (count == 0) {
					names.push_back(name);
					string temp = "Successfully connected with Server";
					int n = temp.length();
					char bufferr[n+1];
					strcpy(bufferr, temp.c_str());
					send(connfd3, bufferr, strlen(bufferr), 0);
				} else {
					string temp = "Name already taken. Enter a new name.";
					int n = temp.length();
					char bufferr[n+1];
					strcpy(bufferr, temp.c_str());
					send(connfd3, bufferr, strlen(bufferr), 0);
				}
			}
			else {
				name = "";
				for (int i = 2; i < strlen(buffer); i++)
				{
					name = name + buffer[i];
				}
				names.push_back(name);
				string temp = "Step cleared";
				int n = temp.length();
				char bufferr[n+1];
				strcpy(bufferr, temp.c_str());
				send(connfd3, bufferr, strlen(bufferr), 0);
			}
		}
		else if (buffer[0] == '0' && buffer[1] == '1') {
			cout<<"[" << name << "]: joined the network." << endl;
			string message = "";
			for (int i = 2; i < strlen(buffer); i++) {
				message = message + buffer[i]; //message has textfiles and location of those files Eg: file1.txt#Hehe.txt#%/home/kali/Downloads/NASA
			}
			sharedFiles.push_back(message);
			string message2 = "";
			bool match = 0;
			for (int i = 0; i < message.length(); i++) {
				if(message[i] == '%'){
					break;
				}
				else if (message[i] != '#') {
					message2 = message2 + message[i];
				} else {
			 		list<string>::iterator itr;
			 		list<string>::iterator itr2;
					match = 0;
					for (itr = totalFiles.begin(), itr2 = fileClients.begin(); itr != totalFiles.end(); itr++, itr2++) {
						if (*itr == message2) {
							*itr2 = *itr2 + name + "#";
							match = 1;
							break;
						}
					}
					if (match == 0) {
						totalFiles.push_back(message2);
						fileClients.push_back(name + "#");
					}
					message2 = "";
				}
			}
			// Find the position of '%' in the string
			size_t percentSignPos = message.find('%');

			if (percentSignPos != string::npos) {
				// Extract the substring after '%' and saving the file location
				string msg2 = message.substr(percentSignPos + 1);
				filesLocation.push_back(msg2);
			}
			string clientNames = "";
			list<string>::iterator itr;
			list<int>::iterator itr2;
			list<int>::iterator itr3;
			list<string>::iterator itr4;
			list<string>::iterator itr5;
			list<string>::iterator itr6;
			list<string>::iterator itr7;
			// Displaying the table
			cout << endl;
    		cout << setw(15) << left << "Name" << setw(10) << left << "Port" << setw(15) << left << "Socket" << "Shared Files\n";
			cout << setw(15) << left << "-----"<< setw(10) << left << "-----" << setw(15) << left << "-------" << "---------------\n";
    		for (itr = names.begin(), itr2 = ports.begin(), itr3 = sockets.begin(), itr4 = sharedFiles.begin(); itr != names.end(); ++itr, ++itr2, ++itr3, ++itr4) {
        		cout << setw(15) << left << *itr << setw(10) << left << *itr2 << setw(15) << left << *itr3;
				for (char c : *itr4) {
					if (c == '%'){
						break;
					}
            		else if (c == '#') {
                		cout << "  ";  // Replace '#' with a space
            		} else {
                		cout << c;
           			}
        		}
				cout << endl;
				clientNames = clientNames + "#" + *itr;

    		}
			cout << endl;
			
			clientNames = "01" + clientNames;
			// Convert clientNames to a C-style string for sending
			const char* bufferToSend = clientNames.c_str();
			send(connfd3, bufferToSend, strlen(bufferToSend), 0);

		}
		else if (buffer[0] == '0' && buffer[1] == '2') {
			// Handle case when the client sends only "02"
			string allFilesAndSharedPersons;
			list<string>::iterator itr;
			list<string>::iterator itr2;

			for (itr = totalFiles.begin(), itr2 = fileClients.begin(); itr != totalFiles.end(); ++itr, ++itr2) {
				string fileName = *itr;
				string sharedPersons = *itr2;
				cout << "File Name:   " << fileName << "\t\t" << "fileClients:   " << sharedPersons << endl;

				replace(sharedPersons.begin(), sharedPersons.end(), '#', ' ');

				allFilesAndSharedPersons += fileName + " sharing by : " + sharedPersons + "\n";
			}
			// Convert allFilesAndSharedPersons to a C-style string for sending
			const char* bufferToSend = allFilesAndSharedPersons.c_str();

			send(connfd3, bufferToSend, strlen(bufferToSend), 0);
		}
		else if (buffer[0] == '0' && buffer[1] == '3') {
			string message = "";
			for (int i = 2; i < strlen(buffer); i++) {
				message = message + buffer[i];
			}
			string message2 = "";
			list<string>::iterator itr;
			list<string>::iterator itr2;
			for (itr = names.begin(), itr2 = sharedFiles.begin(); itr != totalFiles.end(); ++itr, ++itr2) {
				if (message == *itr) {
					message2 = *itr2;
					break;
				}
			}
			string message3 = "";
			for (int i = 0; i < message2.length(); i++) {
				if (message2[i] != '#') {
					message3 = message3 + message2[i];
				}
				else {
					temp.push_back(message3);
					message3 = "";
				}
			}
			list<string>::iterator itr3;
			string message5 = "";
			string message6 = "";
			if (temp.size() > 1) {
				message6 = " files are";
			} else {
				message6 = " file is";
			}
			while (!temp.empty()) {
				if (temp.size() > 2) {
					message5 = message5 + temp.front() + ", ";
				}
				else if (temp.size() == 2) {
					message5 = message5 + temp.front() + " and ";
				}
				else if (temp.size() == 0) {
					cout << "Hello " << endl;
				}
				else {
					message5 = message5 + temp.front() + message6 + " shared by " + message + ".";
				}
				temp.pop_front();
			}
			int n = message5.length();
			char buffer[n+1];
			strcpy(buffer, message5.c_str());
            send(connfd3, buffer, strlen(buffer), 0);
		}
		else if (buffer[0] == '0' && buffer[1] == '4') {
			cout << name << " has left the chat\n";
			list<string>::iterator itr;
			list<int>::iterator itr2;
			list<int>::iterator itr3;
			list<string>::iterator itr4;
			list<string>::iterator itr9;
			for (itr = names.begin(), itr2 = ports.begin(), itr3 = sockets.begin(), itr4 = sharedFiles.begin(), itr9 = filesLocation.begin(); itr != names.end(); itr++, itr2++, itr3++, itr4++, itr9++) {
				if (*itr == name) {
					names.remove(*itr);
					ports.remove(*itr2);
					sockets.remove(*itr3);
					sharedFiles.remove(*itr4);
					filesLocation.remove(*itr9);
					break;
				}
			}
			list<string>::iterator itr5;
			list<string>::iterator itr6;
			for (itr5 = fileClients.begin(), itr6 = totalFiles.begin(); itr5 != fileClients.end(); itr5++, itr6++) {
				string original = *itr5;
				string substring = name + "#";
				size_t ind = original.find(substring);
				if(ind != string::npos){
					original.erase(ind,substring.length());
					if (original != "") {
						*itr5 = original;
					} else {
						fileClients.remove(*itr5);
						totalFiles.remove(*itr6);
						break;
					}
				}
			}
			break;
		}
        else if (buffer[0] == '0' && buffer[1] == '5'){
			cout << "Searching for client \n";
            list<string>::iterator itr;
			list<int>::iterator itr2;
			list<int>::iterator itr3;
			cout << " Name = " << name << endl;
			string otr_cli;
			for(int i = 2;i < strlen(buffer);i++){
				otr_cli += buffer[i];
			}
			cout << otr_cli << " cli" << endl;
            for (itr = names.begin(), itr2 = sockets.begin(),itr3 = ports.begin(); itr != names.end(); ++itr, ++itr2, ++itr3) {
				if (otr_cli == *itr) {
					cout << "Yes" << endl;
                    // Asking for new port number from client
					char start_temp_serv[100]="11";
					send(connfd3,start_temp_serv, strlen(start_temp_serv), 0);
					// Recv portnumber from client which will act as server.
					bzero(start_temp_serv, 100);
	    			recv(connfd3, start_temp_serv, 100, 0);

					cout << "Yess" << endl;
					// Send recieved port number to other client	
					string str(start_temp_serv);
					str = "10" + str;
                    int leng = str.length();
		            char port_number[leng + 1];
		            strcpy(port_number, str.c_str());
					int temp_sock = *itr2;
					send(temp_sock,port_number, strlen(port_number), 0);
					break;
				}				
			}
		}
		else if (buffer[0] == '0' && buffer[1] == '6') {	
			string name2 = "";
			string message = "";
			for (int i = 2; i < strlen(buffer); i++) {
				message = message + buffer[i];
			}
			for (int i = 2; i < strlen(buffer); i++) {
				if(buffer[i] == '%'){
					break;
				}
				name2 = name2 + buffer[i];
			}
			cout << name2 << " --name2 \t" << message << " mesg" << endl; 
			list<string>::iterator itr;
			list<int>::iterator itr2;
			string fileContents;
			size_t percentPos = message.find('%'); //finding the % position
			if (percentPos != string::npos) {
                // Extract the substring after '%' and saving the file location
                fileContents = message.substr(percentPos + 1);
			}
			fileContents = "[ " + name + " ]:- " + fileContents;
			char buffer4[fileContents.length()+1];
			strcpy(buffer4, fileContents.c_str());
			bool exist = false;
			for (itr = names.begin(), itr2 = sockets.begin(); itr != names.end(); itr++, itr2++) {
				if (name2 == *itr) {
					send(*itr2, buffer4, strlen(buffer4), 0);
					exist = true;
				}
			}
			if(!exist){
				char buffer4[] = "There is no peer with this name!!!!\n";
				send(connfd3, buffer4, strlen(buffer4), 0);
			}
		}
		else if (buffer[0] == '0' && buffer[1] == '7') {
			string message = "";
			for (int i = 2; i < strlen(buffer); i++) {
				message = message + buffer[i];
			}
			list<string>::iterator itr;
			string folderPath = "";
			bool fileExists = false;
			for(itr = filesLocation.begin(); itr != filesLocation.end(); itr++){
				folderPath = *itr;

				for (const auto& entry : fs::directory_iterator(folderPath)) {
					if (entry.path().filename() == message) {
						// File found
						fileExists = true;
						cout << message << " exists in the server." << endl;
						cout << "File Uploading....." << endl;

						// Open the file and read its contents into another string
						ifstream file(entry.path());
						if (file.is_open()) {
							string fileContents((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

							// Now 'fileContents' contains the content of the file and sending that message for downloading
							fileContents = string("07") + "#" + message + "%" + fileContents;
							char buffer4[fileContents.length() + 1];
							strcpy(buffer4, fileContents.c_str());
							send(connfd3, buffer4, strlen(buffer4), 0);
							sleep(2);
							cout << "File Uploaded Successfully\n" << endl;
						} else {
							cerr << "Error opening file: " << entry.path() << endl;
						}

						//update in the sharedfiles of the newly downloaded file
						list<string>::iterator itr4;
						for (itr = names.begin(), itr4 = sharedFiles.begin(); itr != names.end(); ++itr, ++itr4) {
							if(*itr == name){
								string fileEntry;
								size_t lastHashPos = itr4->rfind('#'); //find the last # position
								
								if (lastHashPos != string::npos) {
									// Extract the portion before the last '#'
									fileEntry = itr4->substr(0, lastHashPos + 1);
									
									// Append the new file
									fileEntry += message + "#";

									// Append the remaining portion after the last '#'
									fileEntry += itr4->substr(lastHashPos + 1);
									
									// Update sharedFiles with the modified entry
									*itr4 = fileEntry;
								}
								else{
									*itr4 = message + "#" + *itr4;
								}
							}
						}
						list<string>::iterator itr2;

						for (itr = totalFiles.begin(), itr2 = fileClients.begin(); itr != totalFiles.end(); ++itr, ++itr2) {
							string fileName = *itr; //filename
							// *itr2 is shared clients
							if(fileName == message){
								*itr2 = *itr2 + name + "#";
							}
						}
						// Break out of the loop since we found the file
						break;
					}
				}
				if(fileExists){
					break;
				}
			}
			if (!fileExists) {
				// File not found, print a message
				cout << "No file named " << message << " exists in the server." << endl;
				string fileContents = "There is no file named " + message + " exists in the server!!!";
				char buffer4[fileContents.length() + 1];
				strcpy(buffer4, fileContents.c_str());
				send(connfd3, buffer4, strlen(buffer4), 0);
			}
		}
		else {
		    cout<<"[" << name << "]: " << buffer << endl;
		}
    }
    return NULL;
}
