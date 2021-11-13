#include "read_from_server.h"
#include "functions.h"
#include "comunication.h"

int read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string & port){
    comunication klient(ip,port,timeout);
    klient.create_socket();
    packet_data packet(size);

    FILE *file = open_file(path,mode,'w');
    if (file == NULL){
        std::cerr << "Can not open file " << path << "\n";
        exit(42);
    }
    
    packet.create_request(TFTP_READ,path,mode);
    klient.send_msg(packet.size(),packet.buffer);

    int16_t block;
    int16_t recv_data_size = INT16_MAX;
    unsigned long total_received = 0;
    print_time(); std::cout << "Read request send on server "<<ip << " port: "<<port<<"\n";
    while(!(recv_data_size < packet.buffer_size)){
        recv_data_size = klient.receive_msg(packet.packet_size,packet.buffer);
        //std::cerr << recv_data_size << "\n";
        recv_data_size -= 4; 
        packet.start_buffer();
        int16_t opcode = packet.get_2B(); 
        //std::cerr << opcode << "\n";
        if (opcode == TFTP_DATA){
            block = packet.get_2B(); 
            total_received += recv_data_size;
            print_time(); std::cout << "Downloading " <<recv_data_size << "B, of " << total_received << "B\n"; 
            fwrite(packet.end_buffer,1,recv_data_size,file);
            packet.create_ACK(TFTP_ACK,block);
            klient.send_msg(packet.size(),packet.buffer);
        }
        else{
            std::cerr << "RECEIVED ERROR\n";
            return 42;
        }
    }

    print_time(); std::cout << "Transfer completed without errors\n";
    fclose(file);
   return 0;
}