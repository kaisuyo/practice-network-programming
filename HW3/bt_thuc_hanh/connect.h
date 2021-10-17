#ifndef __CONNECT__
#define __CONNECT__

void get_ip(char *hostName, char *ip);
void get_hostName(char *ip, char *hostName);
int isHostName(char *hostName);
int isIp(char *ip);
int isIpV4(char *str);

#endif