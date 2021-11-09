#include "read_from_server.h"
#include "functions.h"
#include "comunication.h"

int read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string & port){
    std::cerr << "read_from server\n";
    comunication klient(ip,port,timeout);
    klient.create_socket();
    packet_data packet;

    FILE *file = fopen(path.c_str(),"w");    
    packet.create_request(TFTP_READ,path,mode);
    klient.send_msg(packet.size(),packet.buffer);

    int16_t block;
    int16_t recv_data_size = INT16_MAX;
    while(recv_data_size >= block_size){
        if (klient.receive_msg(packet.buffer_size,packet.buffer)){
            packet.start_buffer();
            int16_t opcode = packet.get_2B(); 
            std::cerr << opcode << "\n";
            if (opcode != TFTP_DATA){
                std::cerr << "RECEIVED ERROR\n";
                return 42;
            }
            block = packet.get_2B();
            recv_data_size = packet.size();
            std::cout << packet.end_buffer;
            fwrite(packet.end_buffer,1,packet.size(),file);
            klient.send_msg(packet.size(),packet.buffer);
        }
    }
    std::cout << "\n";

   return 0;
}