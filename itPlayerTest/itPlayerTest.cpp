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
it_pattern patTest;
it_sampler smpTest;
it_instrument insTest;
it_channel chnTest;
it_channel chnPatTest[64];
it_player playTest;

float bufl[wavlen / 2];
float bufr[wavlen / 2];

std::string noteToString(int note)
{
	std::string str = "";
	if (note % 12 == 0) str = "C-";
	if (note % 12 == 1) str = "C#";
	if (note % 12 == 2) str = "D-";
	if (note % 12 == 3) str = "D#";
	if (note % 12 == 4) str = "E-";
	if (note % 12 == 5) str = "F-";
	if (note % 12 == 6) str = "F#";
	if (note % 12 == 7) str = "G-";
	if (note % 12 == 8) str = "G#";
	if (note % 12 == 9) str = "A-";
	if (note % 12 == 10) str = "A#";
	if (note % 12 == 11) str = "B-";
	char tmp[8];
	sprintf(tmp, "%s%d", str.c_str(), note / 12);
	str = tmp;
	return str;
}
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
	itReadFromFile(&hit, "..\\test\\laamaa_-_wb22-wk21.it");


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
					smpTest.setPitch(key + 12 * 4);
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
			insTest.setInstrument(&hit, insN);
			insTest.setRelease();
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
					insTest.setPitch(key + 12 * 4);
					insTest.setNoteOn();
				}
				hwo.PlayAudio((char*)wavbuf, wavlen * sizeof(int32_t));
			}
			printf("done.\n");
		}
		if (cmd == "channel")
		{
			int patn, chn;
			std::cin >> patn >> chn;
			patTest.unpackPattern(&hit, patn);
			patTest.printPatternInfo(chn);
		}
		if (cmd == "playchn")
		{
			int patn, chn;
			std::cin >> patn >> chn;
			patTest.unpackPattern(&hit, patn);
			patTest.printPatternInfo(chn);
			chnTest.setItHandle(&hit);
			for (int n = 0; n < patTest.getRowCount(); ++n)
			{
				chnTest.updataRow(patTest.getRowData(chn, n));
				for (int j = 0; j < hit.itHead.initSpeed; ++j)//initSpeed是tickPerRow
				{
					chnTest.processBlock(bufl, bufr, wavlen / 2);
					for (int i = 0; i < wavlen; i += 2)
					{
						wavbuf[i + 0] = bufl[i / 2] * 8192;
						wavbuf[i + 1] = bufr[i / 2] * 8192;
					}
					hwo.PlayAudio((char*)wavbuf, wavlen * sizeof(int32_t));
				}
				int key = vk_note();
				if (key == -1) break;
			}
		}
		if (cmd == "playpat")
		{
			int patn;
			std::cin >> patn;
			patTest.unpackPattern(&hit, patn);
			for (int m = 0; m < 64; ++m)
			{
				chnPatTest[m].setItHandle(&hit);
			}
			int channelCount = patTest.getChannelCount();
			int rowCount = patTest.getRowCount();
			printf("channelCount:%d\n", channelCount);
			for (int n = 0; n < rowCount; ++n)
			{
				for (int m = 0; m < channelCount; ++m)
				{
					chnPatTest[m].updataRow(patTest.getRowData(m + 1, n));
				}
				for (int j = 0; j < hit.itHead.initSpeed; ++j)//initSpeed是tickPerRow
				{
					for (int i = 0; i < wavlen; i += 2)
					{
						wavbuf[i + 0] = 0;
						wavbuf[i + 1] = 0;
					}
					for (int m = 0; m < channelCount; ++m)
					{
						chnPatTest[m].processBlock(bufl, bufr, wavlen / 2);
						for (int i = 0; i < wavlen; i += 2)
						{
							wavbuf[i + 0] += bufl[i / 2] * 6400;
							wavbuf[i + 1] += bufr[i / 2] * 6400;
						}
					}
					hwo.PlayAudio((char*)wavbuf, wavlen * sizeof(int32_t));
				}
				int key = vk_note();
				if (key == -1) break;
			}
		}
	}
	return 0;
}