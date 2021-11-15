
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "comunication.h"
#include <sys/stat.h>
#include <iostream>

void write_on_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port);
void send_data(comunication &klient,packet_data &packet,packet_data &packet_ack,FILE *file, std::string &file_size);

#endif