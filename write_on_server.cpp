#include "write_on_server.h"
#include "functions.h"
#include "comunication.h"
#include <sys/stat.h>

int send_data(comunication &klient,packet_data &packet,packet_data &packet_ack,FILE *file, std::string &file_size){
    int16_t block_send = 1;
    int16_t block_ack;
    int16_t opcode;
    ssize_t received_data;
    ssize_t data_read = SSIZE_MAX; // pocet dat, co byla prectena 
    unsigned long long total_data_send = 0;
    
    while (!(data_read < packet.buffer_size)){
        packet.clear_buffer();
        packet.add_2B(TFTP_DATA);
        packet.add_2B(block_send);
        data_read = fread(packet.end_buffer,1,packet.buffer_size,file);
        klient.send_msg(data_read+4,packet.buffer);   // 4 je velikost opcode + block
        received_data = klient.receive_msg(packet_ack.packet_size,packet_ack.buffer);
        opcode = packet_ack.get_2B();
        block_ack = packet_ack.get_2B();
        packet_ack.start_buffer();
        if (opcode == TFTP_ACK && block_ack == block_send){                  
            block_send++;   
            total_data_send += data_read;
            print_time(); std::cout << "Sending " << data_read << "B of " << file_size << "B\n"; 
        }
        else if(opcode == TFTP_ERR) {
           print_time(); std::cerr << "Error " << packet_ack.get_2B()<< ": " << packet_ack.get_string()<< " transfer terminated\n";
           throw std::exception();
        }
        else{

        }            
    }
    return 0;
}

int write_on_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port){
    FILE *file;
    try{
        comunication klient(ip,port,DEFAULT_TIMEOUT);
        klient.create_socket();
        packet_data packet(size);
        packet_data packet_ack(TFTP_DEFAUL_BLOK_SIZE); // na ack packety neni potreba vice

        int16_t block_send = 1;
        int16_t block_ack;
        int16_t opcode;
        ssize_t received_data;
        ssize_t data_read = SSIZE_MAX; // pocet dat, co byla prectena         
        file = open_file(path,mode,'r');
        fseek(file, 0, SEEK_END); // seek to end of file
        ssize_t file_size = ftell(file); // get current file pointer
        fseek(file, 0, SEEK_SET); // seek back to beginning of 
        std::string file_size_str = std::to_string(file_size);
        //std::cerr << file_size_str << "\t "<< "\n";

        packet.create_request(TFTP_WRITE,path,mode,(char *)file_size_str.c_str());
        packet.option_setup(size,timeout);
        print_time(); std::cout << "Write request send on server "<<ip << " port: "<<port<<"\n";     
        klient.send_msg(packet.size(),packet.buffer);
        packet.clear_buffer();    
        received_data = klient.receive_msg(packet.packet_size,packet.buffer);                
        opcode = packet.get_2B();
        //block_ack = packet.get_2B();
        if(opcode == TFTP_OPTION_ACK){
            std::string{packet.get_string()};  //potrebuji zahodit option specifier tsize
            std::string{packet.get_string()};  //potrebuji zahodit value tsize
            packet.OACK_option_handler_blksize(size);
            packet.OACK_option_handler_timeout(timeout);
            send_data(klient,packet,packet_ack,file,file_size_str);
        }
        else if (opcode == TFTP_ACK){
            //server nepodporuje options, musim nastavit buffer zpet na default velikost
            print_time(); std::cerr << "Server refused options, resume transfer by RFC 1350 specification, http://tools.ietf.org/html/rfc1350\n";
            packet.change_buffer(TFTP_DEFAUL_BLOK_SIZE);
            send_data(klient,packet,packet_ack,file,file_size_str);  
        }
        else{
           print_time(); std::cerr << "Error " << packet.get_2B()<< ": " << packet.get_string()<< " transfer terminated\n";
           throw std::exception();
        }

        print_time(); std::cout << "Transfer completed without errors\n";
        fclose(file);
    }
    catch(...){
        fclose(file);
        return 42;
    }
    return 0;
}