
#ifndef __SERVER_H__
#define __SERVER_H__

#include "test.h"
#include <iostream>
int read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,int port);

#endif