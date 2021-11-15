#include "main.h"
#include "read_from_server.h"
#include "write_on_server.h"
#include <regex>
#include "comunication.h"

//https://datatracker.ietf.org/doc/html/rfc1350

unsigned int arg_preparer(std::string &str){
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
                str.erase(i,1);
                i--;
            }                       
        }
        old = str.at(i);
        i++;
    }
    if(isspace(str.at(i-1))){
        return space_count;
    }
    else return ++space_count;
}

int main(int argc, char *argv[]){
    using namespace std;
    while(1){
        string start = "";
        cout << "> ";
        getline(cin,start);

        if (!strcmp("end",start.c_str())){
            return 0;
        }

        unsigned int argc = arg_preparer(start);
        start.push_back(' ');    //aby nedoslo k segfaultu, kdyz za posledni budu vkladat, MUSI zde byt MEZERA!
        argc++;       //bezne je v argc na prvnim miste jmeno souboru, ze ho musim "umele pridat"
        char *arguments[argc];
        char *p =const_cast<char *>(start.c_str());        
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
        for(;;){
            switch(getopt(argc,arguments, "RWd:pt:Ts:Smc:Ca:Ah")){ 
                case 'h':
                    cout << "help:\n";
                    cout << "-R/W -d path_to_file -t timeout -s block_size -a address,port -c netascii/octet\n";
                    cout << "end <-- write for exit\n";
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
                    if(timeout <= 0 && timeout > 300){
                        cerr << "Timeout must be number: 0 < timeout < 256\n";
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
                return 1;
            }
            if ((port < 0) |(port > 65535)){
                cerr<< "Bad port:" << port_str << endl;
                return 1;
            }       
        }
        string ip = regex_replace(ip_port,port_reg,"");

        #if 1
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
            return 1;
        }

    }
    return 0;
}