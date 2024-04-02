#include "Global.h"
extern int telnetPort;
extern SOCKET MainServerSockfd;

#if defined(_WIN32) || defined(_WIN64)//Windowsƽ̨�¶���
DWORD WINAPI telHoneyPotInit(LPVOID lpParameter)
{
    int server_fd, new_client_fd;
    struct sockaddr_in server_address, client_address = {0};
    WORD wVersionRequested = {0};
    WSADATA ws;
    int result = WSAStartup(MAKEWORD(2, 2), &ws);
    if (result != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", result);
        
        return result; // ���ش������
    }
    // ���� socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Failed to create socket1\n");
        exit(-1);
    }

    memset(&server_address, '0', sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(telnetPort);   // TelnetĬ�϶˿ں�Ϊ23
    server_address.sin_addr.s_addr = INADDR_ANY;

    // �� socket �� IP��ַ�Ͷ˿ڽ��а�
    if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) 
    {
        fprintf(stderr, "Failed to bind socket1\n");
        exit(-1);
    }

    // ��ʼ������������
    if (listen(server_fd, 5) == -1) {
        
        fprintf(stderr, "Failed to listen for connections\n");
        exit(-1);
    }

    printf("Telnet Server is listening on port %d\n", ntohs(server_address.sin_port));
    printf("Telnet  thread started.\n");

    while (1) {
        char buffer[1024];

        // �ȴ��ͻ�������
        int client_len = sizeof(client_address);
        if ((new_client_fd = accept(server_fd, (struct sockaddr*)&client_address, (int*)&client_len)) == -1) {
            perror("Failed to accept connection from a client");
            continue;
        }


        //printf("Client connected with address: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Client connected with address: %s:%d\n", clientIP, ntohs(client_address.sin_port));
        sendAttackInfo(MainServerSockfd,clientIP, ntohs(client_address.sin_port));

        // ��ȡ�ͻ��˷��͵����ݲ���ӡ������̨��
        size_t bytesRead = recv(new_client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            printf("Received data from client:\n%s\n", buffer);

            // ��ͻ��˷�����ͬ������
            send(new_client_fd, buffer, strlen(buffer), 0);
        }
        else {
            fprintf(stderr, "Error reading data from client\n");
        }

        closesocket(new_client_fd);
    }

    WSACleanup();
    return 0;
}

#elif defined(__linux__) || defined(__unix__) //Linuxƽ̨�¶���
void telHoneyPotInit() {
    int server_fd, new_client_fd;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len;
    char buffer[MAX_BUFFER_SIZE];

    // ���� socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(telnetPort);   // TelnetĬ�϶˿ں�Ϊ23
    server_address.sin_addr.s_addr = INADDR_ANY;

    // �� socket �� IP��ַ�Ͷ˿ڽ��а�
    if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Failed to bind socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // ��ʼ������������
    if (listen(server_fd, 5) == -1) {
        perror("Failed to listen for connections");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Telnet Server is listening on port %d\n", telnetPort);
    printf("Telnet thread started.\n");

    while (1) {
        // �ȴ��ͻ�������
        client_len = sizeof(client_address);
        new_client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_len);
        if (new_client_fd == -1) {
            perror("Failed to accept connection from a client");
            continue;
        }

        // ��ȡ�ͻ��˵�ַ��Ϣ
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_address.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Client connected with address: %s:%d\n", clientIP, ntohs(client_address.sin_port));
        sendAttackInfo((clientIP), ntohs(client_address.sin_port));

        // ��ȡ�ͻ��˷��͵����ݲ���ӡ������̨��
        ssize_t bytesRead = recv(new_client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            printf("Received data from client:\n%s\n", buffer);

            // ��ͻ��˷�����ͬ������
            send(new_client_fd, buffer, strlen(buffer), 0);
        }
        else if (bytesRead == 0) {
            printf("Client disconnected\n");
        }
        else {
            perror("Error reading data from client");
        }

        close(new_client_fd);
    }

    close(server_fd);
}
#endif

//int telHoneyPotKilled(HANDLE thread)
//{ 
//    if (thread != NULL) {
//        WaitForSingleObject(thread, INFINITE);
//        CloseHandle(thread);
//        if (DEBUG) { printf("rdpThread Killed !\n"); }
//    }
//    if (DEBUG) { printf("rdpHoneyPot Killed !\n"); }
//    return 1;
//
//}


