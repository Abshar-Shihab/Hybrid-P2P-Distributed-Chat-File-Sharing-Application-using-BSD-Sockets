#include "NewServer.h"

#define SERVER_PORT_NO 80

void catch_ctrl_c(int signal) ;
void* RecieveMessage(void *connfd);
void* SendMessage(void *connfd);
void* Sen_Mess_to_ser(void *connfd);

namespace fs = filesystem;

pthread_t thread_handle;
pthread_t thread_handle2;
int fd;
bool flag = true;
bool first = true;

void displayMenu() {
    cout << "\n=== Menu ===\n";
    cout << "01 - List connected clients\n";
    cout << "02 - Shared Files Directory\n";
    cout << "03 - Specific Client's Shared Files\n";
    cout << "04 - Exit\n";
    cout << "05 - Establish direct connection with peer\n";
    cout << "06 - Message a Client\n";
    cout << "07 - File Download Central\n";
    cout << "Enter your choice: " << flush;
}

string address; //global variable

int main() {


	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("Socket Creation failed\n");
		return -1;
	}


	struct sockaddr_in s_addr;

	s_addr.sin_family 	= AF_INET;
	s_addr.sin_port	= htons(80);
	inet_aton("127.0.0.1", &s_addr.sin_addr);
	if (connect(fd, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
		perror("Socket Connect failed\n");
		return -1;
	}
    signal(SIGINT, catch_ctrl_c);
    signal(SIGHUP, catch_ctrl_c);

    label1:
    string name0 = "00";
    string name1 = "";
    cout << "Enter Name: ";
    getline(cin, name1);
    name0 = name0 + name1;
    int n = name0.length();
    char buffer1[n+1];
    strcpy(buffer1, name0.c_str());
    send(fd, buffer1, strlen(buffer1), 0);
    char buffer2[100];
    bzero(buffer2, 100);
    recv(fd, buffer2, 100, 0);
    cout << "[Server]: " << buffer2 << endl;
    string name = "";
    for (int i = 0; i < strlen(buffer2); i++)
    {
        name = name + buffer2[i];
    }
    if (name == "Name already taken. Enter a new name.") {
        goto label1;
    }
    string message = "01";
    string temp;
    int choose;      //address is already declared as a global variable
    cout << "Choose the folder to share your file" << endl;
    cout << "1 :- NASA\n2 :- CN\n3 :- SNS\nEnter: ";
    cin >> choose;
    if(choose == 1){
        address = "/home/kali/Downloads/NASA";
    }
    else if(choose == 2){
        address = "/home/kali/Downloads/CN";
    }
    else if(choose == 3){
        address = "/home/kali/Downloads/SNS";
    }
    else{
        cout << "Wrong folder choose, Defaulty set to NASA folder" << endl;
        address = "/home/kali/Downloads/NASA";
    }
    for (const auto& entry : fs::directory_iterator(address))
    {
        temp = entry.path().string();
        location.push_back(temp);
        const size_t last_slash_idx = temp.find_last_of("/");
		if (std::string::npos != last_slash_idx)
		{
			temp.erase(0, last_slash_idx + 1);
		}
        file_name.push_back(temp);
        message = message + temp + "#";
    }
    int count1 = 0;
    for (int i = 0; i < message.length(); i++) {
        if (message[i] == '#') {
            count1++;
        }
    }
    if (count1 < 1) {
        cout << "You don't Have any files to upload in Server But You can Download\n";
    }
    message = message + "%" + address; //sending all details with file location like this:----> 01file1.txt#Hehe.txt#%/home/kali/Downloads/NASA
    n = message.length();
    char bufferr[n+1];
    strcpy(bufferr, message.c_str());
    send(fd, bufferr, strlen(bufferr), 0);
    /*if (count1 < 1) { //If a peer doesnt has a file to share, then we can uncomment this to not to connect with server
        char buffer2[] = "04";
        send(fd, buffer2, strlen(buffer2), 0);
    }*/
    long fd2 = (long) fd;
    pthread_create(&thread_handle, NULL, RecieveMessage, (void*)fd2);
    pthread_create(&thread_handle2, NULL, Sen_Mess_to_ser, (void*)fd2);


	pthread_join(thread_handle, NULL);
    pthread_join(thread_handle2, NULL);
	cout<<"\n______\n\n";
	return 0;
}

void catch_ctrl_c(int signal) 
{
	char str[100]="04exit";
	send(fd,str,sizeof(str),0);
    int pthread_detach(pthread_t thread_handle);
	close(fd);
	exit(signal);
}

