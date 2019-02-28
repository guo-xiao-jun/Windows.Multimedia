#pragma once
#include "stdafx.h"
#include "WaveRecord.h"

using namespace System;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Runtime::InteropServices;

NS_WINDOWS_MUTIMEDIA_BEGIN

public ref class WindowsRecord
{
public:
	static void WindowsRecord::BeginRecord(String^ path);
	static void WindowsRecord::EndRecord();
private:
	static void StartRecord();

private:
	static String^ m_path = nullptr;
};

NS_WINDOWS_MUTIMEDIA_END