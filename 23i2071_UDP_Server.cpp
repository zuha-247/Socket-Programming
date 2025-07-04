//SERVER SIDE UDP CODE

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

// Function to handle client request
void handle_client(SOCKET server_sock, sockaddr_in client_addr, int client_len, int choice)
{
    int option = choice;
    string msg;

    if (option == 1)
    {
        //Uploading file........
        msg = "Uploading a file Section.........";
        cout << msg << endl;

        // Step 1: Receive file name
        char filename[256] = { 0 };
        recvfrom(server_sock, filename, sizeof(filename), 0, (struct sockaddr*)&client_addr, &client_len);
        cout << "Receiving file: " << filename << endl;

        // Step 2: Receive file size
        long filesize = 0;
        recvfrom(server_sock, (char*)&filesize, sizeof(filesize), 0, (struct sockaddr*)&client_addr, &client_len);
        cout << "File size: " << filesize << " bytes\n";

        // Step 3: Receive file content
        ofstream outFile(filename, ios::binary);
        char buffer[1024];
        long bytesReceived = 0;

        while (bytesReceived < filesize)
        {
            int bytes = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_len);
            if (bytes <= 0) break;

            outFile.write(buffer, bytes);
            bytesReceived += bytes;
        }

        outFile.close();
        cout << "File received and saved.\n";

    }
    else if (option == 2)
    {
        //Downloading a file.....
        string msg = "Downloading a file Section...........";
        cout << msg << endl;

        // Step 1: Receive filename
        char filename[256] = { 0 };
        int recvLen = recvfrom(server_sock, filename, sizeof(filename), 0, (struct sockaddr*)&client_addr, &client_len);
        if (recvLen <= 0) {
            cerr << "Failed to receive filename.\n";
            return;
        }
        cout << "Client wants to download: " << filename << endl;

        ifstream inFile(filename, ios::binary);
        if (!inFile.is_open()) {
            cout << "File not found.\n";
            long filesize = -1;
            sendto(server_sock, (char*)&filesize, sizeof(filesize), 0, (struct sockaddr*)&client_addr, client_len);
            return;
        }

        // Step 2: Send file size
        inFile.seekg(0, ios::end);
        long filesize = inFile.tellg();
        inFile.seekg(0);
        sendto(server_sock, (char*)&filesize, sizeof(filesize), 0, (struct sockaddr*)&client_addr, client_len);

        // Step 3: Send file content
        char buffer[1024];
        while (!inFile.eof()) {
            inFile.read(buffer, sizeof(buffer));
            int bytesRead = inFile.gcount();
            sendto(server_sock, buffer, bytesRead, 0, (struct sockaddr*)&client_addr, client_len);
        }

        inFile.close();
        cout << "File sent successfully.\n";

    }
    else if (option == 3)
    {
        //Checking Availability of Files.........
        string msg1 = "Checking Available files Section.........";
        cout << msg1 << endl;
        sendto(server_sock, msg1.c_str(), msg1.length(), 0, (struct sockaddr*)&client_addr, client_len);
        cout << msg1 << endl;
        string nameoffiles = "\nThe Available Files are:: \n Text.txt \n Html.txt \n email.txt";
        cout << nameoffiles << endl;
        sendto(server_sock, nameoffiles.c_str(), nameoffiles.length(), 0, (struct sockaddr*)&client_addr, client_len);
    }
    else
    {
        //Exiting from UDP PORT......
        string exitmsg = "Exiting from UDP PORT.........\n Thank you for coming :)";
        sendto(server_sock, exitmsg.c_str(), exitmsg.length(), 0, (struct sockaddr*)&client_addr, client_len);
        cout << exitmsg << endl;
    }

}

void server()
{
    WSADATA wsa;
    SOCKET server_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    int choice;
    char buffer[BUFFER_SIZE] = { 0 };
    string menu = "\n Here are the options that you can do in UDP PORT :-  \n 1) Upload a file. \n 2) Download a file. \n 3) List of Available files. \n 4) Exit ";
    // Initialize Winsock
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Create UDP socket
    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock == INVALID_SOCKET) {
        cerr << "Socket creation failed." << endl;
        WSACleanup();
        return;
    }

    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Bind failed." << endl;
        closesocket(server_sock);
        WSACleanup();
        return;
    }

    cout << "UDP Server listening on port " << PORT << "..." << endl;

    while (true)
    {
        cout << "Client Connected....\n " << endl;
        // Receive message from client
        recvfrom(server_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &client_len);
        cout << "Username :: " << buffer << endl;
        string uname(buffer);

        string message = " Hello! " + uname + " \n Welcome to UDP PORT.......";
        // Send response to client
        sendto(server_sock, message.c_str(), message.length(), 0, (struct sockaddr*)&client_addr, client_len);
        cout << message << endl;        //welcoming user

        // Send Menu to client
        sendto(server_sock, menu.c_str(), menu.length(), 0, (struct sockaddr*)&client_addr, client_len);
        cout << menu << endl;

        // Receive choice from client
        recvfrom(server_sock, (char*)&choice, sizeof(choice), 0, (struct sockaddr*)&client_addr, &client_len);
        cout << "Option: " << choice << endl;

        // Process client request in a new thread
        thread t(handle_client, server_sock, client_addr, client_len, choice);
        t.detach();
    }

    // Cleanup (this part is never reached in the infinite loop)
    closesocket(server_sock);
    WSACleanup();
}

int main()
{
    server();
    return 0;
}
