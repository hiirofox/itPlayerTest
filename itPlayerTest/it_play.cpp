#include "it_play.h"

///!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
float _BufferL[MaxBufferLength];
float _BufferR[MaxBufferLength];

it_channel::it_channel()
{
	lastVol = 64;
	lastPan = 32;
	insPos = 0;
	for (int i = 0; i < MaxInsPerChannel; ++i)
	{
		ins->setUnUse(1);
	}
	bufl = _BufferL;
	bufr = _BufferR;
}

void it_channel::setItHandle(it_handle* hit)
{
	this->hit = hit;
}

void it_channel::updataRow(it_row_data unit)
{
	bool isNewNote = 0;
	if (unit.isNoteChange)//note
	{
		if ((uint8_t)unit.note == 255)//release
		{
			for (int i = 0; i < MaxInsPerChannel; ++i)
			{
				ins[i].setRelease();
			}
		}
		else if ((uint8_t)unit.note == 254)//notecut
		{
			ins[insPos].setNoteCut();
			for (int i = 0; i < MaxInsPerChannel; ++i)
			{
				ins[i].setRelease();
			}
		}
		else if (unit.note >= 0 && unit.note <= 199)
		{
			isNewNote = 1;
			lastNote = unit.note;
		}
	}

	if (unit.isInsChange)//instrument
	{
		isNewNote = 1;
		lastIns = unit.instrument;
	}

	if (unit.isVolChange)//volume
	{
		if (unit.volCmd == 'v')
		{
			lastVol = unit.vol;//????????
			if (!isNewNote)ins[insPos].setVolume(lastVol);
		}
		if (unit.volCmd == 'p')
		{
			lastPan = unit.vol;
			if (!isNewNote)ins[insPos].setPan(lastPan);
		}
	}
	else if (isNewNote)lastVol = 64;

	if (unit.isCmdChange)//cmd
	{
		lastCmd = unit.cmd;
		lastCmdV = unit.cmdValue;
		if (unit.cmd + 64 == 'S')
		{
			if (unit.cmdValue == 0x70)//全部断了
			{
				for (int i = 0; i < MaxInsPerChannel; ++i)
				{
					ins[i].setNoteCut();
				}
			}
		}
	}

	if (isNewNote)
	{
		int NNA = ins[insPos].getNewNoteAction();
		//printf("insPos:%3d note: %3d ins:%3d vol:%3d Action:%d\n", insPos, lastNote, lastIns, lastVol, NNA);

		bool isChangeIns = 0;
		if (NNA == 0)//note cut
		{
			ins[insPos].setNoteCut();
		}
		else if (NNA == 1)//continue
		{
			isChangeIns = 1;
		}
		else if (NNA == 2)//release
		{
			ins[insPos].setRelease();
			isChangeIns = 1;
		}
		else if (NNA == 3)//note fade
		{
			ins[insPos].setRelease();
			isChangeIns = 1;
		}
		if (isChangeIns)
		{
			insPos++;
			if (insPos >= MaxInsPerChannel)insPos = 0;
		}
		ins[insPos].setPitch(lastNote);
		ins[insPos].setInstrument(hit, lastIns);
		ins[insPos].setVolume(lastVol);
		ins[insPos].setPan(lastPan);
		ins[insPos].resetNote();
	}
}

void it_channel::processBlock(float* outl, float* outr, int length)
{
	//printf("tick++\n");
	for (int i = 0; i < length; ++i)
	{
		outl[i] = 0;
		outr[i] = 0;
	}
	for (int j = 0; j < MaxInsPerChannel; ++j)
	{
		ins[j].processBlock(bufl, bufr, length);
		for (int i = 0; i < length; ++i)
		{
			outl[i] += bufl[i];
			outr[i] += bufr[i];
		}
	}
}

//
//////////////////////////////////////////////////////////////////////
//


it_player::it_player()
{
	ordPos = 0;
	rowPos = 0;
	readPos = 0;
	writePos = 0;
}

void it_player::open(it_handle* hit)
{
	this->hit = hit;
	for (int m = 0; m < MaxChannelNum; ++m)
	{
		channels[m].setItHandle(hit);
	}
	pattern.unpackPattern(hit, hit->itHeadData.orders[0]);
	ordPos = 0;
	tickCount = 0;
}


void it_player::processBlock(float* outl, float* outr, int length)
{
	while (writePos - readPos < length)//如果缓冲区里的空间不够读出来的话
	{//里面就用来把1tick的东西填到buffer里
		tempo = hit->itHead.initTempo;//我先不考虑变速先
		int tickSampleNum = TickToSampleNum(tempo);

		if (rowPos >= pattern.getRowCount())//一个pattern结束了！
		{
			rowPos = 0;
			ordPos++;
			pattern.unpackPattern(hit, hit->itHeadData.orders[ordPos]);//读下一个pattern
			printf("play pattern:%d orderPos:%d\n", hit->itHeadData.orders[ordPos], ordPos);
		}
		if (tickCount % hit->itHead.initSpeed == 0)//row++!(如果不变速的话)
		{
			int channelNum = pattern.getChannelCount();
			for (int m = 0; m < channelNum; ++m)//更新row!!
			{
				channels[m].updataRow(pattern.getRowData(m + 1, rowPos));
			}
			tickCount = 0;
			rowPos++;
		}
		tickCount++;

		for (int i = 0; i < tickSampleNum; ++i)///////////////开始processBlock1Tick
		{
			buffer2L[i] = 0;
			buffer2R[i] = 0;
		}
		int channelNum = pattern.getChannelCount();
		for (int m = 0; m < channelNum; ++m)
		{
			channels[m].processBlock(bufl, bufr, tickSampleNum);
			for (int i = 0; i < tickSampleNum; ++i)
			{
				buffer2L[i] += bufl[i];
				buffer2R[i] += bufr[i];
			}
		}

		//填充缓冲区:
		int pos = writePos % ItPlayerBufferSize;
		for (int i = 0; i < tickSampleNum; ++i)
		{
			buffer3L[pos] = buffer2L[i];
			buffer3R[pos] = buffer2R[i];
			pos++;
			if (pos >= ItPlayerBufferSize)pos = 0;
			writePos++;
		}
	}
	int pos = readPos % ItPlayerBufferSize;
	for (int i = 0; i < length; ++i)//从缓冲区取数据出来
	{
		outl[i] = buffer3L[pos];
		outr[i] = buffer3R[pos];
		pos++;
		if (pos >= ItPlayerBufferSize)pos = 0;
		readPos++;
	}
}
