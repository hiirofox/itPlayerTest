#include "it_play.h"


it_channel::it_channel()
{
	insPos = 0;
	for (int i = 0; i < MaxInsPerChannel; ++i)
	{
		ins->setUnUse(1);
	}
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
		if ((uint8_t)unit.note == 255)
		{
			ins[insPos].setRelease();
		}
		else if ((uint8_t)unit.note == 254)
		{
			ins[insPos].setNoteCut();
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
		}
	}
	else if (isNewNote)lastVol = 64;

	if (unit.isCmdChange)//cmd
	{
		lastCmd = unit.cmd;
		lastCmdV = unit.cmdValue;
		if (unit.cmd + 64 == 'S')
		{
			if (unit.cmdValue == 0x70)//È«²¿¶ÏÁË
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
		printf("insPos:%3d note: %3d ins:%3d vol:%3d Action:%d\n", insPos, lastNote, lastIns, lastVol, NNA);

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
}

void it_player::open(it_handle* hit)
{
	this->hit = hit;

}

void it_player::processBlock(float* outl, float* outr, int length)
{
}
