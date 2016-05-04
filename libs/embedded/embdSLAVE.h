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
/* embdSLAVE */
/****************************************************************************/
#ifndef EMBD_SLAVE_H
#define EMBD_SLAVE_H

#ifdef __cplusplus
extern "C"{
#endif
/* LS Mecapion L7NA004B PROTOCOL */
#define EMBD_CONTROL_WORD			0x6040,0x00
#define SHUTDOWN				0x0006
#define	SWITCH_ON				0x0007
#define	SWITCH_OFF				0x0007
#define	DISABLE_VOLTAGE				0x0000
#define	QUICK_STOP				0x0002
#define	DISABLE_OPERATION			0x0007
#define	ENABLE_OPERATION			0x000F
#define	FAULT_RESET				0x0080

#define	EMBD_STATUS_WORD			0x6041, 0x00
#define	NOT_READY_TO_SWITCH_ON			0x0000
#define	SWITCH_ON_DISABLED			0x0040
#define	READY_TO_SWITCH_ON			0x0021
#define	SWITCH_ON_ENABLED			0x0023
#define	OPERATION_ENABLED			0x0027
#define	QUICK_STOP_ACTIVE			0x0007
#define	FAULT_REACTION_ACTIVE			0x000F
#define	FAULT					0x0008

#define	EMBD_ACTUAL_POSITION			0x6064, 0x00	//	RO,	int32
#define	EMBD_TARGET_POSITION			0x607A, 0x00	//	RW,	int32
#define	EMBD_POSITION_RANGE			0x607B, 0x00	//	R,	uint8
#define	EMBD_MIN_POSITION_RANGE_LIMIT 		0x607B, 0x01	//	RW, int32
#define	EMBD_MAX_POSITION_RANGE_LIMIT 		0x607B, 0x02	//	RW, int32
#define	EMBD_SOFTWARE_POSITION_LIMIT	 	0x607D, 0x00
#define	EMBD_MIN_SOFTWARE_POSITION_LIMIT	0x607D, 0x01	//	RW,	int32
#define	EMBD_MAX_SOFTWARE_POSITION_LIMIT	0x607D, 0x02	//	RW,	int32
#define	EMBD_MAX_PROFILE_VELOCITY		0x607F, 0x00
#define	EMBD_PROFILE_VELOCITY			0x6081, 0x00
#define	EMBD_PROFILE_ACCELERATION		0x6083, 0x00
#define	EMBD_PROFILE_DECELERATION		0x6084, 0x00
#define	EMBD_QUICK_STOP_DECELERATION		0x6085, 0x00
#define	EMBD_402_MOTION_PROFILE_TYPE		0x6086, 0x00
#define	EMBD_INTERPOLATION_TIME_PERIOD		0x60C2, 0x00
#define	EMBD_INTERPOLATION_TIME_UNIT		0x60C2, 0x01	//	RW,	uint8
#define	EMBD_INTERPOLATION_TIME_INDEX		0x60C2, 0x02	//	RW,	int8
#define	EMBD_MAX_ACCELERATION			0x60C5, 0x00	//	RW,	uint32
#define	EMBD_MAX_DECELERATION			0x60C6,	0x00	//	RW, uint32

/****************************************************************************/
const char *getStatusWordString(unsigned short statusWord);
/****************************************************************************/
unsigned short getStatusWordInfo(unsigned short statusWord);
/****************************************************************************/
#ifdef __cplusplus
}
#endif//__CPLUSPLUS
#endif//EMBD_SLAVE_H

