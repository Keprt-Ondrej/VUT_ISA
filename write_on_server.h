/**
 * @file write_on_server.h
 * @brief Posilani dat na server
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "comunication.h"
#include <sys/stat.h>
#include <iostream>

/**
 * @brief hlavni telo pri posilani dat
 * 
 * Alokuje pamet pro data, pripravuje sockety, otevira soubor pro poslani
 * 
 * @param path cesta k souboru, co se bude odesilat
 * @param timeout doba timeoutu, pokud je -1 nebyl zadan uzivatelem
 * @param size velikost bloku dat, ktery se bude posilat po siti
 * @param b_multicast zapnuti/vypnuti multicastu, neni implementovano
 * @param mode mod pro prenos, ascii/netascii/octet/binary
 * @param ip server ip, odkud kam se budou data posilat
 * @param port port, na kterem tftp server nasloucha
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void write_on_server_main(std::string &path,int timeout,int size,bool b_multicast,std::string&mode, std::string &ip,std::string &port);

/**
 * @brief Posilani dat na server
 * 
 * Odesila data na server, pokud neprijde acknowledgement, pokusi se znovu data nekolikrat odeslat
 * jinak konci chybou
 * 
 * @note muze vyhodit std::exception pri timeoutu
 *  
 * @param klient trida, pres kterou se uskutecnuje komunikace
 * @param packet trida, kde jsou ulozena pro odeslani
 * @param packet_ack trida, kde se uklada prijaty acknowledgement
 * @param file deskriptor otevreneho souboru, odkud se data ctou
 * @param file_size celkova velikost odesilaneho souboru
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void send_data(comunication &klient,packet_data &packet,packet_data &packet_ack,FILE *file, std::string &file_size);

#endif