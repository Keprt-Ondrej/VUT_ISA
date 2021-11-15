/**
 * @file main.h
 * @brief Vstup do programu mytftpclient
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
#ifndef __MAIN_H__
#define __MAIN_H__ 

#include <iostream>
#include <getopt.h>
#include <regex>

/**
 * @brief Hodnoty pro behavior switch
 * 
 * Podle hodnoty se vybere, zda se bude posilat/prijimat data
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
typedef enum{
    undefined,
    write_on_server,
    read_from_server
} behavior_t; 

/**
 * @brief 
 * 
 * @returnint exit value programu, je vzdy 0
 *  
 * vsechny chybi se vraci na pocatek prikazove radky
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
int main();

/**
 * @brief uprava prikazove radky
 * 
 * maze velke mezery mezi slovy
 * 
 * @param str nacteny radek z prikazove radky
 * @return int pocet slov, potrebny pro getopt()
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
int arg_preparer(std::string &str);

#endif