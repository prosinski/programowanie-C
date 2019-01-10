/*
 ============================================================================
 Name        : Serwer UDP
 Author      : J.Krygier
 Version     :
 Copyright   :
 Description : Serwer UDP
 ============================================================================
 */


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>       // formaty nazw i adresow sieciowych
#include <sys/types.h>   // typy zmienneych
#include <sys/socket.h>  // gniazda
#include <netinet/in.h>  // typy protokolow
#include <arpa/inet.h>   // adresacja IPv4

#define ROZMIAR_BUFORA 1024

// wyprowadzanie bledow
void blad (char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) {
  int deskryptor_gniazda;
  int nr_portu; 					// nr portu na ktorym serwer nasluchuje
  socklen_t dlugosc_adresu_klienta; // w bajtach (unsigned int)
  struct sockaddr_in serveraddr; 	// adres gniazda serwera
  struct sockaddr_in clientaddr; 	// adres gniazda klienta
  struct hostent *hostp; 			// wskaznik do informacji o kliencie laczacym sie z serwerem
  char buf[ROZMIAR_BUFORA]; 		// bufor na wiadomosci
  char *hostaddrp; 					// wskaznik na tablice przetrzymujaca adres IP (d.d.d.d)
  int optval; 						// 32b flag gniazda do ustawienia przez setsockopt
  int rozmiar_danych; 							// rozmiar wiadomosci [B]

  // mozliwosc wprowadzenia nr portu podczas uruchamiania serwera:
#if 0
  if (argc != 2) {
    fprintf(stderr, "podaj nr portu: %s <port>\n", argv[0]);
    exit(1);
  }
  nr_portu = atoi(argv[1]);
  */
#endif

  // albo ustawmy nr portu nasluchiwania serwera na stale
  nr_portu = 8888; // uwaga: nr portu w UDP i TCP jest max 16b (65536)

  //deskryptor_gniazda = socket(AF_INET, SOCK_DGRAM, 0);
  //albo
  deskryptor_gniazda = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (deskryptor_gniazda < 0)
	  blad ("BLAD podczas otwarcia gniazda");

  /* Za pomoca 'setsockopt ()' ustawimy parametr gniazda pozwalajacy na natychmiastowe
   * jego zwolnienie po wymuszonym przerwaniu programu (Ctrl+Z).
   * W przeciwnym porzypadku ponowne uruchomienie serwera mozliwe byloby po ok. 20s.
   * Unikamy chwilowego zblokowania gniazda i wypisania 'ERROR on binding: Address already in use" error'.
   *
   * int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
   * level:
   * SOL_SOCKET - opcja uzyta na poziomie gniazda (bez wzgledu na typ protokolu)
   * IPPROTO_TCP - opcja uzyta na poziomie protokolu TCP
   * ...
   * optname:
   * SO_REUSEADDR - pozwala na natuchmiastowe ponownie uzycie gniazda
   * SO_BROADCAST - pozwala na wyslanie wiadomosci rogloszeniowych
   * ...
   * uzycie: https://linux.die.net/man/3/setsockopt
   */

  optval = 1;
  setsockopt(deskryptor_gniazda, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));  // w socket.h

  // Ustawienie adresu gniazda serwera
  bzero((char *) &serveraddr, sizeof(serveraddr));  		// wyzerowanie bloku pamieci opisujacego gniazdo (string.h)
  serveraddr.sin_family = AF_INET;				//ustwienie typu protokolu: IPv4
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//kto moze sie laczyc z serwerem: INADDR_ANY - kazdy
  //lub
  //serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");  	//konkretny adres klienta; inet_addr() - przeksztalca 'd.d.d.d' na liczbe 32b (inet.h)
  serveraddr.sin_port = htons((unsigned short)nr_portu); 	//na jakim porcie nasluchuje (in.h)

  //laczymy deskryptor gniazda z jego adresem
  /*
   * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   *
   */
  if (bind(deskryptor_gniazda, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)  //socket.h
	  blad ("BLAD w laczeniu (bind)");
  else
	  printf ("SERWER UDP uruchomiony\n");


  dlugosc_adresu_klienta = sizeof(clientaddr);

  //petla do odbioru danych od klienta i wyslania echo
  while (1) {
    bzero(buf, ROZMIAR_BUFORA); // wyzerowanie bufora

    printf ("SERWER UDP: Oczekuje na dane...\n");

	/* pobranie wiadomosci od klienta */
	/* w socket.h
	 *
	 * recvfrom - funkcja blokujaca -> oczekiwanie do czasu nadejscia danych
	 * ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
             struct sockaddr *src_addr, socklen_t *addrlen);
	 */
    rozmiar_danych = recvfrom(deskryptor_gniazda, buf, ROZMIAR_BUFORA, 0,
		 (struct sockaddr *) &clientaddr, &dlugosc_adresu_klienta);
    if (rozmiar_danych < 0)
    	blad("BLAD w recvfrom");

    // identyfikacja od kogo dane odebrano
    /*
     * struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type);
     * addr - wskaznik na adres IPv4 - liczba 32b
     * len - dlugosc adresu - IPv4 = 32b
     * type - typ protokolu: AF_INET = IPv4
     * struct hostent - adres struktury z informacja o hoscie
     */

    hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
			  sizeof(clientaddr.sin_addr.s_addr), AF_INET);              //netdb.h
    if (hostp == NULL)
    	blad("BLAD pozyskania informacji o kliencie");
    hostaddrp = inet_ntoa(clientaddr.sin_addr);
    if (hostaddrp == NULL)
    	blad("BLAD w inet_ntoa\n");
    printf("SERWER UDP: odebrano dane od %s (%s)\n", hostp->h_name, hostaddrp);
    printf("SERWER UDP: odebrano %d/%d bajtow: %s\n", strlen(buf), rozmiar_danych, buf);

    //wyslemy echo do klienta
  	/* w socket.h
	 * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                      const struct sockaddr *dest_addr, socklen_t addrlen);
	 *
	 */
    rozmiar_danych = sendto(deskryptor_gniazda, buf, strlen(buf), 0,
	       (struct sockaddr *) &clientaddr, dlugosc_adresu_klienta);
    if (rozmiar_danych < 0)
    	blad ("BLAD w sendto");
  }
}

