#pragma once
#include "Global.h"

#if defined(_WIN32) || defined(_WIN64)//Windowsƽ̨�¶���
DWORD WINAPI telHoneyPotInit(LPVOID lpParameter);
#elif defined(__linux__) || defined(__unix__) //Linuxƽ̨�¶���
void* telHoneyPotInit(void* lpParameter);
#endif



