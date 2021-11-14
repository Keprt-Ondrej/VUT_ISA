#include "write_on_server.h"
#include "functions.h"
#include "comunication.h"

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
        unsigned long long total_data_send = 0;
        file = open_file(path,mode,'r');


        packet.create_request(TFTP_WRITE,path,mode,"45");
        /*
        packet.add_2B(TFTP_WRITE);
        packet.add_string(path.c_str());
        packet.add_string(mode.c_str());
        */
        print_time(); std::cout << "Write request send on server "<<ip << " port: "<<port<<"\n";     
        klient.send_msg(packet.size(),packet.buffer);
        packet.clear_buffer();    
        received_data = klient.receive_msg(packet.packet_size,packet.buffer);
        if (received_data == -1){
            print_time(); std::cout << "Server timed out .\n";
        }
        
        opcode = packet.get_2B();
        block_ack = packet.get_2B();
        if (opcode == TFTP_ACK && block_ack == 0){
            while (data_read >= TFTP_DEFAUL_BLOK_SIZE){
                packet.clear_buffer();
                packet.add_2B(TFTP_DATA);
                packet.add_2B(block_send);
                data_read = fread(packet.end_buffer,1,TFTP_DEFAUL_BLOK_SIZE,file);
                klient.send_msg(data_read+4,packet.buffer);   // 4 je velikost opcode + block
                received_data = klient.receive_msg(packet_ack.packet_size,packet_ack.buffer);
                opcode = packet_ack.get_2B();
                block_ack = packet_ack.get_2B();
                packet_ack.start_buffer();
                if (opcode == TFTP_ACK && block_ack == block_send){                  
                    block_send++;   
                    total_data_send += data_read;
                    print_time(); std::cout << "Sending " << data_read << "B of " << total_data_send << "B\n"; 
                }
                else{
                    //TODO nedosel ack
                    std::cerr << "here3\n";
                    for (int i = 0;i < 5;i++){
                        //klient.send_msg(data_read + 4,packet.buffer);
                        //klient.receive_msg
                    }
                }            
            }        
        }
        else{
            
        }
        print_time(); std::cout << "Transfer completed without errors\n";
        fclose(file);
    }
    catch(...){

    }
    
    return 0;
}