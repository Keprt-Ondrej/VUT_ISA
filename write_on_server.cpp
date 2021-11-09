#include "write_on_server.h"
#include "functions.h"
#include "comunication.h"

int write_on_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port){
    std::cerr << "write_on_server\n"; 
    comunication klient(ip,port,timeout);
    klient.create_socket();
    packet_data packet;

    FILE *file = fopen(path.c_str(),"r"); 
    packet.create_request(TFTP_WRITE,path,mode);
    klient.send_msg(packet.size(),packet.buffer);
    int16_t send_data_size = INT16_MAX;
    //rcv ack TODO
    while(send_data_size < block_size){
        return 45;
    }

    return 0;
}