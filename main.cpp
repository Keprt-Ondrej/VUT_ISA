/**
 * @file main.cpp
 * @brief Vstup do programu mytftpclient
 * 
 * Nacitani a zpracovani prikazove radky programu 
 * 
 * @author Ondřej Keprt (xkeprt03@stud.fit.vutbr.cz)
 * 
*/

//test gitu

#include "main.h"
#include "read_from_server.h"
#include "write_on_server.h"
#include "comunication.h"

int arg_preparer(std::string &str){
    unsigned int i = 0;
    unsigned int space_count = 0;
    char old = '\0';
    while (isspace(str.at(i))){
        str.erase(i,1);    //skip nazacatku
    }

    while(i < str.size()){  
        if (isspace(str.at(i))){
            if (old != str.at(i)){
                space_count++;
            }
            else{
                str.erase(i,1); //mazu vetsi mezery
                i--;        //musim zmensit, jinak bych jej zvetsil a to nechci, musi zustat stejne velke
            }                       
        }
        old = str.at(i);
        i++;
    }
    if(isspace(str.at(i-1))){   
        return space_count; //pokud byl na konci whitespace => spocitalo to spravne slova
    }
    else return ++space_count; //slov je o 1 vic jak mezer
}

int main(){
    using namespace std;
    while(1){
        string start = "";
        cout << "> ";
        getline(cin,start); //prikazova radka programu

        //ukonceni programu, kdyz jiz nechci stahovat dalsi soubory
        if (!strcmp("end",start.c_str())){
            return 0;
        }

        unsigned int argc = arg_preparer(start);
        start.push_back(' ');    //aby nedoslo k segfaultu, kdyz za posledni budu vkladat, MUSI zde byt MEZERA!
        argc++;       //bezne je v argc na prvnim miste jmeno souboru, zde ho musim "umele pridat" aby jej getopt nepreskocilo
        char *arguments[argc];
        char *p =const_cast<char *>(start.c_str());  
        //ukladam ukazatele do pole, vytvarim umele argv, co byva pri spusteni programu      
        for(int i = 1; i < argc;i++){
            arguments[i] = p;
            while(!isspace(p[0])){
                p++;
            }            
            p[0] = '\0';
            p++;
        }
        behavior_t behavior = undefined;
        bool b_path = false;
        string path;    
        int timeout = -1;
        int size = TFTP_DEFAUL_BLOK_SIZE;
        bool b_multicast = false;
        string mode = "octet"; //mód, který může nabýt jen hodnot "ascii" (nebo "netascii") a "binary" (nebo "octet"), čímž klient specifikuje mód přenosu dat. Implicitně (bez přítomnosti argumentu) se uvažuje binární transfer. 
        string ip_port = "127.0.0.1,69";    

        optind = 1;
        try{
            for(;;){
                switch(getopt(argc,arguments, "RWd:pt:Ts:Smc:Ca:Ah")){ 
                    case 'h':
                        cout << "help:\n";
                        cout << "-R/W -d path_to_file -t timeout -s block_size -a address,port -c netascii/octet\n";
                        cout << "end <-- write for exit\n";
                        throw std::exception();
                    continue;

                    case 'R':
                    if (behavior == undefined){
                        behavior = read_from_server;
                    }
                    else{
                        cerr << "Choose only one behavior!\n";
                        throw std::exception();
                    }               
                    continue;

                    case 'W':
                        if (behavior == undefined){
                            behavior = write_on_server;
                        } 
                        else{
                            cerr << "Choose only one behavior!\n";
                            throw std::exception();
                        }  
                    continue;

                    case 'd':
                        b_path = true;
                        path = optarg;
                    continue;

                    case 't':
                        timeout = atoi(optarg);
                        if(timeout <= 0 && timeout > 300){
                            cerr << "Timeout must be number: 0 < timeout < 256\n";
                            throw std::exception();
                        }
                    continue;  

                    case 's': //parametr -s (nepovinný) velikost specifikuje maximální velikost bloku v násobcích oktetů, které bude klient navrhovat pro přenos. Jako horní hranici, kterou klient odesílá, uvažujte nejmenší z MTU ze všech rozhraních, na které by klient mohl TFTP zprávu serveru odeslat (pozor, tedy potenicálně více než 512 B, co je v RFC).
                        size = atoi(optarg);
                        if(size <= 0){
                            cerr << "Size must be number > 0\n";
                            throw std::exception();
                        }
                    continue;

                    case 'm':
                        b_multicast = true;
                        cerr << "Multicast not implemented\n";
                        throw std::exception();
                    continue;

                    case 'c':
                        mode = optarg;
                    continue;

                    case 'a':
                        ip_port = optarg;
                    continue;

                    
                    default :
                        cerr << "Wrong parameter! \n";
                        throw std::exception();
                    break; 

                    case -1:
                    break;
                }
                break;
            }
        }
        catch(...){
            continue;
        }

        if (b_path == false){
            cerr << "Select path to file!\n";
            continue;
        }

        
        string port_str = "";
        smatch match;
        regex port_reg(",[0-9a-zA-Z]*");
        int port;
        
        regex_search(ip_port,match,port_reg);
        if (match.ready()){
            port_str = match.str(0);
            port_str.erase(0,1);
            try{            
                port = stoi(port_str);
            }
            catch(exception){
                cerr<< "Bad port:" << port_str << endl;
                continue;
            }
            if ((port < 0) |(port > 65535)){
                cerr<< "Bad port:" << port_str << endl;
                continue;
            }       
        }
        string ip = regex_replace(ip_port,port_reg,"");

        //debug controla
        #if 0
        cerr << "--------------------------------------------\n";
        cerr << "path:\t" << path << "\n";
        cerr << "timeout:" << timeout << "\n";
        cerr << "size:\t" << size << "\n";
        cerr << "multicast:" << b_multicast << "\n";   
        cerr << "mod:\t" << mode << "\n";
        cerr << "ip: " << ip << "\n";
        cerr << "port: " << port << "\n";
        cerr << "--------------------------------------------\n";
        #endif

        switch(behavior){
            case write_on_server:
                write_on_server_main(path,timeout,size,b_multicast,mode,ip,port_str);
            break;

            case read_from_server:
                read_from_server_main(path,timeout,size,b_multicast,mode,ip,port_str);
            break;

            default:
            cerr << "Behaviour wasn't selected\n";
            continue;
        }

    }
    return 0;
}