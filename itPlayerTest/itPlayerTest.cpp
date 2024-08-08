#include <iostream>
#include <string>
#include "it_file.h"
#include "it_play.h"
#include "WaveOut.h"

#define SampleRate 48000
#define wavlen (SampleRate/6)
WaveOut hwo(48000, wavlen * sizeof(int32_t));
int32_t wavbuf[wavlen];

it_handle hit;
it_pattern patTest;
int main()
{
	itReadFromFile(&hit, "D:\\JuceProject\\itPlayerTest\\Test\\laamaa_-_wb22-wk21.it");
	
	for (int i = 0; i < hit.itHead.patNum; ++i)
		patTest.unpackPattern(&hit, i);

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
			int smpLen = hit.itSampleHead[smpn - 1].sampleLen;
			int isStereo = hit.itSampleHead[smpn - 1].isStereo;
			int is16Bit = hit.itSampleHead[smpn - 1].is16Bit;
			if (isStereo) smpLen /= 2;

			void* smpbuf = hit.itSampleData[smpn - 1].sampleData;
			for (int pos = 0; pos < smpLen;)
			{
				for (int i = 0; i < wavlen; i += 2)
				{
					int32_t datl, datr;
					if (is16Bit)
					{
						datl = ((int16_t*)smpbuf)[pos];
						if (isStereo)	datr = ((int16_t*)smpbuf)[pos + smpLen];
						else			datr = datl;
						datl *= 32768;
						datr *= 32768;
					}
					else
					{
						datl = ((int8_t*)smpbuf)[pos];
						if (isStereo)	datr = ((int16_t*)smpbuf)[pos + smpLen];
						else			datr = datl;
						datl *= 32768 * 256;
						datr *= 32768 * 256;
					}
					wavbuf[i + 0] = datl;
					wavbuf[i + 1] = datr;
					pos++;
					if (pos >= smpLen)
					{
						for (; i < wavlen; ++i)wavbuf[i] = 0;
						break;
					}
				}
				hwo.PlayAudio((char*)wavbuf, wavlen * sizeof(int32_t));
			}
			printf("done.\n");
		}
	}
	return 0;
}