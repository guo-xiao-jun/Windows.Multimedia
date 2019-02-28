
#include "stdafx.h"
#include <stdio.h>
#include "WaveRecord.h"

#define MAX_LOADSTRING	100
#define BLOCK			1024					//定义块的大小
#define CALLLENGTH		(BLOCK*10)				//设置触发回调函数的字节长度

//录音格式定义
//WAVEFORMATEX pwfx = {
//	WAVE_FORMAT_PCM,// wFormatTag，格式标志
//	1,			// nChannels，通道数，单声道数据用单通道，立体声通道用双通道
//	16000,		// nSamplesPerSec，采样率（HZ），每秒钟采取样本的次数
//	32000,		// nAvgBytesPerSec，每秒转换数据的字节数，，nAvgBytesPerSec = nSamplesPerSec *  nChannels * nBlockAlign。此处的大小与waveInOopen回调函数中写入数据的大小应该一直，否则会出现问题
//	2,			// nBlockAlign，每个样本的字节数，nBlockAlign = (nChannels × wBitsPerSample) / 8
//	16,			// wBitsPerSample，每个样本的位数，必须等于8或者16
//	0			// cbSize，附加在该结构体后面的格式信息的大小
//};

FILE *fpWriteWAV = NULL;
BOOL bFlag = TRUE;
HWAVEIN phwi;

BOOL hesInNumDrive()
{
	return waveInGetNumDevs() < 1 ? FALSE : TRUE;
}

BOOL canGetInDevCaps()
{
	WAVEINCAPS waveIncaps;
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2 设备描述
	if (MMSYSERR_NOERROR != mmResult)
	{
		return FALSE;
	}
	return TRUE;
}

void writeInFormat(WAVEFORMATEX waveFormat, FILE *fp)
{
	fseek(fp, 0L, SEEK_SET);
	fwrite("RIFF", 1, 4, fp);
	unsigned int dwDataLength = BLOCK * BLOCK;
	unsigned int waveFormatSize = sizeof WAVEFORMATEX;
	unsigned int Sec = (dwDataLength + waveFormatSize);
	fwrite(&Sec, sizeof(Sec), 1, fp);
	fwrite("WAVE", 4, 1, fp);
	fwrite("fmt ", 1, 4, fp);
	fwrite(&waveFormatSize, sizeof(waveFormatSize), 1, fp);
	fwrite(&waveFormat.wFormatTag, sizeof(waveFormat.wFormatTag), 1, fp);
	fwrite(&waveFormat.nChannels, sizeof(waveFormat.nChannels), 1, fp);
	fwrite(&waveFormat.nSamplesPerSec, sizeof(waveFormat.nSamplesPerSec), 1, fp);
	fwrite(&waveFormat.nAvgBytesPerSec, sizeof(waveFormat.nAvgBytesPerSec), 1, fp);
	fwrite(&waveFormat.nBlockAlign, sizeof(waveFormat.nBlockAlign), 1, fp);
	fwrite(&waveFormat.wBitsPerSample, sizeof(waveFormat.wBitsPerSample), 1, fp);
	fwrite(&waveFormat.cbSize, sizeof(waveFormat.cbSize), 1, fp);
	fwrite("data", 4, 1, fp);
	fwrite(&dwDataLength, sizeof(dwDataLength), 1, fp);
}

DWORD CALLBACK MicCallback(HWAVEIN hwavein, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (uMsg)
	{
	case WIM_OPEN:
		break;
	case WIM_DATA:
	{
		WAVEHDR *pWaveHdr = (WAVEHDR*)dwParam1;
		waveInPrepareHeader(hwavein, pWaveHdr, sizeof WAVEHDR);
		waveInAddBuffer(hwavein, pWaveHdr, sizeof(WAVEHDR));
		int bufferSize = pWaveHdr->dwBufferLength;
		int writed = fwrite(pWaveHdr->lpData, 1, bufferSize, fpWriteWAV);
	}
	break;
	case WIM_CLOSE:
		break;
	default:
		break;
	}
	return 0;
}

void startRecord(const char* fileName, WAVEFORMATEX pwfx)
{
	if (!hesInNumDrive())
		return;

	if (!canGetInDevCaps())
		return;

	if (NULL == (fpWriteWAV = fopen(fileName, "wb")))
		return;

	//写入文件头
	writeInFormat(pwfx, fpWriteWAV);

	MMRESULT mmResult = waveInOpen(&phwi, WAVE_MAPPER, &pwfx, (DWORD)(MicCallback), NULL, CALLBACK_FUNCTION);
	if (MMSYSERR_NOERROR != mmResult)
	{
		return;
	}

	WAVEHDR pwh1;
	char buffer1[CALLLENGTH];
	pwh1.lpData = buffer1;
	pwh1.dwBufferLength = CALLLENGTH;
	pwh1.dwUser = 1;
	pwh1.dwFlags = 0;
	mmResult = waveInPrepareHeader(phwi, &pwh1, sizeof(WAVEHDR));

	WAVEHDR pwh2;
	char buffer2[CALLLENGTH];
	pwh2.lpData = buffer2;
	pwh2.dwBufferLength = CALLLENGTH;
	pwh2.dwUser = 2;
	pwh2.dwFlags = 0;
	mmResult = waveInPrepareHeader(phwi, &pwh2, sizeof(WAVEHDR));

	if (MMSYSERR_NOERROR == mmResult)
	{
		mmResult = waveInAddBuffer(phwi, &pwh1, sizeof(WAVEHDR));
		mmResult = waveInAddBuffer(phwi, &pwh2, sizeof(WAVEHDR));

		if (MMSYSERR_NOERROR == mmResult)
		{
			mmResult = waveInStart(phwi);
		}
	}
}

void stopRecord()
{
	if (MMSYSERR_NOERROR != waveInStop(phwi))
	{

	}

	if (MMSYSERR_NOERROR != waveInClose(phwi))
	{

	}

	bFlag = FALSE;
	Sleep(100);
	if (fpWriteWAV != nullptr)
		fclose(fpWriteWAV);
}
