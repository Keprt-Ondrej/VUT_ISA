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
#include "limits.h"

#define TFTP_DEFAUL_BLOK_SIZE 512
                //opcode  operation
#define TFTP_READ   1     //Read request (RRQ)
#define TFTP_WRITE  2     //Write request (WRQ)
#define TFTP_DATA   3     //Data (DATA)
#define TFTP_ACK    4     //Acknowledgment (ACK)
#define TFTP_ERR    5     //Error (ERROR)
#define TFTP_OPTION_ACK 6   
#define TFTP_OACK_ERR   8

#define DEFAULT_TIMEOUT 3

class comunication{
    public:
    comunication(std::string &ip,std::string &port,int timeout);
    ~comunication();
    
    int create_socket();
    int send_msg(size_t msg_size,const char *msg);
    int receive_msg(size_t buffer_size, char *msg);

    int get_mtu_size();
    void set_timeout(int timeout);

    public:
    int socket_id;
    bool ipv4;
    struct addrinfo *connection_info;
    std::string ip;
    std::string &port;
    int timeout;   
    int f_mtu_size;  
};

class packet_data{
    public:
    packet_data(int blksize);
    ~packet_data();
    void option_setup(int size,int timeout);
    void start_buffer();
    void print_buffer();
    void add_2B(int16_t c);
    void add_1B(char c);
    void add_string(const char *string);
    int size();
    void create_request(int16_t opcode,std::string &path,std::string &mode,char *file_size);
    int16_t get_2B();
    char *get_string();
    void create_ACK(int16_t opcode,int16_t block);
    void clear_buffer();
    void change_buffer(size_t blk_size);
    void OACK_option_handler_timeout(int timeout);
    void OACK_option_handler_blksize(int size);

    public:
    int buffer_size;
    int packet_size;  
    char *buffer;
    char *end_buffer;
};

void OACK_option_handler_blksize(comunication &klient,packet_data &packet,int size);
void OACK_option_handler_timeout(comunication &klient,packet_data &packet,int timeout);

void print_time();
FILE *open_file(std::string &path,std::string &mode, char RW);






#endif