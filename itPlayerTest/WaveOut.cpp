#include "WaveOut.h"


WaveOut::WaveOut(int sampleRate, int bufSize) :

	thread_is_running(false), m_hThread(0), m_ThreadID(0), m_bDevOpen(false), m_hWaveOut(0), m_BufferQueue(0), isplaying1(false), isplaying2(false) {
	m_Waveformat.wFormatTag = WAVE_FORMAT_PCM;
	m_Waveformat.nChannels = 2;
	m_Waveformat.wBitsPerSample = 32;
	m_Waveformat.nSamplesPerSec = sampleRate;
	m_Waveformat.cbSize = 0;
	m_Waveformat.nBlockAlign = (m_Waveformat.wBitsPerSample * m_Waveformat.nChannels) >> 3;
	m_Waveformat.nAvgBytesPerSec = m_Waveformat.nBlockAlign * m_Waveformat.nSamplesPerSec;
	buf_size = bufSize ;
	buf1 = new char[buf_size];
	buf2 = new char[buf_size];
	ZeroMemory(&wavehdr1, sizeof(WAVEHDR));
	ZeroMemory(&wavehdr2, sizeof(WAVEHDR));
	wavehdr1.lpData = buf1;
	wavehdr1.dwBufferLength = buf_size;
	wavehdr2.lpData = buf2;
	wavehdr2.dwBufferLength = buf_size;
	InitializeCriticalSection(&m_Lock);
}
WaveOut::~WaveOut() {
	WaitForPlayingEnd();
	StopThread();
	Close();
	delete[] buf1;
	delete[] buf2;
	DeleteCriticalSection(&m_Lock);
}
void WaveOut::Start() {
	StartThread();
	Open();
}
void WaveOut::PlayAudio(char* in_buf, unsigned int in_size) {
	while (1) {
		if (isplaying1 && isplaying2) {
			Sleep(10);
			continue;
		}
		else {
			break;
		}
	}
	char* now_buf = NULL;
	WAVEHDR* now_wavehdr = NULL;
	bool* now_playing = NULL;
	if (isplaying1 == false) {
		now_buf = buf1;
		now_wavehdr = &wavehdr1;
		now_playing = &isplaying1;
	}
	if (isplaying2 == false) {
		now_buf = buf2;
		now_wavehdr = &wavehdr2;
		now_playing = &isplaying2;
	}
	if (in_size <= buf_size) {
		now_wavehdr->dwBufferLength = in_size;
	}
	memcpy(now_buf, in_buf, in_size);
	waveOutWrite(m_hWaveOut, now_wavehdr, sizeof(WAVEHDR));
	EnterCriticalSection(&m_Lock);
	*now_playing = true;
	LeaveCriticalSection(&m_Lock);
}

DWORD __stdcall WaveOut::ThreadProc(LPVOID lpParameter) {
	WaveOut* pWaveOut = (WaveOut*)lpParameter;
	pWaveOut->SetThreadSymbol(true);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0)) {
		switch (msg.message) {
		case WOM_OPEN:
			break;
		case WOM_CLOSE:
			break;
		case WOM_DONE:
			//标记完成符号
			WAVEHDR* pWaveHdr = (WAVEHDR*)msg.lParam;
			pWaveOut->SetFinishSymbol(pWaveHdr);
			break;
		}
	}
	pWaveOut->SetThreadSymbol(false);
	return msg.wParam;
}
void WaveOut::StartThread() {
	m_hThread = CreateThread(0, 0, ThreadProc, this, 0, &m_ThreadID);
}
void WaveOut::StopThread() {
	if (!thread_is_running) {
		return;
	}
	if (m_hThread) {
		PostThreadMessage(m_ThreadID, WM_QUIT, 0, 0);
		while (1) {
			if (thread_is_running) {
				Sleep(1);
			}
			else {
				break;
			}
		}
		TerminateThread(m_hThread, 0);
		m_hThread = 0;
	}
}

void WaveOut::Open() {
	//lphWaveOut: PHWaveOut;   {用于返回设备句柄的指针; 如果 dwFlags=WAVE_FORMAT_QUERY, 这里应是 nil}
	//uDeviceID: UINT;         {设备ID; 可以指定为: WAVE_MAPPER, 这样函数会根据给定的波形格式选择合适的设备}
	//lpFormat: PWaveFormatEx; {TWaveFormat 结构的指针; TWaveFormat 包含要申请的波形格式}
	//dwCallback: DWORD        {回调函数地址或窗口句柄; 若不使用回调机制, 设为 nil}
	//dwInstance: DWORD        {给回调函数的实例数据; 不用于窗口}
	//dwFlags: DWORD           {打开选项}// long120823
	MMRESULT mRet;
	mRet = waveOutOpen(0, WAVE_MAPPER, &m_Waveformat, 0, 0, WAVE_FORMAT_QUERY);
	mRet = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &m_Waveformat, m_ThreadID, 0, CALLBACK_THREAD);
	waveOutPrepareHeader(m_hWaveOut, &wavehdr1, sizeof(WAVEHDR));
	waveOutPrepareHeader(m_hWaveOut, &wavehdr2, sizeof(WAVEHDR));
	m_bDevOpen = TRUE;
}

void WaveOut::Close() {
	if (!m_bDevOpen) {
		return;
	}
	if (!m_hWaveOut) {
		return;
	}
	MMRESULT mRet;
	mRet = waveOutUnprepareHeader(m_hWaveOut, &wavehdr1, sizeof(WAVEHDR));
	mRet = waveOutUnprepareHeader(m_hWaveOut, &wavehdr2, sizeof(WAVEHDR));
	mRet = waveOutClose(m_hWaveOut);
	m_hWaveOut = 0;
	m_bDevOpen = FALSE;
}
inline void WaveOut::WaitForPlayingEnd() {
	while (1) {
		if (isplaying1 || isplaying2) {
			Sleep(1);
		}
		else {
			break;
		}
	}
}
void WaveOut::Stop() {
	MMRESULT mRet;
	mRet = waveOutReset(m_hWaveOut);
	WaitForPlayingEnd();
	StopThread();
	Close();
}
inline void WaveOut::SetThreadSymbol(bool running) {
	EnterCriticalSection(&m_Lock);
	thread_is_running = running;
	LeaveCriticalSection(&m_Lock);
}
inline void WaveOut::SetFinishSymbol(PWAVEHDR pWaveHdr) {
	EnterCriticalSection(&m_Lock);
	if (pWaveHdr == &wavehdr1) {
		isplaying1 = false;
	}
	else {
		isplaying2 = false;
	}
	LeaveCriticalSection(&m_Lock);
}