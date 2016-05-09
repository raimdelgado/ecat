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
	float ave;
	float max;
	float min;
	float std;
} MATH_STATS;


int MathAbsValI(int value);
int MathArrayMaxI(int ArraySample[], int ArraySize);
int MathArrayMinI(int ArraySample[], int ArraySize);

float MathAbsValF(float value);
float MathArrayAve(int ArraySample[], int ArraySize, int Scaling);
float MathArrayStD(int ArraySample[], int ArraySize, int Scaling);

//float MathArrayMaxF(int ArraySample[], int ArraySize); /* soon */
//float MathArrayMinF(int ArraySample[], int ArraySize); /* soon */



/****************************************************************************/
int MathAbsValI(int AbsValue){
	
	if (AbsValue > 0 ) return AbsValue; 
	else if (AbsValue < 0 ) return -AbsValue; 
	else return 0;

}
float MathAbsValF(float AbsValue){
	
	if (AbsValue > 0 ) return AbsValue; 
	else if (AbsValue < 0 ) return -AbsValue; 
	else return 0;

}
/****************************************************************************/
int MathArrayMaxI(int ArraySample[], int ArraySize){
	
	int iCnt;
	int MaxVal;

	MaxVal = ArraySample[0];

	for (iCnt=1; iCnt < ArraySize; ++iCnt){

		if(ArraySample[iCnt] > MaxVal){

			MaxVal = ArraySample[iCnt];
		}	
	}
	return MaxVal;
}
/****************************************************************************/
int MathArrayMinI(int ArraySample[], int ArraySize){
	
	int iCnt;
	int MinVal;

	MinVal = ArraySample[0];

	for (iCnt=1; iCnt < ArraySize; ++iCnt){

		if(ArraySample[iCnt] < MinVal){

			MinVal = ArraySample[iCnt];
		}	
	}
	return MinVal;
}

/****************************************************************************/
float MathArrayAve(int ArraySample[], int ArraySize, int Scaling){
	
	int iCnt;
	float CumSum=0;
	float AveVal;

	for (iCnt=0; iCnt < ArraySize; ++iCnt){
		CumSum += ((float)ArraySample[iCnt]/Scaling);
	}
	AveVal =((float)CumSum / ArraySize);
	return AveVal;
}
/****************************************************************************/
float MathArrayStD(int ArraySample[], int ArraySize,int Scaling){
	
	int iCnt;
	float AveVal;
	float Variance;
	float StDev;

	AveVal = MathArrayAve(ArraySample, ArraySize,Scaling);

	for (iCnt=0; iCnt < ArraySize; ++iCnt){
		Variance +=((((float)ArraySample[iCnt]/Scaling) - AveVal)) * ((((float)ArraySample[iCnt]/Scaling) - AveVal));
	}
	StDev = sqrt(Variance / ArraySize);
	return StDev;
}
/****************************************************************************/
MATH_STATS GetStatistics(int ArraySample[], int ArraySize,int Scaling){

	MATH_STATS GetStats;	
	int iMax, iMin;
	float fStD;
	float fAve;

	iMax = MathArrayMaxI(ArraySample, ArraySize);	
	iMin = MathArrayMinI(ArraySample, ArraySize);	
	fAve = MathArrayAve(ArraySample, ArraySize,Scaling);	
	fStD = MathArrayStD(ArraySample, ArraySize,Scaling);	
	
	GetStats.ave = fAve;
	GetStats.max = ((float)iMax / Scaling);
	GetStats.min = ((float)iMin / Scaling);
	GetStats.std = fStD;

	return GetStats;
}
/****************************************************************************/
#endif //EMBD_MATH_H


