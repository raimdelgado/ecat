/*
 *  This file is owned by the Embedded Systems Laboratory of Seoul National University of Science and Technology
 *
 *  2016 Raimarius Delgado
*/
/****************************************************************************/
#ifndef EMBD_MATH_H
#define EMBD_MATH_H
/****************************************************************************/
#include <stdio.h>
#include <math.h>
#include "embdCOMMON.h"
/****************************************************************************/

typedef struct MathSimpleStatistics {
	double ave;
	double max;
	double min;
	double std;
} MATH_STATS;


long MathAbsValI(long value);
long MathArrayMax(long ArraySample[], int ArraySize);
long MathArrayMin(long ArraySample[], int ArraySize);

double MathAbsValF(double value);
double MathArrayAve(long ArraySample[], int ArraySize, int Scaling);
double MathArrayStD(long ArraySample[], int ArraySize, int Scaling);
//double MathArrayMax(long ArraySample[], int ArraySize); /* soon */
//double MathArrayMin(long ArraySample[], int ArraySize); /* soon */



/****************************************************************************/
long MathAbsValI(long AbsValue){
	
	if (AbsValue > 0 ) return AbsValue; 
	else if (AbsValue < 0 ) return -AbsValue; 
	else return 0;

}
double MathAbsValF(double AbsValue){
	
	if (AbsValue > 0 ) return AbsValue; 
	else if (AbsValue < 0 ) return -AbsValue; 
	else return 0;

}
/****************************************************************************/
long MathArrayMax(long ArraySample[], int ArraySize){
	
	int iCnt;
	long MaxVal;

	MaxVal = ArraySample[0];

	for (iCnt=1; iCnt < ArraySize; ++iCnt){

		if(ArraySample[iCnt] > MaxVal){

			MaxVal = ArraySample[iCnt];
		}	
	}
	return MaxVal;
}
/****************************************************************************/
long MathArrayMin(long ArraySample[], int ArraySize){
	
	int iCnt;
	long MinVal;

	MinVal = ArraySample[0];

	for (iCnt=1; iCnt < ArraySize; ++iCnt){

		if(ArraySample[iCnt] < MinVal){

			MinVal = ArraySample[iCnt];
		}	
	}
	return MinVal;
}

/****************************************************************************/
double MathArrayAve(long ArraySample[], int ArraySize, int Scaling){
	
	int iCnt;
	double CumSum=0;
	double AveVal;

	for (iCnt=0; iCnt < ArraySize; ++iCnt){
		CumSum += ((double)ArraySample[iCnt]/Scaling);
	}
	AveVal =((double)CumSum / ArraySize);
//	printf("\n%ld %d %f \n",CumSum,ArraySize,AveVal/SCALE_1M);
	return AveVal;
}
/****************************************************************************/
double MathArrayStD(long ArraySample[], int ArraySize,int Scaling){
	
	int iCnt;
	double AveVal;
	double Variance;
	double StDev;

	AveVal = MathArrayAve(ArraySample, ArraySize,Scaling);

	for (iCnt=0; iCnt < ArraySize; ++iCnt){
		Variance +=((((double)ArraySample[iCnt]/Scaling) - AveVal)) * ((((double)ArraySample[iCnt]/Scaling) - AveVal));
	}
	StDev = sqrt(Variance / ArraySize);
	return StDev;
}
/****************************************************************************/
MATH_STATS GetStatistics(long ArraySample[], int ArraySize,int Scaling){

	MATH_STATS GetStats;	
	long iMax, iMin;
	double fStD;
	double fAve;

	iMax = MathArrayMax(ArraySample, ArraySize);	
	iMin = MathArrayMin(ArraySample, ArraySize);	
	fAve = MathArrayAve(ArraySample, ArraySize,Scaling);	
	fStD = MathArrayStD(ArraySample, ArraySize,Scaling);	
	
	GetStats.ave = fAve;
	GetStats.max = ((double)iMax / Scaling);
	GetStats.min = ((double)iMin / Scaling);
	GetStats.std = fStD;

	return GetStats;
}
/****************************************************************************/
#endif //EMBD_MATH_H


