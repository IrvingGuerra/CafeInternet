#include "Respuesta.h"
int main(int argc, char const *argv[]){
    if (argc != 2)
    {
        printf("Forma de ejecutar: ./%s [LISTENING PORT]\n", argv[0]);
    }
    Respuesta respuesta(atoi(argv[1]));
    while (true){
        respuesta.getRequest();
    }
    return 0;
}
