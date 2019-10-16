#include "Solicitud.h"

Solicitud::Solicitud(){
    socketlocal = new SocketDatagrama(0);
}

void Solicitud::doOperation(const char *serverIpAdress, int serverPort){
	int r = socketlocal->conectar(serverIpAdress, serverPort);
	if (r==0){
		socketlocal->recibeImagen(serverIpAdress);
	}
}

void Solicitud::cerrarSocket(){
	delete socketlocal;
}