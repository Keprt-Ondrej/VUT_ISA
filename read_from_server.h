
#ifndef __SERVER_H__
#define __SERVER_H__


#include <iostream>
#include <stdexcept>

int read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port);

#endif