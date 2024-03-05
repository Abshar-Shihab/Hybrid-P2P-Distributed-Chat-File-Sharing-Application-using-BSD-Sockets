# Hybrid-P2P-Distributed-Chat-File-Sharing-Application-using-BSD-Sockets
The application is designed to enable communication and file sharing among community members. The key requirements include sharing details about files in a client's shared directory, allowing search and download of shared files, supporting full-duplex chat, and maintaining a community with at least four members.

## Server Implementation
The server is responsible for:
-	Accepting incoming connections from clients.
-	Maintaining a list of active clients and their shared files.
-	Handling file search requests.
-	Facilitating chat sessions between clients.
The server uses BSD sockets to establish connections with clients and manages a database of active clients and their shared files.

## Client Implementation
Each client is responsible for:
-	Connecting to the server upon joining the community.
-	Sharing details about its shared files with the server.
-	Initiating and participating in full-duplex chat sessions.
-	Sending file search requests to the server and downloading files from other clients.
