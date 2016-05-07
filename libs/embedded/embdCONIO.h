/*
 *  This file is owned by the Embedded Systems Laboratory of Seoul National University of Science and Technology
 *
 *  2015 Raimarius Delgado 
*/
/****************************************************************************/
/* embdCONIO */
/*****************************************************************************/
#ifndef EMBD_CONIO_H
#define EMBD_CONIO_H
/*****************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/*GETCH*/
/*****************************************************************************/
int getch(){

	struct termios oldt, newt;
	int ch;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~ (ICANON | ECHO);
	tcsetattr (STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return ch;
}

int getche(){
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~ (ICANON);
	tcsetattr (STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}
/*****************************************************************************/
#endif //EMBD_CONIO_H
