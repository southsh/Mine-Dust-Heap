#include "Global.h"

#if defined(_WIN32) || defined(_WIN64)
int mainServerTCPConnect()
{
    // 初始化 Winsock 库
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return -1;
    }

    // 创建套接字
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        perror("Failed to create socket");
        WSACleanup();
        return -1;
    }

    // 构建服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); // 使用inet_pton代替inet_addr
    server_addr.sin_port = htons(SERVER_PORT);

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Failed to connect to server");
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }

    //// 构建消息
    //char message[256];
    //snprintf(message, sizeof(message), "Attack IP: %s, Attack Type: %d\n", attackIP, attackType);

    //// 发送消息
    //if (send(sockfd, message, strlen(message), 0) == SOCKET_ERROR) {
    //    perror("Failed to send message");
    //    closesocket(sockfd);
    //    WSACleanup();
    //    return -1;
    //}

    //// 关闭套接字
    //closesocket(sockfd);
    //WSACleanup();

    return sockfd;
}
 



#elif defined(__linux__) || defined(__unix__) //Linux平台下定义
int mainServerTCPConnect(const char* attackIP,int attackType)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // 构建服务器地址
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect to server");
        exit(EXIT_FAILURE);
    }

    //// 构建消息
    //char message[256];
    //snprintf(message, sizeof(message), "Attack IP: %s, Attack Type: %d\n", attackIP, attackType);

    //// 发送消息
    //if (send(sockfd, message, strlen(message), 0) < 0) {
    //    perror("Failed to send message");
    //    exit(EXIT_FAILURE);
    //}

    //// 关闭套接字
    //close(sockfd);

    return sockfd;

}

#endif



int sendAttackInfo(SOCKET sockfd, const char* attack_ip, int attack_type) {
    // 构建消息  
    char message[256];
    snprintf(message, sizeof(message), "Attack IP: %s, Attack Type: %d\n", attack_ip, attack_type);

    // 发送消息  
    if (send(sockfd, message, strlen(message), 0) == SOCKET_ERROR) {
        perror("Failed to send message");
        return -1;
    }

    // 发送成功，返回0  
    return 0;
}
