//SERVER SIDE TCP CODE

#include <iostream>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
#define PORT 3001
#define BUFFER_SIZE 1024
using namespace std;

void handle_client(SOCKET client_sock)
{
	string menu = "\n Here are the options that you can do in TCP PORT :-  \n 1) Upload a file. \n 2) Download a file. \n 3) List of Available files. \n 4) Exit ";
	cout << menu << endl;
	send(client_sock, menu.c_str(), menu.length() + 1, 0); // +1 for null terminator

	// Buffer for receiving client's selected option
	int option = 0;
	recv(client_sock, (char*)&option, sizeof(option), 0);  // Receiving option as an integer
	cout << "Option received: " << option << endl;  // Debugging output

	string msg;
	if (option == 1)
	{
		msg = "Uploading a file Section.........";
		cout << msg << endl;

		// Step 1: Receive file name
		char filename[256] = { 0 };
		recv(client_sock, filename, sizeof(filename), 0);
		cout << "Receiving file: " << filename << endl;

		// Step 2: Receive file size
		long filesize = 0;
		recv(client_sock, (char*)&filesize, sizeof(filesize), 0);
		cout << "File size: " << filesize << " bytes\n";

		// Step 3: Receive file content
		ofstream outFile(filename, ios::binary);
		char buffer[1024];
		long bytesReceived = 0;

		while (bytesReceived < filesize)
		{
			// Replace std::min with simple conditional logic
			int bytes = recv(client_sock, buffer, (filesize - bytesReceived < sizeof(buffer)) ? (filesize - bytesReceived) : sizeof(buffer), 0);
			if (bytes <= 0) break;
			outFile.write(buffer, bytes);
			bytesReceived += bytes;
		}

		outFile.close();
		cout << "File received and saved.\n";


	}
	else if (option == 2)
	{
		msg = "Downloading a file Section...........";
		cout << msg << endl;
		// Assume server already accepted client socket (clientSock)

		char filename[256] = { 0 };
		recv(client_sock, filename, sizeof(filename), 0); // Step 1: Receive requested filename
		cout << "Client wants to download: " << filename << endl;

		ifstream inFile(filename, ios::binary);
		if (!inFile.is_open()) {
			cout << "File not found.\n";
			long filesize = -1;
			send(client_sock, (char*)&filesize, sizeof(filesize), 0); // Send -1 to signal not found
			return;
		}

		// Step 2: Send file size
		inFile.seekg(0, ios::end);
		long filesize = inFile.tellg();
		inFile.seekg(0);
		send(client_sock, (char*)&filesize, sizeof(filesize), 0);

		// Step 3: Send file content
		char buffer[1024];
		while (!inFile.eof()) {
			inFile.read(buffer, 1024);
			int bytesRead = inFile.gcount();
			send(client_sock, buffer, bytesRead, 0);
		}

		inFile.close();
		cout << "File sent successfully.\n";

	}
	else if (option == 3)
	{
		string msg1 = "Checking Available files Section.........";
		cout << msg1 << endl;

		send(client_sock, msg1.c_str(), msg1.length() + 1, 0); // +1 for null terminator
		string nameoffiles = "\nThe Available Files are:: \n Text.txt \n Html.txt \n email.txt";
		cout << nameoffiles << endl;
		send(client_sock, nameoffiles.c_str(), nameoffiles.length() + 1, 0); // +1 for null terminator
	}
	else
	{
		msg = "\n We are exiting from TCP PORT....... \n Thank You for coming...... \n :)";
		cout << msg << endl;
	}

	send(client_sock, msg.c_str(), msg.length() + 1, 0); // +1 for null terminator
	closesocket(client_sock);
}
void server()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr; int client_len = sizeof(client_addr);
	server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (server_sock == INVALID_SOCKET) {
		cerr << "Socket creation failed." << endl;
		WSACleanup();
		return;
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{

		cerr << "Bind failed." << endl;
		closesocket(server_sock);
		WSACleanup();
		return;
	}
	listen(server_sock, 2);
	cout << "Server listening on port " << PORT << "..." << endl;
	while (true)
	{
		client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
		if (client_sock == INVALID_SOCKET)
		{
			cerr << "Accept failed." << endl;
			continue;
		}
		cout << "Client connected!" << endl;
		char buffer[256] = { 0 }; // Buffer for receiving city name
		recv(client_sock, buffer, sizeof(buffer), 0);
		string uname(buffer);
		// Create the message using string concatenation
		string msg = " Hello! " + uname + " \n Welcome to TCP PORT.......";

		// Print and send
		cout << msg << endl;
		send(client_sock, msg.c_str(), msg.length() + 1, 0); // +1 to include null terminator

		//Now next will be done in handle client function:
		thread t(handle_client, client_sock);			//Multiple clients are handling
		t.detach();
	}

	closesocket(server_sock);
	WSACleanup();
}

int main()
{
	server();
	return 0;
}
