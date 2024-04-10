#include "Global.h"

#if defined(_WIN32) || defined(_WIN64)
int mainServerTCPConnect()
{
    // ��ʼ�� Winsock ��
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return -1;
    }

    // �����׽���
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        perror("Failed to create socket");
        WSACleanup();
        return -1;
    }

    // ������������ַ
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); // ʹ��inet_pton����inet_addr
    server_addr.sin_port = htons(SERVER_PORT);

    // ���ӵ�������
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Failed to connect to server");
        closesocket(sockfd);
        WSACleanup();
        return -1;
    }



    return sockfd;
}
 



#elif defined(__linux__) || defined(__unix__) //Linuxƽ̨�¶���

int mainServerTCPConnect()
{
    // �����׽���  
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Failed to create socket");
        return -1;
    }

    // ������������ַ  
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // ʹ��inet_addr  
    server_addr.sin_port = htons(SERVER_PORT);

    // ���ӵ�������  
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Failed to connect to server");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

#endif


#if defined(_WIN32) || defined(_WIN64)
int sendAttackInfo(SOCKET sockfd, const char* attack_ip, const char* attack_type, int port)
{
    // ������Ϣ  
    char message[256];
    snprintf(message, sizeof(message), "Attack IP: %s, Attack Type: %s��Attack Port: %d\n", attack_ip, attack_type ,port);

    // ������Ϣ  
    if (send(sockfd, message, strlen(message), 0) == SOCKET_ERROR) {
        perror("Failed to send message");
        return -1;
    }

    // ���ͳɹ�������0  
    return 0;
}

#elif defined(__linux__) || defined(__unix__) //Linuxƽ̨�¶���

int sendAttackInfo(int sockfd, const char* attack_ip, const char* attack_type, int port)
{
    // ������Ϣ    
    char message[256];
    // ע�⣺�����޸��˶��ŵı��룬Ӧ����Ӣ�Ķ��Ŷ��������Ķ���  
    snprintf(message, sizeof(message), "Attack IP: %s, Attack Type: %s, Attack Port: %d\n", attack_ip, attack_type, port);

    // ������Ϣ    
    ssize_t sent_bytes = send(sockfd, message, strlen(message), 0);
    if (sent_bytes == -1) {
        perror("Failed to send message");
        return -1;
    }

    // ��Linux�£�send���ܷ��ز��ַ��͵��ֽ�������Ҫ����Ƿ�ȫ������  
    if ((size_t)sent_bytes != strlen(message)) {
        fprintf(stderr, "Failed to send complete message: %zd of %zu bytes sent\n", sent_bytes, strlen(message));
        return -1;
    }

    // ���ͳɹ�������0    
    return 0;
}

#endif