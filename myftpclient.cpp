
#include <iostream>
#include <getopt.h>

int main(int argc, char * argv[]){

    using namespace std;
    cout << "Hello cy koky\n";
    
    for(;;){
        switch(getopt(argc, argv, "ab:h")) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
            case 'a':
                printf("switch 'a' specified\n");
            continue;

            case 'b':
                printf("parameter 'b' specified with the value %s\n", optarg);
            continue;

            case '?':
            case 'h':
            default :
                printf("Help/Usage Example\n");
            break;

            case -1:
            break;
        }
        break;
    }

    return 0;
}