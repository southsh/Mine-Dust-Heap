#pragma once
#include "Global.h"

#if defined(_WIN32) || defined(_WIN64)//Windows平台下定义
DWORD WINAPI telHoneyPotInit(LPVOID lpParameter);
#elif defined(__linux__) || defined(__unix__) //Linux平台下定义
void* telHoneyPotInit(void* lpParameter);
#endif



