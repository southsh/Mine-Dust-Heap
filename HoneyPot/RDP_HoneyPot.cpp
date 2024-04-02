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

    // �����׽���
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Failed to create socket");
#if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    // ���÷�������ַ�ṹ
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(rdpPort);

    // ���׽��ֵ��˿�
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Failed to bind socket");
        CLOSESOCKET(serverSocket);
#if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
#endif
        return EXIT_FAILURE;
    }

    // �������������
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

    // ���ܴ�������Ӳ�����
    while (1) {
        // ���ܿͻ�������
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            perror("Accept failed");
            break;
        }

        // ��ȡ�ͻ��˵�ַ��Ϣ
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Accepted connection from client IP: %s, Port: %d\n", clientIP, ntohs(clientAddr.sin_port));

        sendAttackInfo(MainServerSockfd,clientIP, ntohs(clientAddr.sin_port));
        // �������ݸ��ͻ���
        if (send(clientSocket, buf, strlen(buf), 0) == SOCKET_ERROR) {
            perror("Send failed");
            CLOSESOCKET(clientSocket);
            continue;
        }

        // ���տͻ��˷��͵�����
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

        // �رտͻ����׽���
        CLOSESOCKET(clientSocket);
    }

    // �رշ������׽���
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

    // �����׽���
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // ���÷�������ַ�ṹ
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(rdpPort);

    // ���׽��ֵ��˿�
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Failed to bind socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // �������������
    if (listen(serverSocket, 10) == -1) {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("RDP Server is listening on port %d\n", rdpPort);
    printf("RDP thread started.\n");

    // ���ܴ�������Ӳ�����
    while (1) {
        // ���ܿͻ�������
        clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            break;
        }

        // ��ȡ�ͻ��˵�ַ��Ϣ
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Accepted connection from client IP: %s, Port: %d\n", clientIP, ntohs(clientAddr.sin_port));
        sendAttackInfo((clientIP), ntohs(clientAddr.sin_port));

        // �������ݸ��ͻ���
        if (send(clientSocket, buf, strlen(buf), 0) == -1) {
            perror("Send failed");
            close(clientSocket);
            continue;
        }

        // ���տͻ��˷��͵�����
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

        // �رտͻ����׽���
        close(clientSocket);
    }

    // �رշ������׽���
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


