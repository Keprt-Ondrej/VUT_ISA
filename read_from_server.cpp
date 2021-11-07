#include "read_from_server.h"
#include "functions.h"

int read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,int port){
    std::cerr << "read_from server\n";
    /*
    using namespace udp_client_server;
    udp_server server(ip,port);
    size_t max_size = 1024;
    char buffer[max_size] = "";
    while(1){
        int result = server.timed_recv(buffer,max_size,16);
        if (result == -1){
            print_time(); std::cout << "No data received\n";            
        }
        else{
            print_time();
            printf("%s",buffer);
        }        
    }
    return 1;
    */
   return 0;
}