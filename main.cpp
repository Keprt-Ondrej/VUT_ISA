#include "main.h"
#include "read_from_server.h"
#include "write_on_server.h"
#include <regex>

//https://datatracker.ietf.org/doc/html/rfc1350

int main(int argc, char *argv[]){
    using namespace std; 
     
    behavior_t behavior = undefined;
    bool b_path = false;
    string path;    
    int timeout = -1;
    int size = 42; // TODO podivat se DO RFC kolik to ma byt
    bool b_multicast = false;
    string mode = "octet"; //mód, který může nabýt jen hodnot "ascii" (nebo "netascii") a "binary" (nebo "octet"), čímž klient specifikuje mód přenosu dat. Implicitně (bez přítomnosti argumentu) se uvažuje binární transfer. 
    string ip_port = "127.0.0.1,69";

    for(;;){
        switch(getopt(argc, argv, "RWd:pt:Ts:Smc:Ca:Ah")){ 
            case 'h':
                cout << "help:\n";
                cout << "./myftpclient R/W -d adresar/soubor -t timeout -s velikost -a adresa,port -c mód -m\n";
                return 0;
            continue;

            case 'R':
            if (behavior == undefined){
                behavior = read_from_server;
            }
            else{
                cerr << "Choose only one behavior!\n";
                return 1;
            }               
            continue;

            case 'W':
                if (behavior == undefined){
                    behavior = write_on_server;
                } 
                else{
                    cerr << "Choose only one behavior!\n";
                    return 1;
                }  
            continue;

            case 'd':
                b_path = true;
                path = optarg;
            continue;

            case 't':
                timeout = atoi(optarg);
                if(timeout <= 0){
                    cerr << "Timeout must be number > 0\n";
                    return 1;
                }
            continue;  

            case 's': //parametr -s (nepovinný) velikost specifikuje maximální velikost bloku v násobcích oktetů, které bude klient navrhovat pro přenos. Jako horní hranici, kterou klient odesílá, uvažujte nejmenší z MTU ze všech rozhraních, na které by klient mohl TFTP zprávu serveru odeslat (pozor, tedy potenicálně více než 512 B, co je v RFC).
                size = atoi(optarg);
                if(size <= 0){
                    cerr << "Size must be number > 0\n";
                    return 1;
                }
            continue;

            case 'm':
                b_multicast = true;
            continue;

            case 'c':
                mode = optarg;
            continue;

            case 'a':
                ip_port = optarg;
            continue;

            
            default :
                cerr << "Wrong parameter! \n";
                return 1;
            break; 

            case -1:
            break;
        }
        break;
    }
    
    if (b_path == false){
        cerr << "Select path to file!\n";
        return 1;
    }

    string port_str = "";
    smatch match;
    regex port_reg (",[0-9a-zA-Z]*");
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
            return 1;
        }
        if ((port < 0) |(port > 65535)){
            cerr<< "Bad port:" << port_str << endl;
            return 1;
        }       
    }
    string ip = regex_replace(ip_port,port_reg,"");

    #if 0
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
            return write_on_server_main(path,timeout,size,b_multicast,mode,ip,port_str);
        break;

        case read_from_server:
            return read_from_server_main(path,timeout,size,b_multicast,mode,ip,port_str);
        break;

        default:
        cerr << "Behaviour wasn't selected\n";
        return 1;
    }
    return 0;
}