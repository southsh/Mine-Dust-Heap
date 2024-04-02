#include "Global.h"


int sshPort = 0, telnetPort = 0, rdpPort = 0;
SOCKET MainServerSockfd;//启动与主服务器的连接    注意，这东西设置为全局变量属于线程不安全行为，需要定义下来就不再改变

void errorWithUsage(const char* errorMsg);//错误处理函数
static void usage(FILE* stream, int exit_code);//蜜罐使用手册


int startHoneyPots(bool enableSSH, bool enableTelnet, bool enableRDP); 


int main(int argc, char* argv[]) {
#ifndef NDEBUG
    // Debug模式下不执行任何操作
#else
    // Release模式下隐藏控制台窗口
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
#endif


    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-s 参数需要指定端口号\n");
                exit(EXIT_FAILURE);
            }
            sshPort = atoi(argv[++i]);
            
        }
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-t 参数需要指定端口号\n");
                exit(EXIT_FAILURE);
            }
            telnetPort = atoi(argv[++i]);
            
        }
        else if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "-r 参数需要指定端口号\n");
                exit(EXIT_FAILURE);
            }
            rdpPort = atoi(argv[++i]);
            
        }
        else if (strcmp(argv[i], "-h") == 0) {

            usage(stderr,0);
        }
        else {
            fprintf(stderr, "未知参数：%s\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }


    if (DEBUG) { printf("sshPort = %d,telnetPort = %d rdpPort = %d\n", sshPort, telnetPort, rdpPort); }
    //这里的DEBUG是在config.h里面自己设置的
    MainServerSockfd = mainServerTCPConnect();
    
    printf("MainServerSockfd = %lld\n", MainServerSockfd);
    if(MainServerSockfd == -1)
    {
        fprintf(stderr, "连接主服务器失败\n");
        //这里没有直接推出，是选择继续开启蜜罐，但是无法上报攻击信息到主服务器。
    }


    startHoneyPots(sshPort, telnetPort, rdpPort);
    
    return 0;
}












#if defined(_WIN32) || defined(_WIN64)//Windows平台下定义
int startHoneyPots(bool enableSSH, bool enableTelnet, bool enableRDP)
{
    HANDLE threads[3] = { 0 };

    // 创建 SSH 线程
    threads[0] = CreateThread(NULL, 0, sshHoneyPotInit, NULL, 0, NULL);
    if (threads[0] == NULL) {
        fprintf(stderr, "Failed to create SSH thread.\n");
        return 1;
    }

    // 创建 RDP 线程
    threads[1] = CreateThread(NULL, 0, rdpHoneyPotInit, NULL, 0, NULL);
    if (threads[1] == NULL) {
        fprintf(stderr, "Failed to create RDP thread.\n");
        CloseHandle(threads[0]);
        return 1;
    }

    // 创建 Telnet 线程
    threads[2] = CreateThread(NULL, 0, telHoneyPotInit, NULL, 0, NULL);
    if (threads[2] == NULL) {
        fprintf(stderr, "Failed to create Telnet thread.\n");
        CloseHandle(threads[0]);
        CloseHandle(threads[1]);
        return 1;
    }

    // 等待所有线程完成
    WaitForMultipleObjects(3, threads, TRUE, INFINITE);

    // 关闭所有线程句柄
    for (int i = 0; i < 3; ++i) {
        CloseHandle(threads[i]);
    }

    printf("All threads completed.\n");

    return 0;
}


#elif defined(__linux__) || defined(__unix__) //Linux平台下定义
int startHoneyPots(bool enableSSH, bool enableTelnet, bool enableRDP) {
    pthread_t threads[3] = { 0 };

    // 创建 SSH 线程
    if (enableSSH) {
        if (pthread_create(&threads[0], NULL, reinterpret_cast<void* (*)(void*)>(sshHoneyPotInit), NULL) != 0) {
            fprintf(stderr, "Failed to create SSH thread.\n");
            return 1;
        }
    }

    // 创建 RDP 线程
    if (enableRDP) {
        if (pthread_create(&threads[1], NULL, reinterpret_cast<void* (*)(void*)>(rdpHoneyPotInit), NULL) != 0) {
            fprintf(stderr, "Failed to create RDP thread.\n");
            if (enableSSH) pthread_cancel(threads[0]);
            return 1;
        }
    }

    // 创建 Telnet 线程
    if (enableTelnet) {
        if (pthread_create(&threads[2], NULL, reinterpret_cast<void* (*)(void*)>(telHoneyPotInit), NULL) != 0) {
            fprintf(stderr, "Failed to create Telnet thread.\n");
            if (enableSSH) pthread_cancel(threads[0]);
            if (enableRDP) pthread_cancel(threads[1]);
            return 1;
        }
    }

    // 等待所有线程完成
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
        "   -s  --port <port>   SSH Honeypot，  Port to listen on; defaults to 22.\n"
        "   -t  --port <port>   Telnet Honeypot Port to listen on; defaults to 23.\n"
        "   -r  --port <port>   RDP Honeypot    Port to listen on; defaults to 3389.\n");
    exit(exit_code);
}

void errorWithUsage(const char* errorMsg) {
    fprintf(stderr, "%s\n", errorMsg);
    // 显示useage
    usage(stderr, 0);
    exit(-1);
}
