/*
 ============================================================================
 Name        : projekt2.c
 Author      : Przemysław Rosiński
 Version     :
 Copyright   :
 Description : projekt2 in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>

#include <sys/socket.h>
#include <linux/if_ether.h>

int main(void) {
	printf("Uruchamiam odbieranie ramek Ethernet.\n"); /* prints  */

	//Utworzenie bufora dla odbieranych ramek Ethernet
	char* buffer = (void*) malloc(ETH_FRAME_LEN);

	//Otwarcie gniazda pozwalającego na odbiór wszystkich ramek Ethernet
	int iEthSockHandl = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	//Kontrola czy gniazdo zostało otwarte poprawnie, w przypadku bledu wyświetlenie komunikatu.
	if (iEthSockHandl<0)
			printf("Problem z otwarciem gniazda : %s!\n", strerror(errno));

	//Zmienna do przechowywania rozmiaru odebranych danych
	int iDataLen = 0;

	//Pętla nieskończona do odbierania ramek Ethernet
	while (1) {

		//Odebranie ramki z utworzonego wcześniej gniazda i zapisanie jej do bufora
		iDataLen = recvfrom(iEthSockHandl, buffer, ETH_FRAME_LEN, 0, NULL, NULL);

		//Kontrola czy nie było bledu podczas odbierania ramki
		if (iDataLen == -1)
			printf("Nie moge odebrac ramki: %s! \n", strerror(errno));
		else { //jeśli ramka odebrana poprawnie wyświetlenie jej zawartości
			if ((*(buffer + 12) == 0x08) && (*(buffer + 13) == 0x00) && (*(buffer + 23) == 0x06))
								{
									printf("Odebrano pakiet TCP o rozmiarze %d [B]: \n",iDataLen);

								}
			else
			{		printf("Odebrano  inny pakiet o rozmiarze [B] %d \n",iDataLen);
									}

		}
	}

	return EXIT_SUCCESS;
}

