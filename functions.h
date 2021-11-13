
#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__


#include <ctime>
#include <cstdio>
#include <string>

void print_time();
FILE *open_file(std::string &path,std::string &mode, char RW);

#endif