// CLIENT SIDE UDP CODE

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
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = { 0 };
    string uname;
    int choice;
    int serv_len = sizeof(serv_addr);

    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Create UDP socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Socket creation failed." << endl;
        WSACleanup();
        return;
    }

    // Set up server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Send username to server
    //sendto(sock, message.c_str(), message.length(), 0, (struct sockaddr*)&serv_addr, serv_len);
    cout << " Username :: ";
    getline(cin, uname);
    sendto(sock, uname.c_str(), uname.length() + 1, 0, (struct sockaddr*)&serv_addr, serv_len);

    // Receive Welcome response from server
    recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serv_addr, &serv_len);
    cout << "Server: " << buffer << endl;

    // Receive menu from server
    recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serv_addr, &serv_len);
    cout << buffer << endl;

    //Send client selected option to server
    cout << "Option :: ";
    cin >> choice;
    sendto(sock, (char*)&choice, sizeof(choice), 0, (struct sockaddr*)&serv_addr, serv_len);
    cout << endl;

    if (choice == 1)
    {
        //Uploading a file........
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
        sendto(sock, filepath.c_str(), filepath.length() + 1, 0, (struct sockaddr*)&serv_addr, serv_len);

        // Step 2: Send file size
        inFile.seekg(0, ios::end);
        long filesize = inFile.tellg();
        inFile.seekg(0);
        sendto(sock, (char*)&filesize, sizeof(filesize), 0, (struct sockaddr*)&serv_addr, serv_len);

        // Step 3: Send file content
        char buffer[1024];
        while (!inFile.eof())
        {
            inFile.read(buffer, 1024);
            int bytes = inFile.gcount();
            sendto(sock, buffer, bytes, 0, (struct sockaddr*)&serv_addr, serv_len);
        }

        inFile.close();
        cout << "File sent to server.\n";

    }
    else if (choice == 2)
    {
        //Downloading a file............
        string filename;
        cout << "Enter the filename to download: ";
        cin.ignore();
        getline(cin, filename);

        // Step 1: Send filename to server
        sendto(sock, filename.c_str(), filename.length() + 1, 0, (struct sockaddr*)&serv_addr, serv_len);

        // Step 2: Receive file size
        long filesize = 0;
        int recvLen = recvfrom(sock, (char*)&filesize, sizeof(filesize), 0, (struct sockaddr*)&serv_addr, &serv_len);

        if (filesize == -1) {
            cout << "Server says file not found.\n";
            return;
        }

        cout << "File size: " << filesize << " bytes\n";

        // Step 3: Receive file content
        ofstream outFile("downloaded_" + filename, ios::binary);
        char buffer[1024];
        long bytesReceived = 0;

        while (bytesReceived < filesize) {
            int bytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&serv_addr, &serv_len);
            if (bytes <= 0) break;

            outFile.write(buffer, bytes);
            bytesReceived += bytes;
        }

        outFile.close();
        cout << "File downloaded and saved as: downloaded_" << filename << endl;
    }
    else if (choice == 3)
    {
        //Availability of Files....
        char buffer1[BUFFER_SIZE] = { 0 };
        recvfrom(sock, buffer1, BUFFER_SIZE, 0, (struct sockaddr*)&serv_addr, &serv_len);
        cout << buffer1 << endl;
        char buffer2[BUFFER_SIZE] = { 0 };
        recvfrom(sock, buffer2, BUFFER_SIZE, 0, (struct sockaddr*)&serv_addr, &serv_len);
        cout << buffer2 << endl;
    }
    else
    {
        //Exiting from UDP Port......
        // Receive menu from server
        char buffer[BUFFER_SIZE] = { 0 };
        recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&serv_addr, &serv_len);
        cout << buffer << endl;
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
}

int main()
{
    client();
    return 0;
}