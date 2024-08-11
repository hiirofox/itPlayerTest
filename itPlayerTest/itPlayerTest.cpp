#include <Windows.h>
#include <iostream>
#include <string>
#include "it_file.h"
#include "it_play.h"
#include "WaveOut.h"

#define wavlen (SampleRate/16)
WaveOut hwo(48000, wavlen * sizeof(int32_t));
int32_t wavbuf[wavlen];

it_handle hit;
std::vector<it_pattern> patTest;
it_sampler smpTest;
it_instrument insTest;
int16_t bufl[wavlen / 2];
int16_t bufr[wavlen / 2];

int vk_note()
{
	if (GetAsyncKeyState('Z'))return 0;
	if (GetAsyncKeyState('S'))return 1;
	if (GetAsyncKeyState('X'))return 2;
	if (GetAsyncKeyState('D'))return 3;
	if (GetAsyncKeyState('C'))return 4;
	if (GetAsyncKeyState('V'))return 5;
	if (GetAsyncKeyState('G'))return 6;
	if (GetAsyncKeyState('B'))return 7;
	if (GetAsyncKeyState('H'))return 8;
	if (GetAsyncKeyState('N'))return 9;
	if (GetAsyncKeyState('J'))return 10;
	if (GetAsyncKeyState('M'))return 11;
	if (GetAsyncKeyState('Q'))return 12;
	if (GetAsyncKeyState('2'))return 13;
	if (GetAsyncKeyState('W'))return 14;
	if (GetAsyncKeyState('3'))return 15;
	if (GetAsyncKeyState('E'))return 16;
	if (GetAsyncKeyState('R'))return 17;
	if (GetAsyncKeyState('5'))return 18;
	if (GetAsyncKeyState('T'))return 19;
	if (GetAsyncKeyState('6'))return 20;
	if (GetAsyncKeyState('Y'))return 21;
	if (GetAsyncKeyState('7'))return 22;
	if (GetAsyncKeyState('U'))return 23;
	if (GetAsyncKeyState('I'))return 24;
	if (GetAsyncKeyState(VK_ESCAPE))return -1;
	return -2;
}
int main()
{
	itReadFromFile(&hit, "..\\test\\goluigi_-_stream_disintegration.it");

	patTest.resize(hit.itHead.patNum);
	for (int i = 0; i < hit.itHead.patNum; ++i)
	{
		patTest[i].unpackPattern(&hit, i);
	}

	hwo.Start();
	for (;;)
	{
		printf(">");
		std::string cmd;
		std::cin >> cmd;
		if (cmd == "playsmp")
		{
			int smpn;
			std::cin >> smpn;
			printf("playing smp%d...", smpn);
			smpTest.setRelease();
			smpTest.setSample(&hit, smpn);
			for (int j = 0; j < 256; ++j)GetAsyncKeyState(j);//清空一下
			for (;;)
			{
				smpTest.processBlock(bufl, bufr, wavlen / 2);
				for (int i = 0; i < wavlen; i += 2)
				{
					wavbuf[i + 0] = bufl[i / 2] * 32768;
					wavbuf[i + 1] = bufr[i / 2] * 32768;
				}
				int key = vk_note();
				if (key == -1) break;
				else if (key == -2) smpTest.setRelease();
				else
				{
					smpTest.setPitch(key);
					smpTest.setNoteOn();
				}
				hwo.PlayAudio((char*)wavbuf, wavlen * sizeof(int32_t));
			}
			printf("done.\n");
		}
		if (cmd == "playins")
		{
			int insN;
			std::cin >> insN;
			printf("playing ins%d...\n", insN);
			insTest.setRelease();
			insTest.setInstrument(&hit, insN);
			for (int j = 0; j < 256; ++j)GetAsyncKeyState(j);//清空一下
			for (;;)
			{
				insTest.processBlock(bufl, bufr, wavlen / 2);
				for (int i = 0; i < wavlen; i += 2)
				{
					wavbuf[i + 0] = bufl[i / 2] * 32768;
					wavbuf[i + 1] = bufr[i / 2] * 32768;
				}
				int key = vk_note();
				if (key == -1) break;
				else if (key == -2) insTest.setRelease();
				else
				{
					insTest.setPitch(key);
					insTest.setNoteOn();
				}
				hwo.PlayAudio((char*)wavbuf, wavlen * sizeof(int32_t));
			}
			printf("done.\n");
		}
		if (cmd == "pattern")
		{
			int patn, chn;
			std::cin >> patn >> chn;
			patTest[patn].printPatternInfo(chn);
		}
	}
	return 0;
}