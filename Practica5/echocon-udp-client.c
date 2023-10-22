#include <stdio.h>	//Para usar perror
#include <stdlib.h>	//Para usar EXIT_FAILURE
#include <string.h>
#include <unistd.h> 
#include <errno.h>	//Para errno 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int main (int argc, char *argv[]){
    
    int sock, cadlen;
    struct sockaddr_in host_addr,server_addr; 
    struct in_addr addr; //Direccion IP del servidor en formato de 32 bits
    uint16_t portshort; 
    char *cadena;
    char *cadRecv;
    socklen_t server_size;

    //Creacion del socket en el cliente
    sock= socket(AF_INET,SOCK_DGRAM,0);
    if(sock<0){
        //Comprobamos si la creacion del socket ha sido correcta
        perror("socket()  ha fallado"); // Imprime codigo de error y finaliza.
        exit(EXIT_FAILURE);
    }

    //Asignacion de la direccion del cliente(local) 
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = 0;
    host_addr.sin_addr.s_addr = INADDR_ANY;
    
    //Realizar el bind del socket a la configuracion del socket
    if (bind(sock, (struct sockaddr *)&host_addr, sizeof(host_addr)) == -1) {
        perror("bind() ha fallado");
        exit(EXIT_FAILURE);
    }

    //Obtencion del puerto del servidor
    if(argc==5){
        //Si se incluye el puerto como parametro
        sscanf(argv[3],"%hu",&portshort);
        cadlen = strlen(argv[4]);
        cadena = (char *) malloc (cadlen*sizeof(char));
        strcpy(cadena,argv[4]); 
    }else if(argc==3){
        //Si no se incluye el puerto como parametro
        portshort=5;
        cadlen = strlen(argv[2]);
        cadena = (char *) malloc (cadlen*sizeof(char));
        strcpy(cadena,argv[2]);
    }else{
        printf("El numero de argumentos es incorrecto\n");
        exit(EXIT_FAILURE);
    }
    
    //Transformacion de la direccion IP a 32 bits
    if(inet_aton(argv[1], &addr)==0){
        perror("inet_aton()");
        exit(EXIT_FAILURE);
    }

    //Configuracion del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portshort);
    server_addr.sin_addr = addr; 

    //Comprobacion del tamaño de la cadena
    if(cadlen>80){
        perror("Es una cadena demasidado larga");
        exit(EXIT_FAILURE);
    }
    //Mandamos la cadena al servidor
   if(sendto(sock, cadena ,cadlen, 0,(struct sockaddr*)&server_addr, (socklen_t)sizeof(server_addr))<0){
		perror("sendto() ha fallado");
		exit(EXIT_FAILURE);
	}
    //Reservamos memoria para la cadena de recepcion
    cadRecv = (char*) malloc (cadlen);
    if(cadRecv==NULL){
        perror("Error reservando memoria para la cadena recibida");
        exit(EXIT_FAILURE);
    }
    //Recibimos la cadena transformada del servidor
    if(recvfrom(sock, cadRecv, cadlen, 0,(struct sockaddr*)&server_addr, &server_size)<0){
		perror("recvfrom() ha fallado");
		exit(EXIT_FAILURE);
	}

    //Imprimir el resultado por pantalla.
	printf("Cadena recibida del servidor: %s\n",cadRecv);
	fflush(stdout);

	//Cerrar conexion.
	if(close(sock)<0){
		perror("Error al cerrar la conexion");
		exit(EXIT_FAILURE);
	}
    
    //Liberamos la memoria de ambas cadenas
    free(cadRecv);
    free(cadena);

	//Terminar proceso sin errores.
    
	return 0;
}