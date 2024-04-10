#pragma once
#include "Global.h"


int mainServerTCPConnect();

int sendAttackInfo(SOCKET sockfd, const char* attack_ip, const char* attack_type, int port);