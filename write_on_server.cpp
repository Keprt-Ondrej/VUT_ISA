/**
 * @file write_on_server.cpp
 * @brief Posilani dat na server
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#include "write_on_server.h"

void send_data(comunication &klient,packet_data &packet,packet_data &packet_ack,FILE *file, std::string &file_size){
    int resend_counter = 0; //pocet kolikrat jsem dana dataq odeslal
    int16_t block_send = 1;
    int16_t block_ack;
    int16_t opcode;
    ssize_t received_data;
    ssize_t data_read = SSIZE_MAX; // pocet dat, co byla prectena 
    unsigned long long total_data_send = 0;
    
    while (!(data_read < packet.buffer_size)){ //dokud neni velikost odeslanych dat mensi jak velikost bloku, odesilej data
        if(resend_counter == 0){ 
            //pokud se podarilo data odeslat, nacti dalsi, jinak posli znovu
            packet.clear_buffer();
            packet.add_2B(TFTP_DATA);
            packet.add_2B(block_send);
            data_read = fread(packet.end_buffer,1,packet.buffer_size,file);
        }
        //odeslani dat
        klient.send_msg(data_read+4,packet.buffer);   // 4 je velikost opcode + block
        print_time(); std::cout << "Sending " << data_read << "B of " << file_size << "B to: "<< klient.ip << " port: "<< klient.port<<"\n";
        try{    //chytam timeout z receive_msg
            received_data = klient.receive_msg(packet_ack.packet_size,packet_ack.buffer);
            opcode = packet_ack.get_2B();
            block_ack = packet_ack.get_2B();
        }
        catch(...){//zachyceni timeoutu
            opcode = 42; // abych skocil na resend
        } 
        packet_ack.start_buffer();
        if (opcode == TFTP_ACK && block_ack == block_send){  
            resend_counter = 0; //nastaveni, ze prisel spravny ack, aby se prestal resendovat dany packet               
            block_send++;   
            total_data_send += data_read; 
        }
        else if(opcode == TFTP_ERR) {
           print_time(); std::cerr << "Error " << packet_ack.get_2B()<< ": " << packet_ack.get_string()<< " transfer terminated\n";
           throw std::exception();
        }
        else{ //resend
            //dosel spatny ack_block/doslo k timeotu,posilam packet znovu
            resend_counter++;            
            if(resend_counter == TIMEOT_CNT){
                print_time(); std::cerr << "Error, server is not responding, transfer terminated\n";
                throw std::exception();
            }
            print_time(); std::cerr << "Server is not responding, trying resend data\n";
        }            
    }
    return;
}

void write_on_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port){
    FILE *file;
    try{
        //inicializace socketu a alokace pameti pro buffery
        comunication klient(ip,port,timeout,false); //nastavi timeout zadany, uzivatelem nebo defaultni, pokud server odmitne uzivateluv timeout, ve funkci OACK_option_handler_timeout nastavi timeout klienta na defaultni velikost
        klient.create_socket();
        packet_data packet(size);
        packet_data packet_ack(TFTP_DEFAUL_BLOK_SIZE); // na ack packety neni potreba vice

        int16_t block_send = 1;
        int16_t block_ack;
        int16_t opcode;
        ssize_t received_data;
        ssize_t data_read = SSIZE_MAX; // pocet dat, co byla prectena

        //otevreni souboru
        file = open_file(path,mode,'r');
        if(file == NULL){
            print_time(); std::cerr << "could not open file: "<< path << "\n";
            return;
        }

        //ziskani velikosti souboru
        fseek(file, 0, SEEK_END); 
        ssize_t file_size = ftell(file); 
        fseek(file, 0, SEEK_SET); 
        std::string file_size_str = std::to_string(file_size);

        //vytvoreni pozadavku a pridani voleb uzivatele, pokud je zadal
        packet.create_request(TFTP_WRITE,path,mode,(char *)file_size_str.c_str());
        packet.option_setup(size,timeout);

        //odeslani pozadavku
        print_time(); std::cout << "Write request send on server "<<ip << " port: "<<port<< " requested file: "<<path<<"\n";     
        klient.send_msg(packet.size(),packet.buffer);
        packet.clear_buffer();    

        //prijem dat
        received_data = klient.receive_msg(packet.packet_size,packet.buffer);                
        opcode = packet.get_2B();
        if(opcode == TFTP_OPTION_ACK){
            std::string{packet.get_string()};  //potrebuji zahodit option specifier tsize
            std::string{packet.get_string()};  //potrebuji zahodit value tsize

            //zpracovani odpovedi na volby
            packet.OACK_option_handler_blksize(size);
            packet.OACK_option_handler_timeout(klient,timeout);

            //zasilani dat
            send_data(klient,packet,packet_ack,file,file_size_str);
        }
        else if (opcode == TFTP_ACK){
            //server nepodporuje options, musim nastavit buffer zpet na default velikost
            print_time(); std::cerr << "Server refused options, resume transfer by RFC 1350 specification, http://tools.ietf.org/html/rfc1350\n";

            //pro spravnou funkci musim zmenit velikost bufferu na velikost definovanou v RFC 1350
            packet.change_buffer(TFTP_DEFAUL_BLOK_SIZE); 
            send_data(klient,packet,packet_ack,file,file_size_str);  
        }
        else{
            //nastala chyba na strane serveru
           print_time(); std::cerr << "Error " << packet.get_2B()<< ": " << packet.get_string()<< " transfer terminated\n";
           throw std::exception();
        }

        print_time(); std::cout << "Transfer completed without errors\n";
        fclose(file);
    }
    catch(...){
        fclose(file);
        return;
    }
    return;
}