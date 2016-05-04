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

#include "embdECATM.h"
#include "esiLSMECA.h"
/****************************************************************************/

/* EtherCAT Process Domain */
const ec_pdo_entry_reg_t embdDomain_regs[] = {
	LS_SERVO_REGS(0),
	LS_SERVO_REGS(1),
	{}
};
/****************************************************************************/
/* Private Functions / Variables */
void EcatCheckMasterState(unsigned int *masterState, unsigned int *respondingSlaves);
void EcatCheckSlaveStates(unsigned int *slaveState);
void EcatCheckDomainState(void);
unsigned int syncRefCnt =0;
ec_master_t          	 *eMaster 	= NULL;
ec_master_state_t    	 eMasterState 	= {};

ec_domain_t          	 *eDomain 	= NULL;
ec_domain_state_t    	 eDomainState 	= {};

ec_slave_config_t    	 *slaveConf 	= NULL;
ec_slave_config_state_t  slaveConfState = {};

uint8_t 		 *eDomain_pd 	= NULL;
/****************************************************************************/
const char *EcatStateString(unsigned int al_state){
	if(al_state == INIT) {
		return "INIT";
	}
	else if(al_state == PREOP){
		return "PRE OPERATIONAL";
	}
	else if(al_state == SAFEOP){
		return "SAFE OPERATIONAL";
	}
	else if(al_state == OP){
		return "OPERATIONAL";
	}
	return "Unknown Status";
}
/****************************************************************************/
void EcatCheckDomainState(void){
    ec_domain_state_t ds;
    ecrt_domain_state(eDomain, &ds);
    if (ds.working_counter != eDomainState.working_counter)
        printf("embdDomain Working Counter: %u.\n", ds.working_counter);
    if (ds.wc_state != eDomainState.wc_state)
        printf("embdDomain State: %u.\n", ds.wc_state);
    eDomainState = ds;
}
/*****************************************************************************/
void EcatCheckMasterState(unsigned int *masterState, unsigned int *respondingSlaves){
    ec_master_state_t ms;
    ecrt_master_state(eMaster, &ms);
    if (ms.slaves_responding != eMasterState.slaves_responding)
        printf("%u slave(s).\n", ms.slaves_responding);
    if (ms.al_states != eMasterState.al_states)
        printf("Master State: %s.\n", EcatStateString(ms.al_states));
    if (ms.link_up != eMasterState.link_up)
        printf("Link is %s.\n", ms.link_up ? "up" : "down");
    eMasterState = ms;
    *masterState = ms.al_states;
    *respondingSlaves = ms.slaves_responding;
}
/*****************************************************************************/
void EcatCheckSlaveStates(unsigned int *slaveState){

    ec_slave_config_state_t s;
    ecrt_slave_config_state(slaveConf, &s);


    if (s.al_state != slaveConfState.al_state)
        printf("Slave State: %s.\n", EcatStateString(s.al_state));

    if (s.online != slaveConfState.online)
        printf("Slave is currently %s.\n", s.online ? "online" : "offline");
    if (s.operational != slaveConfState.operational)
        printf("Slave is %sOperational.\n",
                s.operational ? "" : "Not ");

    slaveConfState = s;
    *slaveState = s.al_state;
}
/*****************************************************************************/
ECAT_STAT EcatStatusCheck(void){

	ECAT_STAT EcatCheck;
	unsigned int slaveState = 0, masterState = 0 , respondingSlaves = 0;

	EcatCheckMasterState(&masterState, &respondingSlaves);
	EcatCheckSlaveStates(&slaveState);
	EcatCheckDomainState();

	EcatCheck.master_state	= masterState;
	EcatCheck.slave_state	= slaveState;
	EcatCheck.slave_number	= respondingSlaves;

	return EcatCheck;	
}
/*****************************************************************************/
void EcatWriteAppTimeToMaster(uint64_t masterAppTime){

	ecrt_master_application_time(eMaster, masterAppTime);
		if (syncRefCnt) {
			syncRefCnt--;
		} else {
			syncRefCnt++; // sync every cycle
			ecrt_master_sync_reference_clock(eMaster);
		}
	ecrt_master_sync_slave_clocks(eMaster);
}
/*****************************************************************************/
void EcatReceiveProcessDomain(void){

	ecrt_master_receive(eMaster);
	ecrt_domain_process(eDomain);

}
/*****************************************************************************/
void EcatSendProcessDomain(void){

    	ecrt_domain_queue(eDomain);
	ecrt_master_send(eMaster);

}
/*****************************************************************************/
int EcatInit(int cycleTime, int operationMode){

	int i = 0;

  	/*1.Request Master Instance */
	eMaster = ecrt_request_master(0);
	if (!eMaster){
		return -1;
	}

	/*2.Create Domain */
	eDomain = ecrt_master_create_domain(eMaster);
    	if (!eDomain){
	return -1;
	}

	/*3.Slave Config */
	printf("\nConfiguring PDOs...");

	for(i=0;i<LSMECAPION_SLAVENUM;i++){
		if (!(slaveConf = ecrt_master_slave_config(eMaster, 
							ALIAS_POSITION(i),
							LS_SERVO))){
        		fprintf(stderr, "Failed to get slave configuration.\n");
		       	return -1;
    		}

		/*4. Configure PDOS*/
		if (ecrt_slave_config_pdos(slaveConf, EC_END, lsmecapion_syncs)){
		       	fprintf(stderr, "Failed to configure out PDOs.\n");
        		return -1;
    		}

		/*5. SDO write */
		if (ecrt_slave_config_sdo8(slaveConf,LSMECAPION_OPERATION_MODE, 
					 operationMode)){
	        	fprintf(stderr, "Failed to config SDOs.\n");
	        	return -1;
		}

		/*6. DC setup*/
		ecrt_slave_config_dc(slaveConf, LSMECA_ACTIVATE_WORD, cycleTime, LSMECA_SYNC0_SHIFT, 0, 0);
	}
	printf("OK!\n");

	/*7. Domain Register*/
	if (ecrt_domain_reg_pdo_entry_list(eDomain, embdDomain_regs)) {	
		fprintf(stderr, "PDO entry registration failed!\n");
		return -1;
    	}

	/*8. Start Master and Register Domain */
      	printf("\033[%dm%s\033[0m",94,"\nStarting EtherCAT Master...");
     	
	if (ecrt_master_activate(eMaster)){
	   return -1;
     	}

     	if (!(eDomain_pd = ecrt_domain_data(eDomain))) {
        return -1;

    	}
        
      	printf("\033[%dm%s\033[0m",94,"OK!\n");
 
return 0;
}
/*****************************************************************************/
void EcatQuit(void){

	printf("\033[%dm%s\033[0m",91,"Releasing EtherCAT Master Instance...");

	ecrt_release_master(eMaster);

	printf("\033[%dm%s\033[0m",91,"OK!\n");
}
/*****************************************************************************/
/*LS Mecapion Servo Commands */
/*****************************************************************************/
void lsmecaReady(int iNode)
{
	int i;
	
	if (iNode == _ALLNODE){
		for(i=0;i<LSMECAPION_SLAVENUM;i++){
			EC_WRITE_U16(eDomain_pd + lsmecaCtrlWordOff[i],LSMECA_SWITCH_ON);
		}	
	}
	else{
	
		EC_WRITE_U16(eDomain_pd + lsmecaCtrlWordOff[iNode],LSMECA_SWITCH_ON);
	}
}
/*****************************************************************************/
void lsmecaServoOn(int iNode)
{
	int i;
	
	if (iNode == _ALLNODE){
		for(i=0;i<LSMECAPION_SLAVENUM;i++){
			EC_WRITE_U16(eDomain_pd + lsmecaCtrlWordOff[i],LSMECA_ENABLE_OPERATION);
		}	
	}
	else{
	
		EC_WRITE_U16(eDomain_pd + lsmecaCtrlWordOff[iNode],LSMECA_ENABLE_OPERATION);
	}
}
/*****************************************************************************/
void lsmecaServoOff(int iNode)
{
	int i;
	
	if (iNode == _ALLNODE){
		for(i=0;i<LSMECAPION_SLAVENUM;i++){
			EC_WRITE_U16(eDomain_pd + lsmecaCtrlWordOff[i],LSMECA_DISABLE_OPERATION);

		}	
	}
	else{
	
		EC_WRITE_U16(eDomain_pd + lsmecaCtrlWordOff[iNode],LSMECA_DISABLE_OPERATION);
	}
}
/*****************************************************************************/
void lsmecaShutDown(int iNode)
{
	int i;
	
	if (iNode == _ALLNODE){
		for(i=0;i<LSMECAPION_SLAVENUM;i++){
			EC_WRITE_U16(eDomain_pd + lsmecaCtrlWordOff[i],LSMECA_SHUTDOWN);

		}	
	}
	else{
	
		EC_WRITE_U16(eDomain_pd + lsmecaCtrlWordOff[iNode],LSMECA_SHUTDOWN);
	}
}
/****************************************************************************/
void lsmecaSetVelocity(int iNode, int RevPerMinute)
{
	int i;
	long targetVelocity;

	if (RevPerMinute > APM_FB04AMK_MAXRPM) RevPerMinute = APM_FB04AMK_MAXRPM;
	if (RevPerMinute < -APM_FB04AMK_MAXRPM) RevPerMinute = -APM_FB04AMK_MAXRPM;

	targetVelocity= (long)(LSMECA_19BIT_ENCRES * RPM2RPS(RevPerMinute));

	if (iNode == _ALLNODE){
		for(i=0;i<LSMECAPION_SLAVENUM;i++){
			EC_WRITE_U32(eDomain_pd + lsmecaTargVelOff[i],targetVelocity);

		}	
	}
	else{
		EC_WRITE_U32(eDomain_pd + lsmecaTargVelOff[iNode],targetVelocity);
	}
}
/****************************************************************************/
void lsmecaSetAngularVelocity(int iNode, float AngularVelocity)
{
	int i;
	long targetVelocity;

	targetVelocity= (long)(LSMECA_19BIT_ENCRES * RADPS2RPS(AngularVelocity));

	if (iNode == _ALLNODE){
		for(i=0;i<LSMECAPION_SLAVENUM;i++){
			EC_WRITE_U32(eDomain_pd + lsmecaTargVelOff[i],targetVelocity);

		}	
	}
	else{
		EC_WRITE_U32(eDomain_pd + lsmecaTargVelOff[iNode],targetVelocity);
	}
}
/****************************************************************************/
unsigned short lsmecaGetStatusWordN(int iNode)
{
	unsigned short statusWord;

	statusWord = lsmecaGetStatusVal(EC_READ_U16(eDomain_pd + lsmecaStatWordOff[iNode]));

	return statusWord;
}
/****************************************************************************/
int lsmecaGetActualVelocityN(int iNode)
{
	long currentVelocity; 
	int RevPerMinute;

	currentVelocity = EC_READ_U32(eDomain_pd + lsmecaActlVelOff[iNode]);
	RevPerMinute = RPS2RPM((int)(currentVelocity / LSMECA_19BIT_ENCRES)); 

	return RevPerMinute;
}
/****************************************************************************/
float lsmecaGetActualAngularVelocityN(int iNode)
{
	long currentVelocity; 
	float AngularVelocity;

	currentVelocity = EC_READ_U32(eDomain_pd + lsmecaActlVelOff[iNode]);

	AngularVelocity = RPS2RADPS((float)(currentVelocity/LSMECA_19BIT_ENCRES)); 

	return AngularVelocity;
}
/****************************************************************************/

