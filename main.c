/*
 *  This file is owned by the Embedded Systems Laboratory of Seoul National University of Science and Technology
 *  to test EtherCAT master protocol using the IgH EtherCAT master userspace library.	
 *  
 *  
 *  IgH EtherCAT master library for Linux is found at the following URL: 
 *  <http://www.etherlab.org/en/ethercat>
 *
 *
 *
 *
 *  2015 Raimarius Delgado
*/
/****************************************************************************/
#include "main.h"
/****************************************************************************/
/* Private Functions */
int  XenoInit(void);
void XenoQuit(void);
void DoInput();
void SignalHandler(int signum);
/****************************************************************************/

void EcatCtrlTask(void *arg){

	int iTaskTick = 0;
#ifdef MEASURE_TIMING
	RtmEcatPeriodEnd = rt_timer_read();
#endif
   while (1) {

	   rt_task_wait_period(NULL);

#ifdef MEASURE_TIMING
	   RtmEcatPeriodStart = rt_timer_read();
#endif
    	/* Receive Process Data */
	   EcatReceiveProcessDomain();

    	/* check process data state (optional) */
	   EcatState = EcatStatusCheck(); 

	/*
	 * Do reading of the current process data from here
	 * before processing and sending to Tx
	 */ 

	/* Input Command */
	   DoInput(); 

	/* write application time to master */
   	   RtmEcatMasterAppTime = rt_timer_read();
   	   EcatWriteAppTimeToMaster((uint64_t)RtmEcatMasterAppTime);

	/* send process data */
	   EcatSendProcessDomain();
	   
#if 0 //  0 to omit : 1 for processing

	   if (!(iTaskTick % FREQ_PER_SEC(ECATCTRL_TASK_PERIOD))){
		/*Do every 1 second */

		rt_printf("Task Duration: %d s\n", iTaskTick/FREQ_PER_SEC(ECATCTRL_TASK_PERIOD));	
		}
#endif
#ifdef MEASURE_TIMING	    
	   RtmEcatExecTime = rt_timer_read();
	   if(EcatState.master_state == OP && EcatState.slave_state == OP  ) {

		   if(iBufEcatDataCnt == BUF_SIZE){
			   rt_printf("\nExceeded Queue Size\n");
	   		   lsmecaShutDown(_ALLNODE);
			   quitFlag = 1;
			   break;
		   }
	   	  
		   BufEcatPeriodTime[iBufEcatDataCnt] =  (long)(RtmEcatPeriodStart - RtmEcatPeriodEnd);
	   	   BufEcatExecTime[iBufEcatDataCnt]  =   (long)(RtmEcatExecTime - RtmEcatPeriodStart);
	   	   ++iBufEcatDataCnt;
	   }
#endif

	/*
	 * Do other processes 
	 * after EtherCAT execution 
	 */ 

#ifdef MEASURE_TIMING	    
   	   RtmEcatPeriodEnd = RtmEcatPeriodStart;
#endif
	   iTaskTick++;
    }
}

/****************************************************************************/
void InptCtrlTask(void *arg){

	while (1) {
	
		rt_task_wait_period(NULL);
		
		InptChar = getche(); /* libs/embedded/embdCONIO.h */
   	}
}

/****************************************************************************/

int main(int argc, char **argv){
   	 
	int ret = 0;

	
	/* Interrupt Handler "ctrl+c"  */
	signal(SIGTERM, SignalHandler);
        signal(SIGINT, SignalHandler);	 

	/* EtherCAT Init */
   	if ((ret = EcatInit(ECATCTRL_TASK_PERIOD,LSMECA_CYCLIC_VELOCITY))!=0){
		fprintf(stderr, 
			"Failed to Initiate IgH EtherCAT Master!\n");
		return ret;
	}

	mlockall(MCL_CURRENT|MCL_FUTURE); //Lock Memory to Avoid Memory Swapping

	/* RT-task */

	if ((ret = XenoInit())!=0){
		fprintf(stderr, 
			"Failed to Initiate Xenomai Services!\n");
		return ret;
	}

	while (1) {
		usleep(1);
		if (quitFlag) break;
	}

#ifdef MEASURE_TIMING
#ifdef PRINT_TIMING
	int iCnt;
	FileEcatTiming = fopen("EcatCtrl-Timing.csv", "w");
	for(iCnt=0; iCnt < iBufEcatDataCnt; ++iCnt){

		fprintf(FileEcatTiming,"%ld.%06ld,%ld.%03ld\n",BufEcatPeriodTime[iCnt]/SCALE_1M,
				BufEcatPeriodTime[iCnt]%SCALE_1M,
				BufEcatExecTime[iCnt]/SCALE_1K,
				BufEcatExecTime[iCnt]%SCALE_1K);

	}
	fclose(FileEcatTiming);
#endif

	EcatPeriodStat = GetStatistics(BufEcatPeriodTime, iBufEcatDataCnt,SCALE_1M);  
	printf("\n[Period] Max: %.6f Min: %.6f Ave: %.6f St. D: %.6f\n", EcatPeriodStat.max,
			EcatPeriodStat.min,
			EcatPeriodStat.ave,
			EcatPeriodStat.std);
	EcatExecStat = GetStatistics(BufEcatExecTime, iBufEcatDataCnt,SCALE_1K);  
	printf("[Exec]	 Max: %.3f Min: %.3f Ave: %.3f St. D: %.3f\n", EcatExecStat.max,
			EcatExecStat.min,
			EcatExecStat.ave,
			EcatExecStat.std);


#endif

	XenoQuit();
	EcatQuit();

return ret;
}

