#include "it_instrument.h"

it_instrument::it_instrument()
{
	isSampleOK = 0;
}

void it_instrument::resetNote()
{
	printf("reset ins\n");
	isNoteOn = 1;
	tickPos = 0;
	volNodeN = 0, panNodeN = 0, pitchNodeN = 0;//包络nodeN全弄0了
	if (sampler.getSample(hit, kbTable[initNote]) == -1) isSampleOK = 0;
	else isSampleOK = 1;
	sampler.resetNote();
}

void it_instrument::setNoteOn()
{
	if (isNoteOn == 0)
	{
		resetNote();
	}
}

void it_instrument::setRelease()
{
	//printf("release\n");
	isNoteOn = 0;
}

void it_instrument::setPitch(float note)
{
	initNote = note;
	this->note = note;
}

void it_instrument::getInstrument(it_handle* hit, int instrumentNum)
{
	this->hit = hit;
	ins = &hit->itInstruments[instrumentNum - 1];
	memset(kbTable, -1, sizeof(kbTable));
	for (int i = 0; i < 120; ++i)
	{
		kbTable[ins->kbTable[i].note] = ins->kbTable[i].sample;
	}
	printf("useEnve:%s useLoop:%s loopSusLoop:%s\n",
		ins->envelope.isUseEnve ? "true " : "false",
		ins->envelope.isUseLoop ? "true " : "false",
		ins->envelope.isUseSustainLoop ? "true " : "false");
	printf("loopBeg:%d loopEnd:%d\n", ins->envelope.loopBegining, ins->envelope.loopEnd);
	printf("susLoopBeg:%d susLoopEnd:%d\n", ins->envelope.sustainLoopBegining, ins->envelope.sustainLoopEnd);

}

void it_instrument::processBlock(int16_t* outl, int16_t* outr, int length)
{//我先不管那么多，把每个block当成一个tick，后边track就要考虑很多了

	if (tickPos >= ins->envelope.volNode[volNodeN].tickPos && volNodeN < ins->envelope.nodeCount)
	{
		printf("volNode:%d nodeCount:%d\n", volNodeN, ins->envelope.nodeCount);
		int nextVolNodeN;
		if (ins->envelope.isUseSustainLoop && isNoteOn)
		{
			if (volNodeN == ins->envelope.sustainLoopEnd)
			{
				nextVolNodeN = ins->envelope.sustainLoopBegining;
				volNodeN = ins->envelope.sustainLoopBegining;
				tickPos = ins->envelope.volNode[volNodeN].tickPos;
			}
			else nextVolNodeN = volNodeN + 1;
		}
		else if (ins->envelope.isUseLoop)
		{
			if (volNodeN == ins->envelope.loopEnd)
			{
				nextVolNodeN = ins->envelope.loopBegining;
				volNodeN = ins->envelope.loopBegining;
				tickPos = ins->envelope.volNode[volNodeN].tickPos;
			}
			else nextVolNodeN = volNodeN + 1;
		}
		else nextVolNodeN = volNodeN + 1;

		vol = ins->envelope.volNode[volNodeN].yPos;

		if (volNodeN == nextVolNodeN)volK = 0;
		else {
			volK = (ins->envelope.volNode[nextVolNodeN].yPos - ins->envelope.volNode[volNodeN].yPos) /
				(ins->envelope.volNode[nextVolNodeN].tickPos - ins->envelope.volNode[volNodeN].tickPos);
		}
		volNodeN = nextVolNodeN;
	}

	if (isSampleOK)
	{
		sampler.setPitch(note);//更新一下pitch
		sampler.processBlock(outl, outr, length);//拿一下采样
	}

	float volume = vol / 64.0;
	float volk2 = volK / length;
	for (int i = 0; i < length; ++i)//处理音量包络
	{
		outl[i] *= volume;
		outr[i] *= volume;
		vol += volk2;
	}

	if (volNodeN < ins->envelope.nodeCount)tickPos++;
}
