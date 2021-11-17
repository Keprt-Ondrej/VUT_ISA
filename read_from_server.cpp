/**
 * @file read_from_server.cpp
 * @brief Prijem dat ze serveru
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
#include "read_from_server.h"

void receive_data(comunication &klient,packet_data &packet,FILE *file, std::string &file_size,int16_t start =0){
    int16_t opcode;
    int16_t block;
    int16_t expected_block;
    unsigned long total_received = start;  

    int16_t recv_data_size = INT16_MAX; //pro prvni podminku cyklu

    while(!(recv_data_size < packet.buffer_size)){ //dokud neni velikost prijatych dat mensi jak velikost 
        recv_data_size = klient.receive_msg(packet.packet_size,packet.buffer);        
        recv_data_size -= 4; // 4 velikost hlavicky
        packet.start_buffer();
        opcode = packet.get_2B();         
        if (opcode == TFTP_DATA){
            block = packet.get_2B(); 
            total_received += recv_data_size;
            print_time(); std::cout << "Downloaded " << total_received << "B, of " << file_size << " B from: "<< klient.ip << " port: "<< klient.port<<"\n"; 
            fwrite(packet.end_buffer,1,recv_data_size,file);
            packet.create_ACK(TFTP_ACK,block);
            klient.send_msg(packet.size(),packet.buffer);
        }
        else{
            print_time(); std::cerr << "Error " << packet.get_2B()<< ": " << packet.get_string()<< " transfer terminated\n";
            throw std::exception();
        }
    }
}

void read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string & port){
    FILE *file;
    try{
        //inicializace socketu a priprava bufferu pro data
        comunication klient(ip,port,timeout,true); //nastavi timeout zadany, uzivatelem nebo defaultni, pokud server odmitne uzivateluv timeout, ve funkci OACK_option_handler_timeout nastavi timeout klienta na defaultni velikost
        klient.create_socket();
        packet_data packet(size);

        //otevreni souboru
        file = open_file(path,mode,'w');
        if(file == NULL){
            print_time(); std::cerr << "could not open file: "<< path << "\n";
            return;
        }

        //zjisteni velikosti disku
        struct statvfs disk_info;
        int rer_val = statvfs("./",&disk_info);    
        unsigned long long free_size = disk_info.f_bsize * disk_info.f_bfree;

        //vytvoreni pozadavku a pridani voleb, pokud byly uzivatelem zadány
        packet.create_request(TFTP_READ,path,mode,(char *)"0");
        packet.option_setup(size,timeout);
        
        //zaslání pozadavku na server 
        klient.send_msg(packet.size(),packet.buffer);
        print_time(); std::cout << "Read request send on server "<<ip << " port: "<<port << " requested file: "<<path<<"\n";
        packet.clear_buffer(); // pro spravny zpracovani voleb

        int16_t recv_data_size = klient.receive_msg(packet.packet_size,packet.buffer);
        packet.start_buffer();
        int16_t opcode = packet.get_2B();   
        //velikost nastavím prozatím na neznámou, protoze nemohu pokracovat ve zpracovani paketu, protoze bych mohl zpracovavat poslana data    
        std::string file_size = {"UNKNOWN"};       

        if (opcode == TFTP_OPTION_ACK){
            //server zpracovava volby
            std::string{packet.get_string()};  //potrebuji zahodit option specifier pro tsize
            std::string file_size = std::string{packet.get_string()};           
            unsigned long long file_size_number = strtoull(file_size.c_str(),nullptr,10);
            if ( file_size_number > free_size){
                //nedostatek místa na disku uzivatele
                packet.start_buffer();
                packet.add_2B(TFTP_ERR);              
                packet.add_2B(3);   //errcode pro plny disk specifikovany v RFC
                packet.add_string("Disk full or allocation exceeded");
                print_time();std::cerr << "Not enough space on disk, requested file size: " << file_size <<" B\n";
                //odeslani
                klient.send_msg(packet.size(),packet.buffer);                
                throw std::exception(); //musim uvolnit pamet bufferu a uzavrit soubor a socket, proto zde neni return
            }
            //zpracovani voleb
            packet.OACK_option_handler_blksize(size);
            packet.OACK_option_handler_timeout(klient,timeout);
            //vytvoreni potvrzovaciho paketu
            packet.create_ACK(TFTP_ACK,0);
            klient.send_msg(packet.size(),packet.buffer);
            //prijem vůastnich dat
            receive_data(klient,packet,file,file_size);            
        }
        else if (opcode == TFTP_DATA){
            //server nepodporuje option = posila pakety o velikosti 512 B a nezname velikost prichoziho souboru...
            //neposilam zpracovani rovnou do funkce protoze mi data jiz prisla a muselo by se tak cekat na timeout a novy
            //packet proto zde opakuji cast funkce
            print_time(); std::cerr << "Server refused options, resume transfer by RFC 1350 specification, http://tools.ietf.org/html/rfc1350\n";
            recv_data_size -= 4;  //odecitam hlavicku          
            int16_t block = packet.get_2B();
            print_time(); std::cout << "Downloaded " << recv_data_size << "B, of " << file_size << " B from: "<< klient.ip << " port: "<< klient.port<<"\n";
            fwrite(packet.end_buffer,1,recv_data_size,file); //zapis dat na disk
            //poslani potvrzovaciho paketu
            packet.create_ACK(TFTP_ACK,block);
            klient.send_msg(packet.size(),packet.buffer);
            if (!(recv_data_size < TFTP_DEFAUL_BLOK_SIZE)){
                //pokud je soubor vetsi, ukaldam zbytek dat
                receive_data(klient,packet,file,file_size,recv_data_size); //posilam i pocet prijatych dat
            }            
        }
        else{
            std::cout << "Error " << packet.get_2B()<< ": " << packet.get_string()<< "\n";
            throw std::exception();
        }   

        fclose(file);
        print_time(); std::cout << "Transfer completed without errors\n";
    }
    catch(...){
        fclose(file);
    }
    return;
}