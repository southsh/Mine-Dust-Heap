#pragma once
#include "Global.h"


int mainServerTCPConnect();

int sendAttackInfo(SOCKET sockfd, const char* attack_ip, int attack_type);