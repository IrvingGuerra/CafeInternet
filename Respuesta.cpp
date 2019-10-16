#include "Respuesta.h"
Respuesta::Respuesta(int port){
    // Recibe puerto por el que se compunica el servidor.
    socketLocal = new SocketDatagrama(port);
}
void Respuesta::getRequest(){
    int r = socketLocal->esperaPeticion();
    if (r != 1){
        socketLocal->enviaImagen(r);
    }
}