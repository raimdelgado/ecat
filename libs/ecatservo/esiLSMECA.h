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
 *  2016 Raimarius Delgado
*/
/****************************************************************************/
#ifndef LSMECAPION_ESI_H
#define LSMECAPION_ESI_H

#include "embdECATM.h"

/****************************************************************************/

#define LS_SERVO 0x00007595, 0x00000000 // Vendor ID , Product Code

#define LS_SERVO_REGS(x) \
	{ALIAS_POSITION(x), LS_SERVO, LSMECAPION_CONTROL_WORD , &lsmecaCtrlWordOff[x]}, \
	{ALIAS_POSITION(x), LS_SERVO, LSMECAPION_TARGET_VELOCITY, &lsmecaTargVelOff[x]}, \
	{ALIAS_POSITION(x), LS_SERVO, LSMECAPION_STATUS_WORD, &lsmecaStatWordOff[x]}, \
	{ALIAS_POSITION(x), LS_SERVO, LSMECAPION_ACTUAL_POSITION, &lsmecaActlPosOff[x]}, \
	{ALIAS_POSITION(x), LS_SERVO, LSMECAPION_ACTUAL_VELOCITY, &lsmecaActlVelOff[x]}

/****************************************************************************/

unsigned int lsmecaCtrlWordOff[LSMECAPION_SLAVENUM]	=	{0,};
unsigned int lsmecaTargVelOff[LSMECAPION_SLAVENUM]	=	{0,};
unsigned int lsmecaStatWordOff[LSMECAPION_SLAVENUM]	=	{0,};
unsigned int lsmecaActlPosOff[LSMECAPION_SLAVENUM]	=	{0,};
unsigned int lsmecaActlVelOff[LSMECAPION_SLAVENUM]	=	{0,};

/*****************************************************************************/

ec_pdo_entry_info_t lsmecapion_pdo_entries[] = {
    {LSMECAPION_CONTROL_WORD,16}, // Controlword 
    {LSMECAPION_TARGET_VELOCITY,32}, // Target Velocity 
    {LSMECAPION_STATUS_WORD,16}, // Statusword 
    {LSMECAPION_ACTUAL_POSITION,32}, // Position Actual Value 
    {LSMECAPION_ACTUAL_VELOCITY,32}, // Velocity Actual Value 
};
ec_pdo_info_t lsmecapion_pdos[] = {
    {0x1601, 2, lsmecapion_pdo_entries + 0}, // Tx PDO Mapping 
    {0x1a01, 3, lsmecapion_pdo_entries + 2}, // Rx PDO Mapping 
};
ec_sync_info_t lsmecapion_syncs[] = {
    {0, EC_DIR_OUTPUT, 	0, NULL, 		EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 	0, NULL, 		EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 	1, lsmecapion_pdos + 0, EC_WD_DISABLE},
    {3, EC_DIR_INPUT, 	1, lsmecapion_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

/*****************************************************************************/

unsigned short lsmecaGetStatusVal(unsigned short statusWord)
{
	if((statusWord & 0x4F) == 0x00) {
		statusWord = LSMECA_NOT_READY_TO_SWITCH_ON;
	}
	else if((statusWord & 0x4F) == 0x08) {
		statusWord =  LSMECA_FAULT;
	}
	else if((statusWord & 0x4F) == 0x40) {
		statusWord =  LSMECA_SWITCH_ON_DISABLED;
	}
	else if((statusWord & 0x6F) == 0x27) {
		statusWord =  LSMECA_OPERATION_ENABLED;
	}
	else if((statusWord & 0x6F) == 0x23) {
		statusWord =  LSMECA_SWITCH_ON_ENABLED;
	}
	else if((statusWord & 0x6F) == 0x21) {
		statusWord =  LSMECA_READY_TO_SWITCH_ON;
	}
	else if((statusWord & 0x6F) == 0x07) {
		statusWord =  LSMECA_QUICK_STOP_ACTIVE;
	}
	else if((statusWord & 0x4F) == 0x0F) {
		statusWord =  LSMECA_FAULT_REACTION_ACTIVE;
	}
	else {
		return 0xFFFF;
	}
	return statusWord;
}

const char* lsmecaGetStatusStr(unsigned short statusWord)
{
	if((statusWord & 0x4F) == 0x00) {
		return "Not ready to Switch On";
	}
	else if((statusWord & 0x4F) == 0x08) {
		return "Fault";
	}
	else if((statusWord & 0x4F) == 0x40) {
		return "Switch Off";
	}
	else if((statusWord & 0x6F) == 0x27) {
		return "Operation Enabled";
	}
	else if((statusWord & 0x6F) == 0x23) {
		return "Switch On Enabled";
	}
	else if((statusWord & 0x6F) == 0x21) {
		return "Ready to Switch On";
	}
	else if((statusWord & 0x6F) == 0x07) {
		return "Quick Stop Activate";
	}
	else if((statusWord & 0x4F) == 0x0F) {
		return "Fault Reaction Activate";
	}else{
	
	return "Unknown Status";
	}
}


/*****************************************************************************/
#endif //LSMECAPION_ESI_H

