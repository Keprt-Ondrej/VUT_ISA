#include "myftpclient.h"
#include "server.h"
#include "client.h"

int main(int argc, char *argv[]){
    using namespace std; 
     
    behavior_t behavior = undefined;
    bool b_path = false;
    string path;    
    int timeout = 120;
    int size = 42; // TODO podivat se DO RFC kolik to ma byt
    bool b_multicast = false;
    string mode = "binary"; //mód, který může nabýt jen hodnot "ascii" (nebo "netascii") a "binary" (nebo "octet"), čímž klient specifikuje mód přenosu dat. Implicitně (bez přítomnosti argumentu) se uvažuje binární transfer. 
    string ip_port = "127.0.0.1,69";

    for(;;){
        switch(getopt(argc, argv, "RWd:pt:Ts:Smc:Ca:Ah")){ 
            case 'h':
                cout << "help\n";
                return 0;
            continue;

            case 'R':
            if (behavior == undefined){
                behavior = server;
            }
            else{
                cerr << "Choose only one behavior!\n";
                return 1;
            }               
            continue;

            case 'W':
                if (behavior == undefined){
                    behavior = client;
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
    
    #if 0
    cout << "path:\t" << path << "\n";
    cout << "timeout:" << timeout << "\n";
    cout << "size:\t" << size << "\n";
    cout << "multicast:" << b_multicast << "\n";   
    cout << "mod:\t" << mode << "\n";
    cout << "ip,port:\t" << ip_port << "\n";
    #endif

    switch(behavior){
        case client:
            return client_main(path,timeout,size,b_multicast,mode,ip_port); //std::string &path,int timeout,int size,bool b_multicast,std::string&mod, std::string ip_port
        break;

        case server:
            return server_main(path,timeout,size,b_multicast,mode,ip_port);
        break;

        default:
        cerr << "Behaviour wasn't selected\n";
        return 1;
    }
    return 0;
}