void* Sen_Mess_to_ser(void *connfd){
    long fd = (long) connfd;
    char buffer[1024];
    bzero(buffer,1024);
    while (buffer[0] != '-')
    {
        if(flag == true){
            cin.getline(buffer, 1024);
            string temp = "", temp1 = "";
            if(buffer[0] == '0' && buffer[1] == '3'){
                string temp = "";
                cout << "Enter the Client's Name: ";
                cin >> temp;
                for (int i = 0;i<temp.length();i++){
                    buffer[i+2] = temp[i];
                }
                send(fd, buffer, strlen(buffer), 0);
            }
            else if(buffer[0] == '0' && buffer[1] == '5'){
                string temp = "";
                cout << "Enter the Client's Name: ";
                cin >> temp;
                for (int i = 0;i<temp.length();i++){
                    buffer[i+2] = temp[i];
                }
                send(fd, buffer, strlen(buffer), 0);
            }
            else if(buffer[0] == '0' && buffer[1] == '6'){
                string temp = "", temp1 = "";
                cout << "Enter the Client's Name: ";
                getline(cin, temp);
                cout << "Enter the Message to Send: ";
                getline(cin, temp1);
                temp = temp + "%" + temp1;
                for (int i = 0;i<temp.length();i++){
                    buffer[i+2] = temp[i];
                }
                send(fd, buffer, strlen(buffer), 0);
            }
            else if(buffer[0] == '0' && buffer[1] == '7'){
                string temp = "";
                cout << "Enter the File Name to Download: ";
                cin >> temp;
                for (int i = 0;i<temp.length();i++){
                    buffer[i+2] = temp[i];
                }
                send(fd, buffer, strlen(buffer), 0);
            }
            else{
                 send(fd, buffer, strlen(buffer), 0);
            }

        }
        else
        {
            sleep(5);
        }
    }
    return NULL;
}

void* RecieveMessage(void *fd) {
    cout << "I am recieving" << endl;
    long fd3 = (long) fd;
    char buffer[1000];
    displayMenu();

    for  (int i = 0;; i++)
    {
	    bzero(buffer, 100);
	    recv(fd3, buffer, 100, 0);
	    //cout<<"Server : "<< buffer << endl;
        if(buffer[0] == '1'&& buffer[1]=='0'){
           string temp;
           for (int i = 2;i<strlen(buffer);i++){
            temp += buffer[i];
           }
            int port_num = stoi(temp);
            //cout << "Port Number = " << temp << endl;
            chat_client chat_now(port_num);
            
            flag = false;
            sleep(2);
            //cin.sync();
            cin.clear();
            int count = 1;
            retry:
            if(chat_now.start_client()==-1){
                sleep(1);
                if(count <= 5){
                    count++;
                    goto retry;
                }
                cout << " Could not establish connection, please try again latter" << endl;
            }
            sleep(2);
            flag = true;
        }
        else if (buffer[0] == '1'&& buffer[1]=='1'){
           
            cin.clear();
            
            chat_server new_ser;
           
            // Take client acting as server's port number
            uint16_t por = new_ser.start_listening();
           
            //cout << "Chat serv port = " << por << endl;
            string str = to_string(por);

            // Send port to Main server
            char ch_array[str.length() + 1];
            strcpy(ch_array, str.c_str());
            send(fd3,ch_array,strlen(ch_array),0);
            flag = false;
            
            if(new_ser.accept_conn() ==-1){
                cout << "Connection failed\n";
            }
            
            flag = true;
        }
        else if (buffer[0] == '0' && buffer[1] == '1'){
            if(!first){ // not to running for the first time
                string message = "";
			    for (int i = 2; i < strlen(buffer); i++) {
				    message = message + buffer[i];
			    }
                cout << "\nConnected Clients: ";
                int counter = 1;
                for (int i = 0; i < message.length(); i++) {
                    if(message[i] == '#'){
                        cout << endl << counter << " : ";
                        counter++;
                    }
                    else{
                        cout << message[i];
                    }
    		    }
            }
            first = false;
        }
        else if (buffer[0] == '0' && buffer[1] == '7'){
            string message = "";
			for (int i = 2; i < strlen(buffer); i++) {
				message = message + buffer[i];
			}
            // Find the positions of '#' and '%'
            size_t hashPos = message.find('#');
            size_t percentPos = message.find('%');

            // Check if both '#' and '%' are found
            if (hashPos != string::npos && percentPos != string::npos) {
                // Extract the substring between '#' and '%'
                string fileName = message.substr(hashPos + 1, percentPos - hashPos - 1);

                // Print the result
                cout << "fileName: " << fileName << endl;

                // Find the position of '%' in the string
                size_t percentSignPos = message.find('%'); //message has like this:---> #file1.txt%content of the file
                

                if (percentPos != string::npos) {
                    // Extract the substring after '%' and saving the file location
                    string fileContents = message.substr(percentPos + 1); // fileContents has the content to write in a file

                    // Create and open the file with the specified fileName
                    fileName = address + "/" + fileName;
                    ofstream outputFile(fileName);

                    // Check if the file is opened successfully
                    if (outputFile.is_open()) {
                        // Write the contents to the file
                        outputFile << fileContents;

                        // Close the file
                        outputFile.close();

                        sleep(2);
                        cout << "File Downloaded Successfully." << endl;
                    } else {
                        cerr << "Error: Unable to open the file for writing." << endl;
                    }
                }
            } else {
                cout << "Invalid format: Unable to find '#' and '%' characters." << endl;
            }
        }
        else{
            cout << "\n" << buffer << endl;
        }
        cout << "\n\nEnter your choice : " << flush; // use flush for to delete the delay in printing on console
    }
    return NULL;
}
