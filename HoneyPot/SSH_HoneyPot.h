#pragma once
#include "Global.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#if defined(_WIN32) || defined(_WIN64)//Windows平台下定义
DWORD WINAPI sshHoneyPotInit(LPVOID lpParameter);
#elif defined(__linux__) || defined(__unix__) //Linux平台下定义
void* sshHoneyPotInit(void* lpParameter);
#endif


