#include "Global.h"
extern int sshPort;
extern SOCKET MainServerSockfd;

#if defined(_WIN32) || defined(_WIN64)//Windows平台下定义

DWORD WINAPI sshHoneyPotInit(LPVOID lpParameter) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to load Winsock\n");
        return EXIT_FAILURE;
    }

    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buf[MAX_BUFFER_SIZE] = "Server: hello, I am a server.....";

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Failed to create socket");
        WSACleanup();
        return EXIT_FAILURE;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(sshPort);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Failed to bind socket");
        CLOSESOCKET(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        perror("Listen failed");
        CLOSESOCKET(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("SSH Server is listening on port %d\n", sshPort);
    printf("SSH thread started.\n");

    while (1) {
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            perror("Accept failed");
            break;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        unsigned short clientPort = ntohs(clientAddr.sin_port);
        printf("SSH Accepted connection from client IP: %s, Port: %d\n", clientIP, clientPort);

        sendAttackInfo(MainServerSockfd, clientIP, "SSH", clientPort);

        // 发送数据给客户端  
        if (send(clientSocket, buf, strlen(buf), 0) == SOCKET_ERROR) {
            perror("Send failed");
        }

        // 自动断开连接  
        CLOSESOCKET(clientSocket);
    }

    CLOSESOCKET(serverSocket);
    WSACleanup();
    return EXIT_SUCCESS;
}

#elif defined(__linux__) || defined(__unix__) //Linux平台下定义

void* sshHoneyPotInit(void* lpParameter) {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buf[MAX_BUFFER_SIZE] = "Server: hello, I am a server.....\n";

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("SSH Failed to create socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(sshPort);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("SSH Failed to bind socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 10) < 0) {
        perror("SSH Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("SSH Server is listening on port %d\n", sshPort);
    printf("SSH thread started.\n");

    while (1) {
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            perror("SSH Accept failed");
            break;
        }

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        unsigned short clientPort = ntohs(clientAddr.sin_port);
        printf("SSH Accepted connection from client IP: %s, Port: %d\n", clientIP, clientPort);

        sendAttackInfo(MainServerSockfd, clientIP, "SSH", clientPort);

        if (write(clientSocket, buf, strlen(buf)) < 0) {
            perror("Send failed");
        }

        close(clientSocket); // 关闭客户端连接  
    }

    close(serverSocket); // 关闭服务器socket  
    return NULL;
}

#endif





