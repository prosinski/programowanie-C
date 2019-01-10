/*
 ============================================================================
 Name        : Klient_UDP
 Author      : J.Krygier
 Version     :
 Copyright   :
 Description : Klinet UDP
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h> //dla inet_addr

#define BUFSIZE 1024

// wyprowadzanie bledow
void blad(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int 	deskryptor_gniazda, nr_portu, rozmiar_danych;
    socklen_t dlugosc_adresu_serwera;
    struct 	sockaddr_in serveraddr;
    struct 	hostent *server;
    char 	*nazwa_hosta;
    char 	buf[BUFSIZE];

/*Wlacz jesli nazwa serwera i nr portu bedzie wprowadzana przy starcie.
Jesli wprowadzana bedzie nazwa domenowa, w programie odblokowac uzycie funkcji gethostbyname()*/
#if 0
    /* sparwdz czy podano adres serwera i port*/
   if (argc != 3) {
       fprintf(stderr,"podaj: %s <nazwe serwera> <port>\n", argv[0]);
       exit(0);
    }
    nazwa_hosta = argv[1];
    nr_portu = atoi(argv[2]);
#endif

    // mozna tez na sztywno podac nazwe serwera i port
    //nazwa_hosta = "localhost"; //do uzycia w fukcji gethostbyname()
    nr_portu = 8888;


    //sockfd = socket(AF_INET, SOCK_DGRAM, 0); // mozna tez z domyslnym protokolem
    deskryptor_gniazda = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (deskryptor_gniazda < 0)
    	blad("BLAD w otwarciu gniazda");

    /* gethostbyname(): zapytanie do serwera DNS o adres serwera*/
    /*wylaczyc jesli adres IP serwera podany bedzie loklanie*/
#if 0
    server = gethostbyname(nazwa_hosta);
    if (server == NULL) {
        fprintf(stderr,"BLAD, nie ma takiego hosta %s\n", nazwa_hosta);
        exit(0);
    }
#endif

    /* Konfiguracja adresu gniazda serwera UDP */
    // ropoczynamy od wyczyszczenia gniazda serwera
    bzero((char *) &serveraddr, sizeof(serveraddr)); //to samo co 'memset () do wyzerowania bloku pamieci

    //USTAWIENIE typu protokolu: IPv4
    serveraddr.sin_family = AF_INET;

    //USTAWIENIE adresu IP serwera z pobranego z DNS
    //bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length); //prawe to samo co 'memcpy'
    //albo bezposrenido podanie adresu IP serwera:
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");  //inet_addr przeksztalca string do liczby 32b

    //USTAWIENIE nr portu, na ktorym nasluchuje serwer
    serveraddr.sin_port = htons(nr_portu);

    while (1) {
    	/* get a message from the user */
    	bzero(buf, BUFSIZE);
    	printf("Wprowadz wiadmomosc: ");
    	fgets(buf, BUFSIZE, stdin);

    	/* Wyslanie wiadomosci do serwera UDP */
      	/*
    	 * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                          const struct sockaddr *dest_addr, socklen_t addrlen);
    	 *
    	 */

    	dlugosc_adresu_serwera = sizeof(serveraddr);
    	rozmiar_danych = sendto(deskryptor_gniazda , buf, strlen(buf), 0,
    			               (struct sockaddr *) &serveraddr, dlugosc_adresu_serwera);
    	if (rozmiar_danych < 0)
    		blad("BLAD wysylania danych do serwera");

    	/* pobranie wiadomosci z serwera */
    	/*
    	 * recvfrom - funkcja blokujaca -> oczekiwanie do czasu nadejscia danych
    	 * ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
    	 */
    	rozmiar_danych = recvfrom(deskryptor_gniazda , buf, strlen(buf), 0,
    			                 (struct sockaddr *) &serveraddr, &dlugosc_adresu_serwera);
    	if (rozmiar_danych < 0)
    		blad("BLAD odbioru z gniazda");
    	printf("ECHO z serwera UDP: %s", buf);
    }
    return 0;
}

