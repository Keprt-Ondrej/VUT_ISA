
#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <iostream>
#include <ctime>
void print_time();

class comunication{
    public:
    comunication(std::string &ip,int port);
    ~comunication();
    int send_msg();
    

    private:
    std::string ip;
    int port;
    
};

#endif