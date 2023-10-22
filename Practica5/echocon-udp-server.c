#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>


#define MAX_CADENA 80

int main(int argc, char *argv[]){

    int sockServer;
    char *cadRecv;
    char *cadSend;
    uint16_t portshort;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	socklen_t clientLen=sizeof(client_addr);
	size_t cadlen;

	/*Obtener puerto servidor.
	 * Si no nos lo dan, utilizar el puerto 5.
	 * Si nos lo dan, obtenerlo y usar htons() para pasar a network byte order.*/
    if(argc==3){
        sscanf(argv[2],"%hu",&portshort);
    }else if(argc==1){
        portshort=5;
    }else{
        printf("El numero de argumentos es incorrecto\n");
        exit(EXIT_FAILURE);
    }

	//Crear socket para servidor y obtener descriptor.
	sockServer = socket(AF_INET, SOCK_DGRAM, 0);
	
	//Control de error para la generacion de descriptor de socket.
	if(sockServer < 0){
		perror("socket() ha fallado");
		exit(EXIT_FAILURE);
	}
	
	//Asignacion atributos direccion servidor.
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port=htons(portshort);

	//Bind para enlazar socket a la direccion local (el servidor)
	if(bind(sockServer, (struct sockaddr*) &server_addr, sizeof(server_addr))<0){
		perror("bind() ha fallado");
		exit(EXIT_FAILURE);
	}

	cadRecv= (char*)malloc((MAX_CADENA+1)*sizeof(char)); //Se reserva el tamaño de 80 caracteres mas 1 extra para el final de cadena
	if(cadRecv==NULL){
		perror("Error al reservar memoria para la cadena recibida");
        exit(EXIT_FAILURE);
	}

	//Socket UDP se queda escuchando a la espera de recibir datagramas.
	while(1){

		//Recibimos la cadena del cliente y rellenamos su informacion para saber donde mandarla tranformada
		if(recvfrom(sockServer, cadRecv ,MAX_CADENA+1, 0, (struct sockaddr*)&client_addr, &clientLen)<0){
			perror("recvfrom() ha fallado");
			exit(EXIT_FAILURE);
		}

		//Longitud de la cadena 
		cadlen=strlen(cadRecv);

		cadSend=(char*) malloc (cadlen);
		if(cadSend==NULL){
			perror("Error al reservar memoria para la cadena respuesta");
        	exit(EXIT_FAILURE);
		}
		
		//Recorremos la cadena
		int i; 
		for(i=0;i<cadlen;i++){
	i		if (islower(cadRecv[i])) {
				cadSend[i] = toupper(cadRecv[i]);//Si es minuscula la pasamos a mayusculas
			} else if (isupper(cadRecv[i])) {
				cadSend[i] = tolower(cadRecv[i]);//Si es mayuscula la pasamos a minusculas
			}else{
				cadSend[i]=cadRecv[i]; //Cualquier otro caracter se copia
			}
    	}
		
		cadSend[cadlen]='\0';//Caracter de fin de cadena 
		
		//Enviar a cliente la cadena cambiando las mayusculas por minusculas
		if(sendto(sockServer, cadSend, cadlen, 0,(struct sockaddr*) &client_addr,clientLen)<0){
			perror("sendto() ha fallado");
			exit(EXIT_FAILURE);
		}

		//Liberamos de nuevo la cadena recibida para volver a reservarla
		free(cadSend);
	}

	//Cerrar conexion
	if(close(sockServer) < 0){
		perror("Error al cerrar la conexion");
		exit(EXIT_FAILURE);
	}

	//Liberamos la memoria reservada
	free(cadRecv);
	
	//Terminar proceso sin errores.
	exit(0);
}
