/**
 * @file main.h
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * @brief 
 * 
*/
#ifndef __MAIN_H__
#define __MAIN_H__ 

#include <iostream>
#include <getopt.h>
#include <regex>

typedef enum{
    undefined,
    write_on_server,
    read_from_server
} behavior_t; 

int main();
int arg_preparer(std::string &str);

#endif