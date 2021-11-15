/**
 * @file comunication.cpp
 * @brief tridy pres ktere se uskutecnuje komunikace
 * 
 * obsahuje take pomocne funkce 
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#include "comunication.h"
#include <exception>


comunication::comunication(std::string &ip,std::string &port,int timeout,bool use_multiply_timeout): port(port),ip(ip),connection_info(NULL),timeout(timeout),use_multiply_timeout(use_multiply_timeout){    
    return;
}

void comunication::create_socket(){ 
    int domain;   
    struct addrinfo hint;

    //rozhoduji, zda se bude posilat na ipv4 neb o ipv6
    if (ip.find(":") == -1){        
        domain = AF_INET;
    }
    else{
        domain = AF_INET6;
    }

    //nastaveni napovedy pro hledani serveru
    hint.ai_family = domain;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_flags = AI_PASSIVE;
    hint.ai_protocol = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    if ((socket_id = socket(domain,SOCK_DGRAM,0)) == -1){
        print_time(); std::cerr << "Could not open socket\n";
        throw std::exception();
    }

    int retval = getaddrinfo(ip.c_str(),port.c_str(),&hint,&connection_info);
    if (retval < 0){
        print_time(); std::cerr << "Get addr info failed \n" << ip <<"\t"<<port<<"\n"; 
        throw std::exception();
    }
    set_timeout(timeout);
}

comunication::~comunication(){
    freeaddrinfo(connection_info);
    close(socket_id);
}

int comunication::send_msg(size_t msg_size,const char *msg){
    return sendto(socket_id,msg,msg_size,0,connection_info->ai_addr,connection_info->ai_addrlen);
}

int comunication::receive_msg(size_t buffer_size, char *msg){
    ssize_t ret_value = recvfrom(socket_id,msg,buffer_size,0,connection_info->ai_addr,&connection_info->ai_addrlen);
    if (ret_value == -1){   //pokud doslo k timeoutu
        print_time(); std::cerr << "TIMEOUTED\n";
        throw std::exception();
    }
    return ret_value;
}

void comunication::set_timeout(int timeout_extern){  //menil se timeout pomoci prikazu  
    struct timeval time;    
    if(timeout_extern == -1){
        timeout_extern = DEFAULT_TIMEOUT;
        timeout = timeout_extern;
    }
    else{
        timeout = timeout_extern;
    }
    if(use_multiply_timeout){
        time.tv_sec = (timeout_extern*TIMEOT_CNT)+1; //uvazujeme, ze server posila data opakovane TIMEOT_CNTx (refernci server pro windows), +1 pro zpozdeni PC
    }
    else{
        time.tv_sec = timeout_extern+1;
    }
    
    time.tv_usec = 0;
    if (setsockopt(socket_id,SOL_SOCKET,SO_RCVTIMEO,&time,sizeof(time))){
        print_time(); std::cerr << "timeout setup fail\n";
        throw std::exception();
    } 
    timeout = timeout_extern;
}

packet_data::packet_data(int blksize){
    buffer_size = blksize;
    packet_size = blksize + 4;  // 2 bytes opcode, 2 bytes block number
    buffer = new char[packet_size]();
    end_buffer = buffer;
}

packet_data::~packet_data(){
    delete[] buffer;
}

void packet_data::add_2B(int16_t c){
    *(int16_t *)end_buffer = htons(c);
    end_buffer += 2;
}

void packet_data::add_string(const char *string){
    strcpy(end_buffer, string);
    end_buffer += strlen(string)+ 1;    
}

void packet_data::start_buffer(){    
    end_buffer = buffer;
}

int packet_data::size(){
    return end_buffer - buffer;
}

void packet_data::create_request(int16_t opcode,std::string &path,std::string &mode,char *file_size){
    end_buffer = buffer;
    add_2B(opcode);
    add_string(path.c_str());
    add_string(mode.c_str());
    add_string("tsize");
    add_string(file_size);
}

int16_t packet_data::get_2B(){
    int16_t tmp = *(int16_t *)end_buffer;
    end_buffer += 2;
    return htons(tmp);
}

char * packet_data::get_string(){
    char *tmp = end_buffer;
    end_buffer += strlen(tmp)+1;
    return tmp;
}

void packet_data::create_ACK(int16_t opcode,int16_t block){
    end_buffer = buffer;
    add_2B(opcode);
    add_2B(block);
}

void packet_data::clear_buffer(){
    memset(buffer,0,buffer_size);
    end_buffer = buffer;
}

void packet_data::change_buffer(size_t blksize){
    delete[] buffer;
    buffer_size = blksize;
    packet_size = blksize + 4;  // 2 bytes opcode, 2 bytes block number
    buffer = new char[packet_size]();
    end_buffer = buffer;
}

void packet_data::OACK_option_handler_blksize(int size){
    if(size != TFTP_DEFAUL_BLOK_SIZE){
        std::string{get_string()}; //zahozeni identifikatoru pro bloksize
        size_t real_size = atoi(std::string{get_string()}.c_str());
        if (real_size != size){
            change_buffer(real_size);
            print_time();std::cerr << "Server handle only " << real_size << " B block, resume transfer with "<<real_size << " B size\n";
        }               
    }
}

void packet_data::OACK_option_handler_timeout(comunication &klient,int timeout){
    if(timeout != -1){  //menil se timeout pomoci prikazu
        std::string{get_string()}; //zahozeni identifikatoru pro bloksize
        size_t real_timeout = atoi(std::string{get_string()}.c_str());                
        if (real_timeout == 0){ //pokud server vrati alternativni hodnotu timeoutu, zmeni ji v nastaveni
            //server zamitnul timeout hodnotu
            klient.set_timeout(DEFAULT_TIMEOUT);
            print_time(); std::cerr << "Server refused using timeout: "<< timeout << ", transfer resume with timeout: " << DEFAULT_TIMEOUT << "\n";
        }                     
    }
}

void packet_data::option_setup(int size,int timeout){
    if (size != TFTP_DEFAUL_BLOK_SIZE){
        add_string("blksize");
        add_string(std::to_string(size).c_str());
    }
    if (timeout != -1){
        add_string("timeout");
        add_string(std::to_string(timeout).c_str());
    }
}

void print_time(){
    using std::chrono::system_clock;
    auto currentTime = std::chrono::system_clock::now();
    char buffer[80];
    auto transformed = currentTime.time_since_epoch().count() / 1000000;
    auto millis = transformed % 1000;
    std::time_t tt;
    tt = system_clock::to_time_t ( currentTime );
    auto timeinfo = localtime (&tt);
    strftime (buffer,80,"%F %H:%M:%S",timeinfo);
    std::cout <<"[" << std::string(buffer)<<"."<< (int)millis << "] ";
}

FILE *open_file(std::string &path,std::string &mode, char RW){
    char mode_string[3];    
    mode_string[0] = RW;
    if (!strcmp(mode.c_str(),"netascii") || !strcmp(mode.c_str(),"ascii")){        
        mode_string[1] = '\0';
    }
    else if(!strcmp(mode.c_str(),"binary") || !strcmp(mode.c_str(),"octet")){
        mode_string[1] = 'b';
    }
    else{
        std::cerr << "Wrong mode!\n";
        return NULL;
    }
    mode_string[2] = '\0'; 
    return fopen(path.c_str(),mode_string);
}