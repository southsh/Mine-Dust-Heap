#include "Global.h"


int sshPort = 0, telnetPort = 0, rdpPort = 0;
SOCKET MainServerSockfd;//��������������������    ע�⣬�ⶫ������Ϊȫ�ֱ��������̲߳���ȫ��Ϊ����Ҫ���������Ͳ��ٸı�

void errorWithUsage(const char* errorMsg);//��������
static void usage(FILE* stream, int exit_code);//�۹�ʹ���ֲ�


int startHoneyPots(bool enableSSH, bool enableTelnet, bool enableRDP); 


int main(int argc, char* argv[]) {
#ifndef NDEBUG
    // Debugģʽ�²�ִ���κβ���
#else
    // Releaseģʽ�����ؿ���̨����
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
#endif


    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-s ������Ҫָ���˿ں�\n");
                exit(EXIT_FAILURE);
            }
            sshPort = atoi(argv[++i]);
            
        }
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-t ������Ҫָ���˿ں�\n");
                exit(EXIT_FAILURE);
            }
            telnetPort = atoi(argv[++i]);
            
        }
        else if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-r ������Ҫָ���˿ں�\n");
                exit(EXIT_FAILURE);
            }
            rdpPort = atoi(argv[++i]);
            
        }
        else if (strcmp(argv[i], "-h") == 0) {

            usage(stderr,0);
        }
        else {
            fprintf(stderr, "δ֪������%s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }


    if (DEBUG) { printf("sshPort = %d,telnetPort = %d rdpPort = %d\n", sshPort, telnetPort, rdpPort); }
    //�����DEBUG����config.h�����Լ����õ�
    MainServerSockfd = mainServerTCPConnect();
    
    printf("MainServerSockfd = %lld\n", MainServerSockfd);
    if(MainServerSockfd == -1)
    {
        fprintf(stderr, "������������ʧ��\n");
        //����û��ֱ���Ƴ�����ѡ����������۹ޣ������޷��ϱ�������Ϣ������������
    }


    startHoneyPots(sshPort, telnetPort, rdpPort);
    
    return 0;
}












#if defined(_WIN32) || defined(_WIN64)//Windowsƽ̨�¶���
int startHoneyPots(bool enableSSH, bool enableTelnet, bool enableRDP)
{
    HANDLE threads[3] = { 0 };

    // ���� SSH �߳�
    threads[0] = CreateThread(NULL, 0, sshHoneyPotInit, NULL, 0, NULL);
    if (threads[0] == NULL) {
        fprintf(stderr, "Failed to create SSH thread.\n");
        return 1;
    }

    // ���� RDP �߳�
    threads[1] = CreateThread(NULL, 0, rdpHoneyPotInit, NULL, 0, NULL);
    if (threads[1] == NULL) {
        fprintf(stderr, "Failed to create RDP thread.\n");
        CloseHandle(threads[0]);
        return 1;
    }

    // ���� Telnet �߳�
    threads[2] = CreateThread(NULL, 0, telHoneyPotInit, NULL, 0, NULL);
    if (threads[2] == NULL) {
        fprintf(stderr, "Failed to create Telnet thread.\n");
        CloseHandle(threads[0]);
        CloseHandle(threads[1]);
        return 1;
    }

    // �ȴ������߳����
    WaitForMultipleObjects(3, threads, TRUE, INFINITE);

    // �ر������߳̾��
    for (int i = 0; i < 3; ++i) {
        CloseHandle(threads[i]);
    }

    printf("All threads completed.\n");

    return 0;
}


#elif defined(__linux__) || defined(__unix__) //Linuxƽ̨�¶���
int startHoneyPots(bool enableSSH, bool enableTelnet, bool enableRDP) {
    pthread_t threads[3] = { 0 };

    // ���� SSH �߳�
    if (enableSSH) {
        if (pthread_create(&threads[0], NULL, reinterpret_cast<void* (*)(void*)>(sshHoneyPotInit), NULL) != 0) {
            fprintf(stderr, "Failed to create SSH thread.\n");
            return 1;
        }
    }

    // ���� RDP �߳�
    if (enableRDP) {
        if (pthread_create(&threads[1], NULL, reinterpret_cast<void* (*)(void*)>(rdpHoneyPotInit), NULL) != 0) {
            fprintf(stderr, "Failed to create RDP thread.\n");
            if (enableSSH) pthread_cancel(threads[0]);
            return 1;
        }
    }

    // ���� Telnet �߳�
    if (enableTelnet) {
        if (pthread_create(&threads[2], NULL, reinterpret_cast<void* (*)(void*)>(telHoneyPotInit), NULL) != 0) {
            fprintf(stderr, "Failed to create Telnet thread.\n");
            if (enableSSH) pthread_cancel(threads[0]);
            if (enableRDP) pthread_cancel(threads[1]);
            return 1;
        }
    }

    // �ȴ������߳����
    for (int i = 0; i < 3; ++i) {
        if (threads[i] != 0) {
            pthread_join(threads[i], NULL);
        }
    }

    printf("All threads completed.\n");

    return 0;
}



#endif


static void usage(FILE* stream, int exit_code) {
    fprintf(stream, "Usage: sshpot [-h] [-s <port>] [-t <port>] [-r <port>]\n");
    fprintf(stream,
        "   -h  --help          Display this usage information.\n"
        "   -s  --port <port>   SSH Honeypot��  Port to listen on; defaults to 22.\n"
        "   -t  --port <port>   Telnet Honeypot Port to listen on; defaults to 23.\n"
        "   -r  --port <port>   RDP Honeypot    Port to listen on; defaults to 3389.\n");
    exit(exit_code);
}

void errorWithUsage(const char* errorMsg) {
    fprintf(stderr, "%s\n", errorMsg);
    // ��ʾuseage
    usage(stderr, 0);
    exit(-1);
}
