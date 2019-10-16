#include "Solicitud.h"

#include <time.h>
#include <iostream>

int main(int argc, char const *argv[]){
    if (argc != 6){
        printf("Forma de ejecutar: ./%s [ID_RED] [SERVER PORT] [TIEMPO] [HOST_FIRST] [HOST_LAST]\n", argv[0]);
        exit(0);
    }

    while(1){
        // Solicitud.
        //Ocupamos ID de RED
        std::string id_red = argv[1];

        for (int i = atoi(argv[4]); i <= atoi(argv[5]); ++i){
            id_red = id_red.substr(0, id_red.size()-1); //Quitamos el ultimo byte
            std::string host = id_red + "" + std::to_string(i);
            const char *hostSend = host.c_str();
            Solicitud solicitud;
            // Resultados.
            solicitud.doOperation(hostSend, atoi(argv[2]));
            solicitud.cerrarSocket();
        }
        
        usleep(atoi(argv[3]) * 1000000);
    }

    return 0;
}