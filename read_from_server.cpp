#include "read_from_server.h"
#include "functions.h"
#include "comunication.h"

int read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string & port){
    std::cerr << "read_from server\n";
    comunication klient(ip,port);
    klient.create_socket();
    packet_data packet;

    packet.create_request(TFTP_READ,path,mode);
    klient.send_msg(packet.size(),packet.buffer);

    int16_t block;
    //while(1){}

    if (klient.receive_msg(packet.buffer_size,packet.buffer)){
        packet.start_buffer();
        int16_t opcode = packet.get_2B(); 
        std::cerr << opcode << "\n";
        if (opcode != TFTP_DATA){
            std::cerr << "RECEIVED ERROR\n";
            return 42;
        }
        block = packet.get_2B();
        std::cout << packet.end_buffer;
    }
    std::cout << "\n";

   return 0;
}