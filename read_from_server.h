/**
 * @file read_from_server.h
 * @brief Prijem dat ze serveru
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
#ifndef __SERVER_H__
#define __SERVER_H__


#include <iostream>
#include <stdexcept>
#include "comunication.h"
#include <sys/statvfs.h>
#include <exception>
#include <string>
#include <cstdlib>
#include <sys/ioctl.h>
#include <net/if.h>

/**
 * @brief Hlavni telo cteni dat ze serveru
 * 
 * Alokace pameti pro data, priprava socketu, otevreni souboru
 * 
 * @param path cesta k souboru na serveru
 * @param timeout doba cekani, pokud je -1 hodnota nebyla uzivatelem nastavena
 * @param size velikost bloku dat, ktery bude po siti prenasen
 * @param b_multicast zapnuti/vypnuti multicastu, neni implementovano
 * @param mode mod pro prenos, ascii/netascii/octet/binary
 * @param ip server ip, odkud se budou data cist
 * @param port port, na kterem tftp server nasloucha
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void read_from_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port);

/**
 * @brief Cteni dat ze serveru
 * 
 * @note muze vyhodit std::exception pri timeoutu
 * 
 * @param klient trida, pres kterou se uskutecnuje komunikace
 * @param packet trida, kde jsou ulozena/prijimana data
 * @param file deskriptor otevreneho souboru, kam se data ukladaji
 * @param file_size celkova velikost prenaseneho souboru
 * @param start kolik dat jiz belo drive prijato
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void receive_data(comunication &klient,packet_data &packet,FILE *file, std::string &file_size,int16_t start);

#endif