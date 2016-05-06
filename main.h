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
/* main.h */
/****************************************************************************/
#include <embdECATM.h> //./libs/ecatservo 
#include <embdCONIO.h>  //./libs/embedded
/*****************************************************************************/
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <malloc.h>
#include <pthread.h>
/*****************************************************************************/
/* Xenomai */
/*****************************************************************************/
#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h> //rt_printf
/*****************************************************************************/
/* Real-time Task */
/*****************************************************************************/
#define ECATCTRL_TASK_MODE	T_FPU|T_CPU(0)
#define ECATCTRL_TASK_PRIORITY	(99) // xeno: 99 , preempt: 80
#define ECATCTRL_TASK_PERIOD	(1000000L)

#define INPTCTRL_TASK_PRIORITY	(80) // xeno: 99 , preempt: 80
#define INPTCTRL_TASK_PERIOD	(10000000L)


RT_TASK TskEcatCtrl;
RT_TASK TskInptCtrl;

RTIME 	RtmEcatMasterAppTime;

//#define MEASURE_TIMING
#ifdef MEASURE_TIMING
RTIME RtmEcatPeriodStart=0, RtmEcatPeriodEnd=0, RtmEcatExecTime=0; 
#endif

/*****************************************************************************/
/* global variables */
/*****************************************************************************/
#define	NANOSEC_PER_SEC		(1000000000L)
#define FREQ_PERSEC(x)		(NANOSEC_PER_SEC/(x))

/* ECAT_STAT:
 * unsigned int  master_state;
 * unsigned int	 slave_state;
 * unsigned int	 slave_number */

ECAT_STAT	EcatStat;

int		quitFlag	= 0;
char		InptChar	= 0; 
/*****************************************************************************/



