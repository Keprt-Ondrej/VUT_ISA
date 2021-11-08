#include "write_on_server.h"
#include "functions.h"
#include "comunication.h"

int write_on_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port){
    std::cerr << "write_on_server\n"; 
    comunication klient(ip,port);
    klient.create_socket();
    packet_data packet;

    packet.create_request(TFTP_WRITE,path,mode);
    klient.send_msg(packet.size(),packet.buffer);

    if (klient.receive_msg(packet.buffer_size,packet.buffer)){
        std::cout << packet.buffer+4 << "\n";
    }

    return 0;
}