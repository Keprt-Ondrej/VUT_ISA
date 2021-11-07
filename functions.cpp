#include "functions.h"

void print_time(){
    using namespace std;
    std::time_t result = std::time(nullptr);
    std::tm *time = std::localtime(&result);
    cout <<"["<<
    time->tm_year+1900<<"-"<<time->tm_mday<<"-"
    <<time->tm_mon + 1 <<" "<<time->tm_hour<<":"
    <<time->tm_min<<":"<<time->tm_sec<< "] ";
}


