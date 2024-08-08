#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>
#include "it_file.h"

#define MaxChannel 64

typedef struct IT_PATTERN_UNIT
{
	int16_t row;
	int8_t note;
	int8_t instrument;
	int8_t volume;
	int8_t cmd;
	int8_t cmdValue;
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
	channel_data channelData[64];
public:
	void unpackPattern(it_handle* hit, int patternN)
	{
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
		printf("\npattern%2d:\n", patternN);
		for (int row = 0, pos = 0; row < hit->itPatternHead->rowCount; ++row)
		{
			uint8_t chnMask = pdat[pos++];
			while (chnMask > 0)
			{
				uint8_t chn = (chnMask - 1) & 63;
				if (chnMask & (1 << 7)) lastMask[chn] = pdat[pos++];
				if (lastMask[chn] & (1 << 0)) lastNote[chn] = pdat[pos++];
				if (lastMask[chn] & (1 << 1)) lastInstrument[chn] = pdat[pos++];
				else if (!(lastMask[chn] & (1 << 5))) lastInstrument[chn] = 0;
				if (lastMask[chn] & (1 << 2)) lastVolume[chn] = pdat[pos++];
				else if (!(lastMask[chn] & (1 << 6))) lastVolume[chn] = 0;
				if (lastMask[chn] & (1 << 3))
				{
					lastCmd[chn] = pdat[pos++];
					lastCmdValue[chn] = pdat[pos++];
				}
				else if (!(lastMask[chn] & (1 << 7)))
				{
					lastCmd[chn] = 0;
					lastCmdValue[chn] = 0;
				}
				chnMask = pdat[pos++];

				it_pattern_row tmp;
				tmp.note = lastNote[chn];
				tmp.instrument = lastInstrument[chn];
				tmp.volume = lastVolume[chn];
				tmp.cmd = lastCmd[chn];
				tmp.cmdValue = lastCmdValue[chn];
				channelData[chn].push_back(tmp);

				printf("chn:%2d | note:%2d,inst:%2d,vol:%2d,cmd:%2d,cmdv:%2d\n",
					chn,
					tmp.note,
					tmp.instrument,
					tmp.volume,
					tmp.cmd,
					tmp.cmdValue);
			}
		}
	}
};
class it_player
{
};