/****************************************************************************/

void DoInput(){

	switch(InptChar)
	{
	case 'Q'	:
	case 'q'	:
		lsmecaShutDown(_ALLNODE);
		quitFlag = 1;
		break;
	case 'd'	:
	case 'D'	:
		lsmecaSetVelocity(_ALLNODE,10000);
		break;
	case 'e'	:
	case 'E'	:
		lsmecaSetVelocity(_ALLNODE,-300);
		break;
	case 's'	:
	case 'S'	:
		lsmecaSetAngularVelocity(_ALLNODE,3.4*10);
		break;
	case 'r'	:
	case 'R'	:
		lsmecaReady(_ALLNODE);
		break;
	case 'O'	:
	case 'o'	:
		lsmecaServoOn(_ALLNODE);
		break;
	case 'x'	:
	case 'X'	:
		lsmecaSetVelocity(_ALLNODE,0);
		lsmecaServoOff(_ALLNODE);
		break; 
	default		:
		break;
	}
	InptChar = 0;
}

/****************************************************************************/

void SignalHandler(int signum){
		quitFlag=1;
}

/****************************************************************************/

int XenoInit(void){

	rt_print_auto_init(1); //RTDK

	printf("Creating Xenomai Realtime Task(s)...");
	if(rt_task_create(&TskEcatCtrl,"EtherCAT Control", 0, ECATCTRL_TASK_PRIORITY,ECATCTRL_TASK_MODE)){
	
        		fprintf(stderr, "Failed to create Ecat Control Task\n");
			return _EMBD_RET_ERR_;
	
	}

	if(rt_task_create(&TskInptCtrl,"Input Control", 0, INPTCTRL_TASK_PRIORITY,ECATCTRL_TASK_MODE)){
	
	 		fprintf(stderr, "Failed to create Input Control Task\n");
			return _EMBD_RET_ERR_;

	}

	printf("OK!\n");


	printf("Making Realtime Task(s) Periodic...");
	if(rt_task_set_periodic(&TskEcatCtrl, TM_NOW,rt_timer_ns2ticks(ECATCTRL_TASK_PERIOD))){
	
	 		fprintf(stderr, "Failed to Make Ecat Control Task Periodic\n");
			return _EMBD_RET_ERR_;

	}	
	if(rt_task_set_periodic(&TskInptCtrl, TM_NOW,rt_timer_ns2ticks(INPTCTRL_TASK_PERIOD))){
	
	 		fprintf(stderr, "Failed to Make Input Control Task Periodic\n");
			return _EMBD_RET_ERR_;

	}
	printf("OK!\n");

	printf("Starting Xenomai Realtime Task(s)...");
	if(rt_task_start(&TskEcatCtrl, &EcatCtrlTask, NULL)){
	
	 		fprintf(stderr, "Failed to start Ecat Control Task\n");
			return _EMBD_RET_ERR_;

	}
	if(rt_task_start(&TskInptCtrl, &InptCtrlTask, NULL)){
	
	 		fprintf(stderr, "Failed to start Input Control Task\n");
			return _EMBD_RET_ERR_;

	}
	printf("OK!\n");

	return _EMBD_RET_SCC_;
	
}

/****************************************************************************/

void XenoQuit(void){
	
	rt_task_suspend(&TskInptCtrl);
	rt_task_suspend(&TskEcatCtrl);

	rt_task_delete(&TskInptCtrl);
	rt_task_delete(&TskEcatCtrl);
	printf("\033[%dm%s\033[0m\n",95,"\nXenomai Task(s) Deleted!");

}

/****************************************************************************/


