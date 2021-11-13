#include "functions.h"
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

void print_time(){
    using namespace std;
    std::time_t result = std::time(nullptr);
    std::tm *time = std::localtime(&result);
    cout <<"["<<
    time->tm_year+1900<<"-"<<time->tm_mday<<"-"
    <<time->tm_mon + 1 <<" "<<time->tm_hour<<":"
    <<time->tm_min<<":"<<time->tm_sec<< "] ";
}

FILE *open_file(std::string &path,std::string &mode, char RW){
    char mode_string[3];    
    mode_string[0] = RW;
    if (!strcmp(mode.c_str(),"netascii") || !strcmp(mode.c_str(),"ascii")){
        std::cerr << "ascii\n";
        mode_string[1] = '\0';
    }
    else if(!strcmp(mode.c_str(),"binary") || !strcmp(mode.c_str(),"octet")){
        mode_string[1] = 'b';
        std::cerr << "bin\n";
    }
    else{
        std::cerr << "Wrong mode!\n";
        exit(42);
    }
    mode_string[2] = '\0';
    std::cerr << mode_string << "\n";
    return fopen(path.c_str(),mode_string);
}
