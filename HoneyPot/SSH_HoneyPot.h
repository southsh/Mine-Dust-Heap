#pragma once
#include "Global.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#if defined(_WIN32) || defined(_WIN64)//Windowsƽ̨�¶���
DWORD WINAPI sshHoneyPotInit(LPVOID lpParameter);
#elif defined(__linux__) || defined(__unix__) //Linuxƽ̨�¶���
void* sshHoneyPotInit(void* lpParameter);
#endif


