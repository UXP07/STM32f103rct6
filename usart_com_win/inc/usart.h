#ifndef _USART_H_
#define _USART_H_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h> // 必须包含，用于 _kbhit() 和 _getch()

#define PortClose   0
#define PortOpen    1

void printMeau();
void openPort();
void sendData();
void readData();
void closePort();
int getBaudRateCode();
void sendSparate();
void sendContinue();
void loopSend(HANDLE hSerial, char* data, int len, int nums, int delay_ms);

#endif