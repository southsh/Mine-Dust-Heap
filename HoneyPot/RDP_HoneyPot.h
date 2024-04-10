#pragma once
#include "Global.h"
#if defined(_WIN32) || defined(_WIN64)
DWORD WINAPI rdpHoneyPotInit(LPVOID lpParameter);
#elif defined(__linux__) || defined(__unix__)
void* rdpHoneyPotInit(void* lpParameter);
#endif





