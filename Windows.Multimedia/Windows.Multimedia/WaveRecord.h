#pragma once
#include "resource.h"
#pragma comment(lib, "winmm.lib")
#include "mmsystem.h"

BOOL hesInNumDrive();
BOOL canGetInDevCaps();
void writeInFormat(WAVEFORMATEX waveFormat, FILE* fp);
DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
void startRecord(const char* fileName, WAVEFORMATEX pwfx);
void stopRecord();

