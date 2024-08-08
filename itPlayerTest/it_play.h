#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>
#include "it_file.h"

#define MaxChannel 64

std::string noteToString(int note);

typedef struct IT_PATTERN_UNIT
{
	int16_t row;
	int8_t note;
	int8_t instrument;
	int8_t volume;
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

				if (lastMask[chn] & (1 << 0)) lastNote[chn] = pdat[pos++];
				if (lastMask[chn] & (1 << 4)) isNoteChange = 1;

				if (lastMask[chn] & (1 << 1)) lastInstrument[chn] = pdat[pos++];
				if (lastMask[chn] & (1 << 5)) isInsChange = 1;

				if (lastMask[chn] & (1 << 2)) lastVolume[chn] = pdat[pos++];
				if (lastMask[chn] & (1 << 6)) isVolChange = 1;

				if (lastMask[chn] & (1 << 3))
				{
					lastCmd[chn] = pdat[pos++];
					lastCmdValue[chn] = pdat[pos++];
				}
				if (lastMask[chn] & (1 << 7))
				{
					isCmdChange = 1;
				}
				chnMask = pdat[pos++];

				it_pattern_row tmp;
				tmp.row = row;
				tmp.note = lastNote[chn];
				tmp.isNoteChange = isNoteChange;
				tmp.instrument = lastInstrument[chn];
				tmp.isInsChange = isInsChange;
				tmp.volume = lastVolume[chn];
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

			if (!unit.isNoteChange)//note
			{
				if (unit.note == 255)printf("=== ");
				else if (unit.note == 254)printf("^^  ");
				else if (unit.note >= 0 && unit.note <= 199) printf("%s ", noteToString(unit.note).c_str());
				else printf("??? ");
			}
			else printf("... ");

			if (!unit.isInsChange)//instrument
			{
				printf("%02d ", unit.instrument);
			}
			else printf(".. ");

			if (!unit.isVolChange)//volume
			{
				printf("v%02d ", unit.volume);
			}
			else printf("... ");

			if (!unit.isCmdChange)//cmd
			{
				printf("%c%02X ", unit.cmd + 64, (uint8_t)unit.cmdValue);
			}
			else printf("... ");

			printf("\n");
		}
	}
};
class it_player
{
};