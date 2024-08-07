#pragma once

#include <stdint.h>
#include <iostream>
#include <vector>
#include "it_file.h"

#define MaxChannel 64
class it_pattern
{
private:
	typedef struct IT_PATTERN_UNIT
	{
		int16_t row;
		int8_t note;
		int8_t instrument;
		int8_t volume;
		int8_t cmd;
		int8_t cmdValue;
	}it_pattern_unit;
	typedef std::vector<it_pattern_unit> row_data;
	std::vector<row_data> pattern_data;
public:
	void unpackPattern(it_handle* hit)
	{
		uint8_t* pdat = (uint8_t*)hit->itPatternData->patternData;
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

		for (int row = 0, pos = 0; row < hit->itPatternHead->rowCount; ++row)
		{
			for (;;)
			{
				uint8_t chnVariable = pdat[pos++];
				if (chnVariable == 0) break;
				uint8_t chn = (chnVariable - 1) & 63;
				if (chn > maxChn)maxChn = chn;

				uint8_t mask;
				if (chnVariable & 128)
				{
					mask = pdat[pos++];

				}
			}
		}
	}
};
class it_player
{
};