#include "comunication.h"

comunication::comunication(std::string &ip,std::string &port,int timeout): port(port),ip(ip),connection_info(NULL),timeout(timeout){
    return;
}

int comunication::create_socket(){ 
    int domain;   
    struct addrinfo hint;
    if (ip.find(":") == -1){
        domain = AF_INET;      
        std::cerr << "ipv4\n";
    }
    else{
        domain = AF_INET6;
    }
    hint.ai_family = domain;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_flags = AI_PASSIVE;
    hint.ai_protocol = 0;
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;

    if ((socket_id = socket(domain,SOCK_DGRAM,0)) == -1){
        std::cerr << "can not open socket\n"; //TODO
        return 42;
    }

    int retval = getaddrinfo(ip.c_str(),port.c_str(),&hint,&connection_info);
    if (retval < 0){
        std::cerr << "info failed \n" << ip <<"\t"<<port<<"\n"; //TODO
        exit(42);
    }

    if (timeout != -1){
        struct timeval time;
        time.tv_sec = timeout;
        time.tv_usec = 0;
        if (setsockopt(socket_id,SOL_SOCKET,SO_RCVTIMEO,&time,sizeof(time))){
            std::cerr << "time out  setup fail\n";
            exit(42);
        } 
    }


    return 0;
}

comunication::~comunication(){
    freeaddrinfo(connection_info);
    close(socket_id);
}

int comunication::send_msg(size_t msg_size,const char *msg){
    return sendto(socket_id,msg,msg_size,0,connection_info->ai_addr,connection_info->ai_addrlen);
}

int comunication::receive_msg(size_t buffer_size, char *msg){
    return recvfrom(socket_id,msg,buffer_size,0,connection_info->ai_addr,&connection_info->ai_addrlen);

}

packet_data::packet_data(){
    end_buffer = buffer;
}

packet_data::~packet_data(){}

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

void packet_data::create_request(int16_t opcode,std::string &path,std::string &mode){
    end_buffer = buffer;
    add_2B(opcode);
    add_string(path.c_str());
    add_string(mode.c_str());    
}

int16_t packet_data::get_2B(){
    int16_t tmp = *(int16_t *)end_buffer;
    end_buffer += 2;
    return htons(tmp);
}

void packet_data::create_ACK(int16_t opcode,int16_t block){
    end_buffer = buffer;
    add_2B(opcode);
    add_2B(block);
}


/*
htons() host to network short

htonl() host to network long

ntohs() network to host short

ntohl() network to host long
*/