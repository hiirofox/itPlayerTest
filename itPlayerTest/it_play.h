#pragma once

#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "WaveOutDef.h"
#define MaxChannel 64

std::string noteToString(int note);

typedef struct IT_PATTERN_UNIT
{
	int16_t row;
	int8_t note;
	int8_t instrument;
	int8_t vol;
	int8_t volCmd;//逆天it之vol还包含声像控制
	int8_t cmd;
	int8_t cmdValue;
	bool isNoteChange;
	bool isInsChange;
	bool isVolChange;
	bool isCmdChange;
}it_pattern_row;
typedef std::vector<it_pattern_row> channel_data;

class tracker_pattern
{
private:
public:
};

class it_pattern :public tracker_pattern
{
private:
	int patternNum;
	channel_data patternData[64];
public:
	void unpackPattern(it_handle* hit, int patternN)
	{
		patternNum = patternN;
		uint8_t* pdat = (uint8_t*)hit->itPatternData[patternN].patternData;
		int maxChn = -1999999;

		uint8_t		lastMask[MaxChannel];
		int8_t		lastNote[MaxChannel];
		int8_t		lastInstrument[MaxChannel];
		int8_t		lastVolume[MaxChannel];
		int8_t		lastCmd[MaxChannel];
		int8_t		lastCmdValue[MaxChannel];
		memset(lastMask, 0, sizeof(lastMask));
		memset(lastNote, 0, sizeof(lastNote));
		memset(lastInstrument, 0, sizeof(lastInstrument));
		memset(lastVolume, 0, sizeof(lastVolume));
		memset(lastCmd, 0, sizeof(lastCmd));
		memset(lastCmdValue, 0, sizeof(lastCmdValue));
		//printf("\npattern%2d:\n", patternN);
		for (int row = 0, pos = 0; row < hit->itPatternHead->rowCount; ++row)
		{
			uint8_t chnMask = pdat[pos++];
			while (chnMask > 0)
			{
				bool isNoteChange = 0;
				bool isInsChange = 0;
				bool isVolChange = 0;
				bool isCmdChange = 0;

				uint8_t chn = (chnMask - 1) & 63;
				if (chnMask & (1 << 7)) lastMask[chn] = pdat[pos++];

				if (lastMask[chn] & (1 << 0)) lastNote[chn] = pdat[pos++], isNoteChange = 1;
				else if (lastMask[chn] & (1 << 4)) isNoteChange = 0;

				if (lastMask[chn] & (1 << 1)) lastInstrument[chn] = pdat[pos++], isInsChange = 1;
				else if (lastMask[chn] & (1 << 5)) isInsChange = 0;

				if (lastMask[chn] & (1 << 2)) lastVolume[chn] = pdat[pos++], isVolChange = 1;
				else if (lastMask[chn] & (1 << 6)) isVolChange = 0;

				if (lastMask[chn] & (1 << 3))
				{
					lastCmd[chn] = pdat[pos++];
					lastCmdValue[chn] = pdat[pos++];
					isCmdChange = 1;
				}
				else if (lastMask[chn] & (1 << 7))
				{
					isCmdChange = 0;
				}
				chnMask = pdat[pos++];

				it_pattern_row tmp;
				tmp.row = row;
				tmp.note = lastNote[chn];
				tmp.isNoteChange = isNoteChange;
				tmp.instrument = lastInstrument[chn];
				tmp.isInsChange = isInsChange;

				uint8_t vol = lastVolume[chn];
				char volcmd = 'v';
				if (vol > 64)//逆天it之vol还包各种指令
				{
					if (vol < 75)						volcmd = 'a', vol -= 65;
					else if (vol < 85)					volcmd = 'b', vol -= 75;
					else if (vol < 95)					volcmd = 'c', vol -= 85;
					else if (vol < 105)					volcmd = 'd', vol -= 95;
					else if (vol < 115)					volcmd = 'e', vol -= 105;
					else if (vol < 125)					volcmd = 'f', vol -= 115;
					else if (vol < 193)					volcmd = 'p', vol -= 128;
					else if (vol < 203)					volcmd = 'g', vol -= 193;
				}
				tmp.vol = vol;
				tmp.volCmd = volcmd;

				tmp.isVolChange = isVolChange;
				tmp.cmd = lastCmd[chn];
				tmp.cmdValue = lastCmdValue[chn];
				tmp.isCmdChange = isCmdChange;
				patternData[chn].push_back(tmp);


				/*printf("chn:%2d,row:%2d | note:%2d,inst:%2d,vol:%2d,cmd:%2d,cmdv:%2d\n",
					chn,
					row,
					tmp.note,
					tmp.instrument,
					tmp.volume,
					tmp.cmd,
					tmp.cmdValue);*/

			}
		}
	}
	void printPatternInfo(int channelNum)
	{
		printf("pattern:%2d channel:%2d:\n", patternNum, channelNum);
		for (auto unit : patternData[channelNum - 1])
		{
			printf("row:%3d mask:%d,%d,%d,%d | ", unit.row, unit.isNoteChange, unit.isInsChange, unit.isVolChange, unit.isCmdChange);

			if (unit.isNoteChange)//note
			{
				if ((uint8_t)unit.note == 255)printf("=== ");
				else if ((uint8_t)unit.note == 254)printf("^^  ");
				else if (unit.note >= 0 && unit.note <= 199) printf("%s ", noteToString(unit.note).c_str());
			}
			else printf("... ");

			if (unit.isInsChange)//instrument
			{
				printf("%02d ", unit.instrument);
			}
			else printf(".. ");

			if (unit.isVolChange)//volume
			{
				char volcmd = 'v';
				uint8_t vol = unit.vol;
				printf("%c%02d ", volcmd, vol);
			}
			else printf("... ");

			if (unit.isCmdChange)//cmd
			{
				printf("%c%02X ", unit.cmd + 64, (uint8_t)unit.cmdValue);
			}
			else printf("... ");

			printf("\n");
		}
	}
};

