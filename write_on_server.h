
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <iostream>

int write_on_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port);

#endif