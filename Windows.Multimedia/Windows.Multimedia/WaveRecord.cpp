
#include "stdafx.h"
#include <stdio.h>
#include "WaveRecord.h"

#define MAX_LOADSTRING	100
#define BLOCK			1024					//�����Ĵ�С
#define CALLLENGTH		(BLOCK*10)				//���ô����ص��������ֽڳ���

//¼����ʽ����
//WAVEFORMATEX pwfx = {
//	WAVE_FORMAT_PCM,// wFormatTag����ʽ��־
//	1,			// nChannels��ͨ�����������������õ�ͨ����������ͨ����˫ͨ��
//	16000,		// nSamplesPerSec�������ʣ�HZ����ÿ���Ӳ�ȡ�����Ĵ���
//	32000,		// nAvgBytesPerSec��ÿ��ת�����ݵ��ֽ�������nAvgBytesPerSec = nSamplesPerSec *  nChannels * nBlockAlign���˴��Ĵ�С��waveInOopen�ص�������д�����ݵĴ�СӦ��һֱ��������������
//	2,			// nBlockAlign��ÿ���������ֽ�����nBlockAlign = (nChannels �� wBitsPerSample) / 8
//	16,			// wBitsPerSample��ÿ��������λ�����������8����16
//	0			// cbSize�������ڸýṹ�����ĸ�ʽ��Ϣ�Ĵ�С
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
	MMRESULT mmResult = waveInGetDevCaps(0, &waveIncaps, sizeof(WAVEINCAPS));//2 �豸����
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

	//д���ļ�ͷ
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
