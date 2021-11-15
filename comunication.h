/**
 * @file comunication.h
 * @brief tridy pres ktere se uskutecnuje komunikace
 * 
 * obsahuje take pomocne funkce
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/

#ifndef __COMUNICATION_H__
#define __COMUNICATION_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h> //socket close
#include <string.h>
#include <iostream>
#include "limits.h"
#include <arpa/inet.h>
#include <chrono>


#define TFTP_DEFAUL_BLOK_SIZE 512 //< velikost bloku dat, specifikovana RFCckem


                //opcode  operation
#define TFTP_READ   1     //Read request (RRQ)
#define TFTP_WRITE  2     //Write request (WRQ)
#define TFTP_DATA   3     //Data (DATA)
#define TFTP_ACK    4     //Acknowledgment (ACK)
#define TFTP_ERR    5     //Error (ERROR)
#define TFTP_OPTION_ACK 6   //Option Acknowledgment (OACK)
#define TFTP_OACK_ERR   8   //Error Acknowledgment (OERR)

#define DEFAULT_TIMEOUT 5 //< zakladni timeout, ktery je nastaven, kdyz neni zadan uzivatelem
#define TIMEOT_CNT 6      //< pocet opakovani pokusu o prijeti/odeslani dat

/**
 * @brief trida pro spravu socketu
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
class comunication{
    public:
    /**
     * @brief nastavi hodnoty, ktere bude pouzivat pro vytvareni socketu
     * 
     * @warning socket nevytvari/neotevira, pouze pripravuje hodnoty!
     * 
     * @param ip adresa serveru
     * @param port port, kde server nasloucha
     * @param timeout doba timeoutu
     * @param use_multiply_timeout pokud chci nasobit dobu cekani poctem cekani,pocet =  TIMEOUT_CNT
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    comunication(std::string &ip,std::string &port,int timeout,bool use_multiply_timeout);

    /**
     * @brief uvolni pamet addrinfo a uzavre socket
     * 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    ~comunication();
    
    /**
     * @brief nastavi timeout socketu
     * 
     * @param timeout doba timeoutu
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void set_timeout(int timeout);

    /**
     * @brief inicializuje socket a nastavuje timeout
     * 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void create_socket();

    /**
     * @brief posila zpravu na server
     * 
     * @param msg_size celkova velikost packetu
     * @param msg bufferm ve kterem jsou data packetu
     * @return int vraci pocet odeslanych dat (v Bytech)
     * 
     *  @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    int send_msg(size_t msg_size,const char *msg);

    /**
     * @brief prijima zpravu od serveru
     * 
     * @param buffer_size celkova velikost packetu, kam se mohou prijata data ulozit
     * @param msg ukazatel na buffer
     * @return int velikost prijatych dat (velikost celeho packetu)
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    int receive_msg(size_t buffer_size, char *msg);

    private:
    int socket_id;      //< deskriptor socketu
    struct addrinfo *connection_info;   //< informace ulozene o serveru
    std::string ip;     //< adresa serveru
    std::string &port;  //<port serveru
    int timeout;        //nastaveny timeout
    int f_mtu_size;     //< nejvetsi mozna velikost cesty <-- neni implementovano
    bool use_multiply_timeout;      //< podle tohoto boolu rozhodujeme, zda se pri nastavovani timeoutu bude nasobit konstantou TIMEOUT_CNT
};

/**
 * @brief trida, ktera obsahuje buffer pro ukladani/posilani dat
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
class packet_data{
    public:
    /**
     * @brief vytvori novou instanci bufferu pro komunikaci
     * 
     * alokace pameti pomoci new, funkce muze vyhodit vyjimku
     * 
     * @param blksize velikost bufferu == velikost bloku dat, co se bude posilat po siti
     * @note celkova velikost dat je tedy blksize + velikost hlavicky (4Byty)
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    packet_data(int blksize);

    /**
     * @brief Uvolni alokovanou pamet bufferu
     * 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    ~packet_data();

    /**
     * @brief nastavi ukazatel end_buffer na zacatek bufferu
     * 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void start_buffer();

    /**
     * @brief nastavi vsechny datove polozky na 0 a nastavi end_buffer na zacatek
     * 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void clear_buffer();

    /**
     * @brief zmeni velikost bufferu
     * 
     * nejdrive uvolni pamet pomoci delete a novou naalokuje pomoci new
     * funkce muze vyhodit vyjimku
     * 
     * @param blk_size 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void change_buffer(size_t blk_size);  

    /**
     * @brief ulozi hodnotu na misto, kde ukazuje end_buffer a posune jej o 2 byty
     * 
     * funkce dostava klasicky hodnotu a sama ji uklada spravne pro sitovy packet
     * 
     * @param c ukladana hodnota
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void add_2B(int16_t c);

    /**
     * @brief funkce kopiruje string namisto, kde ukazuje end_buffer a nasledne jej zvetsi o velikost stringu
     * 
     * @param string 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void add_string(const char *string);
    
    /**
     * @brief vraci hodnotu 2 Bytovou hodnotu,kde ukazuje end_buffer a posune jej o 2 byty
     * 
     * @return int16_t prectena hodnota z bufferu
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    int16_t get_2B();

    /**
     * @brief vraci ukazatel na string, na miste kde ukazuje end_buffer nasledne posune end_buffer za string
     * 
     * @return char* string, ktery je obsazen v bufferu
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    char *get_string();
    
    /**
     * @brief 
     * 
     * @return int velikost packetu, pokud pro manipulaci s daty byli pouzity pouze funkce add_2B a add_string
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    int size();

    /**
     * @brief vytvari zakladni request s option tsize (velikost souboru)
     * 
     * @param opcode vyper operace, TFTP_READ pro stahovani, TFTP_WRITE pro zapis
     * @param path cesta k souboru
     * @param mode prenosu = ascii/netascii octet/binary
     * @param file_size velikost souboru
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void create_request(int16_t opcode,std::string &path,std::string &mode,char *file_size);

    /**
     * @brief nastaveni rozsiteni blksize a timeout
     * 
     * @param size velikost bloku dat, ktery se prenasi po siti
     * @param timeout 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void option_setup(int size,int timeout);

    /**
     * @brief vetvari potvrzeni
     * 
     * @param opcode TFTP_ACK
     * @param block blok dat, ktery byl prijat
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void create_ACK(int16_t opcode,int16_t block);

    /**
     * @brief zpracovava timeout odpoved od serveru
     * 
     * pokud server potvrdi 
     * 
     * @param timeout 
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void OACK_option_handler_timeout(comunication &klient,int timeout);

    /**
     * @brief zpracovava option velikosti bloku dat, pokud server odmitne, nastavi velikost bufferu na velikost, kterou server navrhne
     * 
     * @param size poptavana velikost bufferu
     * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
    */
    void OACK_option_handler_blksize(int size);

    public:
    int buffer_size;    //< velikost bloku dat
    int packet_size;    //< velikost celeho packetu
    char *buffer;       //< ukazatel na pocatek packetu
    char *end_buffer;   //<pohyblivy ukazatel po bufferu
};

/**
 * @brief funkce pro tisknuti casu, neukoncuje radek
 * 
 * funkce byla prevzata a upravena z odpovedi ze stackoverflow
 * @see https://stackoverflow.com/questions/16077299/how-to-print-current-time-with-milliseconds-using-c-c11/50923834#50923834
 * 
 * @author https://stackoverflow.com/users/7394372/enrico-pintus
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
void print_time();

/**
 * @brief otevira soubor v zadanem rezimu
 * 
 * @param path cesta k souboru
 * @param mode binary/octet nebo ascii/netascii
 * @param RW r pro cteni nebo w pro zapis
 * @return FILE* ukazatel na otevreny soubor, v danem modu nebo NULL pri chybe
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
*/
FILE *open_file(std::string &path,std::string &mode, char RW);

#endif