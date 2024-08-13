#include "it_pattern.h"

#define GET_NOTE(mask)    ((mask) & 1 || (mask) & 16)
#define GET_INSTRUMENT(mask) ((mask) & 2 || (mask) & 32)
#define GET_VOLUME(mask)  ((mask) & 4 || (mask) & 64)
#define GET_COMMAND(mask) ((mask) & 8 || (mask) & 128)

void it_pattern::unpackPattern(it_handle* hit, int patternN)
{
	rowCount = hit->itPatternHead[patternN].rowCount;
	channelCount = 0;
	printf("rowCount:%3d channelCount:%3d\n", rowCount, channelCount);
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
	for (int row = 0, pos = 0; row < hit->itPatternHead[patternN].rowCount; ++row)
	{
		uint8_t chnMask = pdat[pos++];
		while (chnMask > 0)
		{
			it_row_data tmp;
			tmp.isNoteChange = 1;
			tmp.isInsChange = 1;
			tmp.isVolChange = 1;
			tmp.isCmdChange = 1;


			uint8_t chn = (chnMask - 1) & 63;
			if (channelCount < chn)channelCount = chn;
			if (chnMask & (1 << 7)) lastMask[chn] = pdat[pos++];

			if (lastMask[chn] & (1 << 0))///////////////////////////////////////////////////
			{
				lastNote[chn] = pdat[pos++];
				tmp.note = lastNote[chn];
			}
			else if (lastMask[chn] & (1 << 4))
			{
				tmp.note = lastNote[chn];
			}
			else
			{
				tmp.isNoteChange = 0;
			}

			if (lastMask[chn] & (1 << 1))///////////////////////////////////////////////////
			{
				lastInstrument[chn] = pdat[pos++];
				tmp.instrument = lastInstrument[chn];
			}
			else if (lastMask[chn] & (1 << 5))
			{
				tmp.instrument = lastInstrument[chn];
			}
			else
			{
				tmp.isInsChange = 0;
			}

			uint8_t vol = 64;
			char volcmd = 'v';
			if (lastMask[chn] & (1 << 2))///////////////////////////////////////////////////
			{
				lastVolume[chn] = pdat[pos++];
				vol = lastVolume[chn];
			}
			else if (lastMask[chn] & (1 << 6))
			{
				vol = lastVolume[chn];
			}
			else
			{
				tmp.isVolChange = 0;
			}
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

			if (lastMask[chn] & (1 << 3))///////////////////////////////////////////////////
			{
				lastCmd[chn] = pdat[pos++];
				lastCmdValue[chn] = pdat[pos++];
				tmp.cmd = lastCmd[chn];
				tmp.cmdValue = lastCmdValue[chn];
			}
			else if (lastMask[chn] & (1 << 7))
			{
				tmp.cmd = lastCmd[chn];
				tmp.cmdValue = lastCmdValue[chn];
			}
			else
			{
				tmp.isCmdChange = 0;
			}
			chnMask = pdat[pos++];

			tmp.row = row;


			patternData[chn][row] = tmp;


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
void it_pattern::printPatternInfo(int channelNum)
{
	printf("pattern:%2d channel:%2d:\n", patternNum, channelNum);
	for (int i = 0; i < rowCount; ++i)
	{
		it_row_data unit = patternData[channelNum - 1][i];
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

it_row_data it_pattern::getRowData(int channelNum, int rowNum)
{
	return patternData[channelNum - 1][rowNum];
}

int it_pattern::getRowCount()
{
	return rowCount;
}

int it_pattern::getChannelCount()
{
	return channelCount;
}
