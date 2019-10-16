#include "SocketDatagrama.h"

#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>   
#include<unistd.h>  
#include<iostream>
#include<fstream>
#include<errno.h>
#include<sys/ioctl.h>

using namespace std;

namespace std {
  template <typename _CharT, typename _Traits>
  inline basic_ostream<_CharT, _Traits> &
  tab(basic_ostream<_CharT, _Traits> &__os) {
    return __os.put(__os.widen('\t'));
  }
}

SocketDatagrama::SocketDatagrama(int puertoLocal){
    s = socket(AF_INET, SOCK_STREAM, 0);

    int longitudLocal = sizeof(direccionLocal);

    bzero(&direccionLocal, longitudLocal);
    direccionLocal.sin_family = AF_INET;
    direccionLocal.sin_addr.s_addr = INADDR_ANY;
    direccionLocal.sin_port = htons(puertoLocal);

    bind(s, (sockaddr *)&direccionLocal, longitudLocal);
}

int SocketDatagrama::conectar(const char *serverIpAdress, int serverPort){

  memset(&direccionForanea,0,sizeof(direccionForanea));
  direccionForanea.sin_addr.s_addr = inet_addr(serverIpAdress);
  direccionForanea.sin_family = AF_INET;
  direccionForanea.sin_port = htons(serverPort);
  if (connect(s , (struct sockaddr *)&direccionForanea , sizeof(direccionForanea)) < 0) {
    cout<<strerror(errno);
    close(s);
    std::cout << "[ ERROR ] " << std::tab << "Error en la conexion" << std::endl;
    return 1;
  }else{
    std::cout << "[ SUCCESS ] " << std::tab << "Conectado" << std::endl;
    return 0;
  }


}

int SocketDatagrama::esperaPeticion(){
  //Listen
  listen(s , 3);
  //Accept and incoming connection
  int new_socket;
  std::cout << "[ INFO ] " << std::tab << "Esperando por clientes..." << std::endl;
  int longitudLocal = sizeof(direccionLocal);
  if((new_socket = accept(s, (struct sockaddr *)&direccionLocal,(socklen_t*)&longitudLocal))){
    std::cout << "[ SUCCESS ] " << std::tab << "Conexion establecida satisfactoriamente" << std::endl;
  }
  fflush(stdout);

  if (new_socket<0){
    std::cout << "[ ERROR ] " << std::tab << "Error de aceptacion en el Socket" << std::endl;
    return 1;
  }else{
    return new_socket;
  }
}

int SocketDatagrama::enviaImagen(int socket){
  FILE *picture;
  int size, read_size, stat, packet_index;
  char send_buffer[10240], read_buffer[256];
  packet_index = 1;
  picture = fopen("capture.png", "r");

  if(picture == NULL) {
    std::cout << "[ ERROR ] " << std::tab << "Error al obtener la imagen" << std::endl;
  }

  fseek(picture, 0, SEEK_END);
  size = ftell(picture);
  fseek(picture, 0, SEEK_SET);
  std::cout << "[ INFO ] " << std::tab << "Tamaño total de la imagen: " << size << std::endl;

  std::cout << "[ INFO ] " << std::tab << "Enviando el tamaño de la imagen..." << std::endl;
  write(socket, (void *)&size, sizeof(int));

  std::cout << "[ INFO ] " << std::tab << "Enviando imagen como un Array de Bytes " << std::endl;

  do { //Read while we get errors that are due to signals.
    stat = read(socket, &read_buffer , 255);
    std::cout << "[ INFO ] " << std::tab << "Bytes leidos: " << stat << std::endl << std::endl;
  } while (stat < 0);

  while(!feof(picture)) {
    read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);
    do{
      stat = write(socket, send_buffer, read_size);  
    }while (stat < 0);

    std::cout << "[ INFO ] " << std::tab << "Numero de paquete: " << packet_index << std::endl;
    std::cout << "[ SUCCESS ] " << std::tab << "Longitud de paquete enviado: " << read_size << std::endl; 
    printf(" \n");
    packet_index++;  
    bzero(send_buffer, sizeof(send_buffer));
  }
  return 0;
}

int SocketDatagrama::recibeImagen(){
  int recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;
  char imagearray[10241];
  FILE *image;
  do{
    stat = read(s, &size, sizeof(int));
  }while(stat<0);

  std::cout << "[ SUCCESS ] " << std::tab << "Tamaño de imagen: " << size << std::endl;
  std::cout << "[ SUCCESS ] " << std::tab << "Numero de paquete para recibir: " << stat << std::endl;

  char buffer[] = "Recibido";
  do{
    stat = write(s, &buffer, sizeof(int));
  }while(stat<0);

  //Clona la imagen
  image = fopen("capture2.png", "w");

  if( image == NULL) {
    std::cout << "[ ERROR ] " << std::tab << "Ocurriro un error al clonar la imagen" << std::endl;
  return -1; }
  //Loop while we have not received the entire file yet
  struct timeval timeout = {10,0};
  fd_set fds;
  int buffer_fd;
  while(recv_size < size) {
    FD_ZERO(&fds);
    FD_SET(s,&fds);

    buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

    if (buffer_fd < 0)
      std::cout << "[ ERROR ] " << std::tab << "Mal descriptor de buffer" << std::endl;

    if (buffer_fd == 0)
      std::cout << "[ ERROR ] " << std::tab << "Tiempo para recibir paquete excedido" << std::endl;

    if (buffer_fd > 0){
        do{
          read_size = read(s,imagearray, 10241);
        }while(read_size <0);

        std::cout << "[ INFO ] " << std::tab << "Numero de paquete: " << packet_index << std::endl;
        std::cout << "[ SUCCESS ] " << std::tab << "Longitud de paquete recibido: " << read_size << std::endl;
        //Write the currently read data into our image file
        write_size = fwrite(imagearray,1,read_size, image);
        std::cout << "[ SUCCESS ] " << std::tab << "Escribiendo dato en la imagen: " << write_size << std::endl;
        if(read_size !=write_size) {
          std::cout << "[ ERROR ] " << std::tab << "Error en la lectura de paquetes" << std::endl; 
        }
        //Increment the total number of bytes read
        recv_size += read_size;
        packet_index++;
        std::cout << "[ INFO ] " << std::tab << "Total de tamaño de imagen recibido: " << recv_size << std::endl;
        printf(" \n");
    }
  } 
  fclose(image);
  std::cout << "[ SUCCESS ] " << std::tab << "Imagen recibida correctamente" << std::endl;

  close(s);

  return 1;
}

SocketDatagrama::~SocketDatagrama(){
    close(s);
}