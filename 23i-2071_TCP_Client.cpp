// CLIENT SIDE TCP CODE

#include <iostream>
#include <winsock2.h>
#include<string>
#include <fstream>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 3001
#define BUFFER_SIZE 1024
using namespace std;
void client()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET sock;
	struct sockaddr_in serv_addr;
	string uname, msg;
	int option;			//Selection of client what it will choose
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Socket creation failed." << endl;
		WSACleanup();
		return;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
	if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR)
	{
		cerr << "Connection failed." << endl;
		closesocket(sock); WSACleanup();
		return;
	}
	cout << " Username :: ";
	getline(cin, uname);
	send(sock, uname.c_str(), uname.length() + 1, 0); // +1 to send null terminator

	// Receive info
	char buffer[256] = { 0 }; // Buffer for received message
	recv(sock, buffer, sizeof(buffer), 0);

	// Display server response
	cout << buffer << endl;


	//Display menu to client
	char buffer1[256] = { 0 }; // Buffer for received message
	recv(sock, buffer1, sizeof(buffer1), 0);

	// Display server response
	cout << buffer1 << endl;
	cout << "Option Selected :: ";
	cin >> option;
	send(sock, (char*)&option, sizeof(option), 0);

	//here the code should be like ke jon si option selected hai uss mein ja kr uss ka kaam karay:
	if (option == 1)
	{
		//uploading section

		string filepath;
		cout << " Please enter name of file :: ";
		cin.ignore();
		getline(cin, filepath);
		ifstream inFile(filepath, ios::binary);

		if (!inFile.is_open())
		{
			cerr << "File not found.\n";
			return;
		}

		// Step 1: Send file name
		send(sock, filepath.c_str(), filepath.size() + 1, 0);

		// Step 2: Send file size
		inFile.seekg(0, ios::end);
		long filesize = inFile.tellg();
		inFile.seekg(0);
		send(sock, (char*)&filesize, sizeof(filesize), 0);

		// Step 3: Send file content
		char buffer[1024];
		while (!inFile.eof())
		{
			inFile.read(buffer, 1024);
			int bytes = inFile.gcount();
			send(sock, buffer, bytes, 0);
		}

		inFile.close();
		cout << "File sent to server.\n";

	}
	else if (option == 2)
	{
		//Downloading section
		string filename;
		cout << "Enter the filename to download: ";
		cin.ignore();
		getline(cin, filename);
		send(sock, filename.c_str(), filename.size() + 1, 0); // Step 1: Send filename to server

		// Step 2: Receive file size
		long filesize = 0;
		recv(sock, (char*)&filesize, sizeof(filesize), 0);

		if (filesize == -1) {
			cout << "File not found on server.\n";
			return;
		}

		ofstream outFile("downloaded_" + filename, ios::binary); // Save with a prefix
		char buffer[1024];
		long totalReceived = 0;

		while (totalReceived < filesize) {
			int bytes = recv(sock, buffer, (filesize - totalReceived < sizeof(buffer)) ? (filesize - totalReceived) : sizeof(buffer), 0);
			if (bytes <= 0) break;
			outFile.write(buffer, bytes);
			totalReceived += bytes;
		}

		outFile.close();
		cout << "File downloaded and saved as: downloaded_" << filename << endl;


	}
	else if (option == 3)
	{
		//Listing file section
		// // Receive info
		char buffer2[256] = { 0 }; // Buffer for received message
		recv(sock, buffer2, sizeof(buffer2), 0);
		// Display server response
		cout << buffer2 << endl;
		char buffer4[256] = { 0 }; // Buffer for receiving available files
		recv(sock, buffer4, sizeof(buffer4), 0);
		// Display server response
		cout << buffer4 << endl;

	}
	else
	{
		//Exiting section mein
		// // Receive info
		char buffer3[256] = { 0 }; // Buffer for received message
		recv(sock, buffer3, sizeof(buffer3), 0);
		// Display server response
		cout << buffer3 << endl;

		//cout << "Exit........." << endl;

	}

	closesocket(sock);
	WSACleanup();
}
int main()
{
	client();
	return 0;
}