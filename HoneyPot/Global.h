#pragma once

#if defined(_WIN32) || defined(_WIN64)//Windowsƽ̨�¶���

#define PLATFORM_WINDOWS    //��Ϊһ������ʱ��ǣ�����ָʾ��������Windowsƽ̨�ϱ���
#include <Winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")  

#define CLOSESOCKET closesocket

#elif defined(__linux__) || defined(__unix__) //Linuxƽ̨�¶���
#define PLATFORM_UNIX
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define CLOSESOCKET close


#else//������
#error "Unsupported platform!"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "SSH_HoneyPot.h"
#include "RDP_HoneyPot.h"
#include "Telnet_HoneyPot.h"
#include "config.h"
#include "AttackHandle.h"
#include "Global.h"

