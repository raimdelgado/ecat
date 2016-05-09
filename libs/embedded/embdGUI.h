/*
 *  This file is owned by the Embedded Systems Laboratory of Seoul National University of Science and Technology
 *  
 *  2016 Raimarius Delgado
*/
/****************************************************************************/
#ifndef EMBD_GUI_H
#define EMBD_GUI_H
/****************************************************************************/
#include "embdCOMMON.h"
#include "embdSOCKET.h"

/*GUI Protocol*/
#define PTL_ECAT_STATE		"M"
#define PTL_TIME_MONITOR	"P"
#define PTL_SLAVE_INFO		"S"
#define PTL_SLAVE_DIV		"X"
#define PTL_SLAVE_FIN		"Z"

/*Offfsets*/
#define OFFSET_ECAT_STATE	(0)
#define OFFSET_TIME_MONITOR	(4)
#define OFFSET_SLAVE_INFO	(17)

/*Size*/
#define SIZE_BYTE		(8)
#define SIZE_2BYTES		(16)
#define SIZE_3BYTES		(24)
#define SIZE_4BYTES		(32)
/****************************************************************************/
int  GUIConnect(const char *HostIP, short HostPort, const char *RemoteIP, short RemotePort);
void GUIDisconnect(void);
void GUISendDgram(void);

void GUIEcatState(unsigned int MasterState,unsigned int SlaveState,unsigned int SlaveNumber);
void GUISlaveInformation(int **DataGrid ,int SlaveNumber,int DataNumber);
void GUITimingMonitor(int EcatPeriod,int EcatExecution,int EcatJitter);
/****************************************************************************/
int GUIConnect(const char *HostIP, short HostPort, const char *RemoteIP, short RemotePort){
	
	if(SocketInit(HostIP, HostPort, RemoteIP, RemotePort)){
		fprintf(stderr, "Connection to GUI Failed!\n");
		return _EMBD_RET_ERR_;
	}
	printf("Connected to GUI at [%s]!\n",RemoteIP);
	return _EMBD_RET_SCC_;
}
/****************************************************************************/
void GUIDisconnect(void){
	
	SocketQuit();
	printf("Disconnected from GUI!\n");
	
}
/****************************************************************************/
void GUISendDgram(void){
	
	SocketSendClearDgram();
	
}
/****************************************************************************/

void GUIEcatState(unsigned int MasterState,unsigned int SlaveState,unsigned int SlaveNumber){
	
	SocketCopyToDgram(OFFSET_ECAT_STATE,(unsigned char*)PTL_ECAT_STATE,SIZE_BYTE);
	SocketCopyToDgram(OFFSET_ECAT_STATE+1,SocketIntToDgram(MasterState,SIZE_BYTE),SIZE_BYTE);
	SocketCopyToDgram(OFFSET_ECAT_STATE+2,SocketIntToDgram(SlaveState,SIZE_BYTE),SIZE_BYTE);
	SocketCopyToDgram(OFFSET_ECAT_STATE+3,SocketIntToDgram(SlaveNumber,SIZE_BYTE),SIZE_BYTE);

}
/****************************************************************************/
void GUITimingMonitor(int EcatPeriod,int EcatExecution,int EcatJitter){
	
	SocketCopyToDgram(OFFSET_TIME_MONITOR,(unsigned char*)PTL_TIME_MONITOR,SIZE_BYTE);
	SocketCopyToDgram(OFFSET_TIME_MONITOR+1,SocketIntToDgram(EcatPeriod,SIZE_4BYTES),SIZE_4BYTES);
	SocketCopyToDgram(OFFSET_TIME_MONITOR+5,SocketIntToDgram(EcatExecution,SIZE_4BYTES),SIZE_4BYTES);
	SocketCopyToDgram(OFFSET_TIME_MONITOR+9,SocketIntToDgram(EcatJitter,SIZE_4BYTES),SIZE_4BYTES);

}
/****************************************************************************/
void GUISlaveInformation(int **DataGrid ,int SlaveNumber,int DataNumber){
	int iSlvNum, iDataNum, iOffSet;

	iOffSet = 0;
	SocketCopyToDgram(OFFSET_SLAVE_INFO+iOffSet,(unsigned char*)PTL_SLAVE_INFO,SIZE_BYTE);
	for(iSlvNum=0;iSlvNum<SlaveNumber;++iSlvNum){
		++iOffSet;
		for(iDataNum=0;iDataNum<DataNumber;++iDataNum){
			SocketCopyToDgram(OFFSET_SLAVE_INFO+iOffSet,SocketIntToDgram(DataGrid[iSlvNum][iDataNum],SIZE_4BYTES),SIZE_4BYTES);
			iOffSet+=4;
		}
		SocketCopyToDgram(OFFSET_SLAVE_INFO+iOffSet,(unsigned char*)PTL_SLAVE_DIV,SIZE_BYTE);
	}
	++iOffSet;
	SocketCopyToDgram(OFFSET_SLAVE_INFO+iOffSet,(unsigned char*)PTL_SLAVE_FIN,SIZE_BYTE);
}
/****************************************************************************/
#endif // EMBD_GUI_H
