
#ifndef __SERVER_H__
#define __SERVER_H__


#include <iostream>
#include <stdexcept>
#include "functions.h"
#include "comunication.h"
#include <sys/statvfs.h>
#include <exception>
#include <string>
#include <cstdlib>
#include <sys/ioctl.h>
#include <net/if.h>

int read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port);

#endif