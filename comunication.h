/**
 * @file comunication.h
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/

#ifndef __COMUNICATION_H__
#define __COMUNICATION_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h> //socket close
#include <string.h>
#include <iostream>

#define block_size 512
                //opcode  operation
#define TFTP_READ   1     //Read request (RRQ)
#define TFTP_WRITE  2     //Write request (WRQ)
#define TFTP_DATA   3     //Data (DATA)
#define TFTP_ACK    4     //Acknowledgment (ACK)
#define TFTP_ERR    5     //Error (ERROR)

class comunication{
    public:
    comunication(std::string &ip,std::string &port,int timeout);
    ~comunication();
    
    int create_socket();
    int send_msg(size_t msg_size,const char *msg);
    int receive_msg(size_t buffer_size, char *msg);

    private:
    int socket_id;
    struct addrinfo *connection_info;
    std::string ip;
    std::string &port;
    int timeout;    
};

class packet_data{
    public:
    packet_data();
    ~packet_data();
    void start_buffer();
    void add_2B(int16_t c);
    void add_string(const char *string);
    int size();
    void create_request(int16_t opcode,std::string &path,std::string &mode);
    int16_t get_2B();
    void create_ACK(int16_t opcode,int16_t block);

    public:
    constexpr static int buffer_size = block_size + 4;  // 2 opcode + 2 block
    char buffer[buffer_size];
    char *end_buffer;
};













#endif