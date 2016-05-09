/*
 *  This file is owned by the Embedded Systems Laboratory of Seoul National University of Science and Technology
 *
 *  2016 Raimarius Delgado
*/
/****************************************************************************/
#ifndef EMBD_SOCKET_H
#define EMBD_SOCKET_H
/****************************************************************************/
#include "embdCOMMON.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*Memory Copy*/
#include <string.h>

/* Socket */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
/****************************************************************************/
#define SCK_DOMAIN		AF_INET
#define SCK_TYPE		SOCK_DGRAM
#define SCK_PROTOCOL		IPPROTO_UDP

#define BITS2BYTE		(8)
#define DGRAM_SIZE		(1464) //optimal size for a datagram
/****************************************************************************/
int  SocketInit(const char *HostIP, short HostPort, const char *RemoteIP, short RemotePort);
void SocketQuit(void);

unsigned char* SocketIntToDgram(int num, int SizeBit); /*LITTLE ENDIAN*/
void SocketCopyToDgram(int OffSet, unsigned char* Message,int SizeBit);
void SocketSendClearDgram(void);
/****************************************************************************/
char SendDgram[DGRAM_SIZE] = 	{0,};
char RecvDgram[DGRAM_SIZE] = 	{0,};

int RemoteSck, HostSck;
int SendSck, RecvSck;
struct sockaddr_in SckAddHostAdd, SckAddRemoteAdd;
/****************************************************************************/
int SocketInit(const char *HostIP, short HostPort, const char *RemoteIP, short RemotePort){

	printf("\nOpening Socket for UDP Communication...");

	if((HostSck = socket(SCK_DOMAIN, SCK_TYPE, SCK_PROTOCOL))<0){
		fprintf(stderr, "Unable to Open UDP Socket from this Host!\n");
		return _EMBD_RET_ERR_;
	}

	SckAddHostAdd.sin_family = SCK_DOMAIN;
	SckAddHostAdd.sin_port = htons(HostPort);
	SckAddHostAdd.sin_addr.s_addr = inet_addr(HostIP);

	if((RemoteSck = socket(SCK_DOMAIN, SCK_TYPE, SCK_PROTOCOL))<0){
		fprintf(stderr, "Unable to Open UDP Socket for Remote!\n");
		return _EMBD_RET_ERR_;
	}

	SckAddRemoteAdd.sin_family = SCK_DOMAIN;
	SckAddRemoteAdd.sin_port = htons(RemotePort);
	SckAddRemoteAdd.sin_addr.s_addr = inet_addr(RemoteIP);


	if((RecvSck = bind(HostSck,(struct sockaddr *)&SckAddHostAdd,sizeof(SckAddHostAdd)))<0){
		fprintf(stderr, "Unable to Open UDP Socket for Remote!\n");
		return _EMBD_RET_ERR_;
	}
	printf("OK\n");
	
	return _EMBD_RET_SCC_;
}
/****************************************************************************/
void SocketQuit(void){
	
	printf("Closing UDP Socket...");

	close(RemoteSck);
	close(HostSck);

	printf("OK!\n");

}
/****************************************************************************/
void SocketCopyToDgram(int OffSet, unsigned char* Message,int SizeBit){

	int SizeByte;
	SizeByte = SizeBit / BITS2BYTE;
	memcpy(SendDgram + OffSet, Message, SizeByte);

}
/****************************************************************************/
void SocketSendClearDgram(void){

	int iDgram;
	SendSck = sendto(RemoteSck, SendDgram,DGRAM_SIZE, 0, 
			(struct sockaddr *)&SckAddRemoteAdd, 
			sizeof(SckAddRemoteAdd));

	for (iDgram = 0; iDgram < DGRAM_SIZE; ++iDgram){
		SendDgram[iDgram] = 0;
	}
}
/****************************************************************************/
unsigned char* SocketIntToDgram(int num, int SizeBit){
	unsigned char* retBytes;
	int i;

		retBytes = (unsigned char*) malloc((SizeBit/BITS2BYTE)*sizeof(unsigned char));

		for(i = 0; i < SizeBit/BITS2BYTE; ++i){

			retBytes[i] = (num >> (BITS2BYTE*i) & 0xff);
		}

	return retBytes;
}
/****************************************************************************/
#endif //EMBD_SOCKET_H




