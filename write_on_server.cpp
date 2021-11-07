#include "write_on_server.h"
#include <string.h>

int write_on_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,int port){
    std::cerr << "write_on_server\n";  

/*
    using namespace udp_client_server;
    udp_client klient(ip,port);    
    const char *zprava = "Tohle je moje zprava\n";
    size_t msg_size = (size_t)strlen(zprava);
    klient.send(zprava,msg_size);
    
*/
    return 1;
}