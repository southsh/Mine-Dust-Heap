#include "Global.h"
extern int rdpPort;
extern SOCKET MainServerSockfd;

#if defined(_WIN32) || defined(_WIN64)
DWORD WINAPI rdpHoneyPotInit(LPVOID lpParameter)
{
#if defined(_WIN32) || defined(_WIN64)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to load Winsock\n");
        return EXIT_FAILURE;
    }
#endif

    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr = {0};
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buf[MAX_BUFFER_SIZE] = "Server: hello, I am a server.....";

    // 创建套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Failed to create socket");
#if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    // 设置服务器地址结构
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(rdpPort);

    // 绑定套接字到端口
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Failed to bind socket");
        CLOSESOCKET(serverSocket);
#if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    // 监听传入的连接
    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        perror("Listen failed");
        CLOSESOCKET(serverSocket);
#if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    printf("RDP Server is listening on port %d\n", rdpPort);
    printf("RDP thread started.\n");

    // 接受传入的连接并处理
    while (1) {
        // 接受客户端连接
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            perror("Accept failed");
            break;
        }

        // 获取客户端地址信息
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Accepted connection from client IP: %s, Port: %d\n", clientIP, ntohs(clientAddr.sin_port));

        sendAttackInfo(MainServerSockfd,clientIP, ntohs(clientAddr.sin_port));
        // 发送数据给客户端
        if (send(clientSocket, buf, strlen(buf), 0) == SOCKET_ERROR) {
            perror("Send failed");
            CLOSESOCKET(clientSocket);
            continue;
        }

        // 接收客户端发送的数据
        char recvBuf[MAX_BUFFER_SIZE];
        memset(recvBuf, 0, sizeof(recvBuf));
        int bytesRead = recv(clientSocket, recvBuf, sizeof(recvBuf), 0);
        if (bytesRead == SOCKET_ERROR) {
            perror("Error reading data from client");
            CLOSESOCKET(clientSocket);
            continue;
        }
        else if (bytesRead == 0) {
            printf("Client disconnected\n");
            CLOSESOCKET(clientSocket);
            continue;
        }

        printf("Received data from client: %s\n", recvBuf);

        // 关闭客户端套接字
        CLOSESOCKET(clientSocket);
    }

    // 关闭服务器套接字
    CLOSESOCKET(serverSocket);

#if defined(_WIN32) || defined(_WIN64)
    WSACleanup();
#endif

    return EXIT_SUCCESS;

}

#elif defined(__linux__) || defined(__unix__)
void rdpHoneyPotInit() {
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen;
    char buf[MAX_BUFFER_SIZE] = "Server: hello, I am a server.....";

    // 创建套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址结构
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(rdpPort);

    // 绑定套接字到端口
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Failed to bind socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // 监听传入的连接
    if (listen(serverSocket, 10) == -1) {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("RDP Server is listening on port %d\n", rdpPort);
    printf("RDP thread started.\n");

    // 接受传入的连接并处理
    while (1) {
        // 接受客户端连接
        clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            break;
        }

        // 获取客户端地址信息
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Accepted connection from client IP: %s, Port: %d\n", clientIP, ntohs(clientAddr.sin_port));
        sendAttackInfo((clientIP), ntohs(clientAddr.sin_port));

        // 发送数据给客户端
        if (send(clientSocket, buf, strlen(buf), 0) == -1) {
            perror("Send failed");
            close(clientSocket);
            continue;
        }

        // 接收客户端发送的数据
        char recvBuf[MAX_BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, recvBuf, sizeof(recvBuf), 0);
        if (bytesRead == -1) {
            perror("Error reading data from client");
            close(clientSocket);
            continue;
        }
        else if (bytesRead == 0) {
            printf("Client disconnected\n");
            close(clientSocket);
            continue;
        }

        printf("Received data from client: %s\n", recvBuf);

        // 关闭客户端套接字
        close(clientSocket);
    }

    // 关闭服务器套接字
    close(serverSocket);
}
#endif
//int rdpHoneyPotKilled(HANDLE thread)
//{
//    if (thread != NULL) {
//        WaitForSingleObject(thread, INFINITE);
//        CloseHandle(thread);
//        if (DEBUG) { printf("rdpThread Killed !\n"); }
//    }
//    if (DEBUG) { printf("rdpHoneyPot Killed !\n"); }
//    return 1;
//}


