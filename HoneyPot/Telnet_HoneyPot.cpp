#include "Global.h"
extern int telnetPort;
extern SOCKET MainServerSockfd;

#if defined(_WIN32) || defined(_WIN64)//Windowsƽ̨�¶���
DWORD WINAPI telHoneyPotInit(LPVOID lpParameter) {
    int server_fd, new_client_fd;
    struct sockaddr_in server_address, client_address;
    int client_len;
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA ws;
    int result = WSAStartup(wVersionRequested, &ws);
    if (result != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", result);
        return result; // ���ش������  
    }

    // ���� socket  
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Failed to create socket\n");
        WSACleanup();
        return -1;
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(telnetPort);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // �� socket �� IP��ַ�Ͷ˿ڽ��а�  
    if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        fprintf(stderr, "Failed to bind socket\n");
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    // ��ʼ������������  
    if (listen(server_fd, 5) == -1) {
        fprintf(stderr, "Failed to listen for connections\n");
        closesocket(server_fd);
        WSACleanup();
        return -1;
    }

    printf("Telnet Server is listening on port %d\n", ntohs(server_address.sin_port));
    printf("Telnet thread started.\n");

    while (1) {
        char clientIP[INET_ADDRSTRLEN];
        client_len = sizeof(client_address);

        // �ȴ��ͻ�������  
        if ((new_client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_len)) == -1) {
            perror("Failed to accept connection from a client");
            continue;
        }

        // ��¼�ͻ��˵�IP�Ͷ˿ں�  
        inet_ntop(AF_INET, &(client_address.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Telnet Client connected with address: %s:%d\n", clientIP, ntohs(client_address.sin_port));

        const char* telnt = "Telnet";
        sendAttackInfo(MainServerSockfd, clientIP, "Telnet",ntohs(client_address.sin_port));


        // �Զ��Ͽ���ͻ��˵�����  
        closesocket(new_client_fd);
        printf("Connection with client closed.\n");
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}

#elif defined(__linux__) || defined(__unix__) //Linuxƽ̨�¶���

void* telHoneyPotInit(void* lpParameter) {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buf[MAX_BUFFER_SIZE] = "Server: hello, I am a server.....\n";

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Telnet Failed to create socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(telnetPort);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Telnet Failed to bind socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 10) < 0) {
        perror("Telnet Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Telnet Server is listening on port %d\n", telnetPort);
    printf("Telnet thread started.\n");

    while (1) {
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            perror("TelnetPort Accept failed");
            break;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        unsigned short clientPort = ntohs(clientAddr.sin_port);
        printf("TelnetPort Accepted connection from client IP: %s, Port: %d\n", clientIP, clientPort);

        sendAttackInfo(MainServerSockfd, clientIP, "Telnet", clientPort);

        if (write(clientSocket, buf, strlen(buf)) < 0) {
            perror("Send failed");
        }

        close(clientSocket); // �رտͻ�������  
    }

    close(serverSocket); // �رշ�����socket  
    return NULL;
}

#endif



