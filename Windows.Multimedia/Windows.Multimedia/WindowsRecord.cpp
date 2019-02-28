#include "stdafx.h"
#include "WindowsRecord.h"

NS_WINDOWS_MUTIMEDIA_BEGIN

void WindowsRecord::BeginRecord(String^ path)
{
	m_path = path;
	Task::Factory->StartNew(gcnew Action(&WindowsRecord::StartRecord));
}

void WindowsRecord::EndRecord()
{
	stopRecord();
}

void WindowsRecord::StartRecord()
{
	auto filePathPtr = Marshal::StringToHGlobalAnsi(m_path);
	auto filepath = (char*)(void*)filePathPtr;
	Marshal::FreeHGlobal(filePathPtr);
	WAVEFORMATEX pwfx = { WAVE_FORMAT_PCM, 1, 16000, 32000, 2, 16, 0 };
	startRecord(filepath, pwfx);
}

NS_WINDOWS_MUTIMEDIA_END