class it_sampler
{
private:
	ItSample::it_sample_head* smpHead;
	ItSample::it_sample_data* smpData;
	float pos, speed;
	bool isNoteOn = 0;
	bool isIntoLoop = 0;	//在各种loop里了
	int loopMode;			//loop模式 0:off 1:on 2:bidi
	int loopState;
public:
	it_sampler()
	{
	}
	void getSample(it_handle* hit, int sampleNum)
	{
		smpHead = &hit->itSampleHead[sampleNum - 1];
		smpData = &hit->itSampleData[sampleNum - 1];
	}
	void resetNote()
	{
		pos = 0;
		isNoteOn = 1;
	}
	void setNoteOn()
	{
		if (isNoteOn == 0)
		{
			pos = 0;
			isNoteOn = 1;
		}
	}
	void setRelease()
	{
		isNoteOn = 0;
	}
	void setPitch(float note)
	{
		speed = powf(2.0, note / 12.0);
		speed *= (float)smpHead->C5Speed / SampleRate;
		printf("speed:%.5f\n", speed);
	}
	void processBlock16Bit(int16_t* outl, int16_t* outr, int length)//test
	{
		int sampleLen = smpHead->sampleLen;
		int16_t* datl = (int16_t*)smpData->sampleData;
		int16_t* datr;
		if (smpHead->isStereo) datr = (int16_t*)smpData->sampleData + sampleLen;
		else datr = (int16_t*)smpData->sampleData;

		int loopLow, loopHigh, loopMode;
		if (isNoteOn && smpHead->isUseSustainLoop)//因为，反正总是只有一个loop在工作。
		{
			loopLow = smpHead->sustainLoopBegining;
			loopHigh = smpHead->sustainLoopEnd;
			loopMode = smpHead->sustainLoopMode;
			//if (!smpHead->isUseSustainLoop)loopMode = -1;
		}
		else
		{
			loopLow = smpHead->loopBegining;
			loopHigh = smpHead->loopEnd;
			loopMode = smpHead->loopMode;
			if (!smpHead->isUseLoop)loopMode = -1;
		}
		int i = 0;
		for (; i < length && pos < sampleLen; ++i)
		{
			outl[i] = datl[(int)pos];
			outr[i] = datr[(int)pos];

			if (isIntoLoop)//如果是在循环里面，考虑的事情就很多了
			{
				if (loopMode == 0)//on是到头就重来
				{
					pos += speed;
					if (pos >= loopHigh) pos = loopLow;
				}
				else if (loopMode == 1)//bidi是来回
				{
					if (loopState == 1)			pos -= speed;//回头
					else						pos += speed;

					if (pos >= loopHigh)		pos -= speed, loopState = 1;
					else if (pos <= loopLow)	pos += speed, loopState = 2;
				}
				else//其他就当作没有
				{
					pos += speed;
				}
			}
			else//如果不在循环里面就直接前进就好了
			{
				if (pos >= loopLow - speed && pos <= loopHigh + speed)//两边那个-speed和+speed为了防止跑飞
				{
					isIntoLoop = 1;
				}
				pos += speed;
			}
		}
		for (; i < length; ++i)//零填充，如果pos比i提前爆了的话
		{
			outl[i] = 0;
			outr[i] = 0;
		}
		//printf("pos:%.5f\n", pos);
	}
};
class tracker_instrument
{
private:
public:
};
class it_instrument :public tracker_instrument
{
private:
public:
};

class it_player
{
private:
public:
};