#include "functions.h"
#include <string.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>
#include <fstream>

void print_time(){      //https://stackoverflow.com/questions/16077299/how-to-print-current-time-with-milliseconds-using-c-c11
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
        exit(42);
    }
    mode_string[2] = '\0';

    FILE * file = fopen(path.c_str(),mode_string);
    if (file == NULL){
        throw std::exception();
    }
    return file;
}