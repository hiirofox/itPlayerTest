#include "it_instrument.h"

it_instrument::it_instrument()
{
	isSampleOK = 0;
}

void it_instrument::resetNote()
{
	//printf("reset ins\n");
	isNoteOn = 1;
	if (sampler.setSample(hit, kbTable[initNote]) == -1) isSampleOK = 0;
	else isSampleOK = 1;
	sampler.resetNote();
	volEnve.resetNote();
	panEnve.resetNote();
	pitchEnve.resetNote();
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
	volEnve.setRelease();
	panEnve.setRelease();
	pitchEnve.setRelease();
}

void it_instrument::setPitch(float note)
{
	initNote = note;
	this->note = note;
}

void it_instrument::setInstrument(it_handle* hit, int instrumentNum)
{
	this->hit = hit;
	ins = &hit->itInstruments[instrumentNum - 1];
	memset(kbTable, -1, sizeof(kbTable));
	for (int i = 0; i < 120; ++i)
	{
		kbTable[ins->kbTable[i].note] = ins->kbTable[i].sample;
	}

	volEnve.setEnvelope(&ins->volEnve);
	panEnve.setEnvelope(&ins->panEnve);
	pitchEnve.setEnvelope(&ins->pitchEnve);

}

void it_instrument::processBlock(int16_t* outl, int16_t* outr, int length)
{//我先不管那么多，把每个block当成一个tick，后边track就要考虑很多了

	volEnve.updata();
	panEnve.updata();
	pitchEnve.updata();
	bool isUseVolEnve = ins->volEnve.isUseEnve;
	bool isUsePanEnve = ins->panEnve.isUseEnve;
	bool isUsePitchEnve = ins->pitchEnve.isUseEnve;

	float vol = volEnve.getYPos();
	float volK = volEnve.getYPosK();
	float pan = panEnve.getYPos();
	float panK = panEnve.getYPosK();
	float pitch = pitchEnve.getYPos();
	bool isUseFilter = ins->pitchEnve.isUseFilter;

	if (isSampleOK)
	{
		if (!isUseFilter)	sampler.setPitch(note + pitch / 2);//更新一下pitch
		else				sampler.setPitch(note);
		sampler.processBlock(outl, outr, length);//拿一下采样
	}
	//printf("vol:%2.5f volk:%2.5f\n", vol, volK);
	printf("pitch:%.5f\n", pitch);

	float vol2 = vol / 64.0;
	float volk2 = volK / length / 128.0;
	float pan2 = pan / 32.0;
	float panK2 = panK / length / 32.0;
	if (!isUseVolEnve)vol2 = 1.0, volk2 = 0;
	if (!isUsePanEnve)pan2 = 0.0, panK2 = 0;
	for (int i = 0; i < length; ++i)//处理音量包络
	{
		outl[i] *= vol2 * (1.0 - pan2) * 0.5;
		outr[i] *= vol2 * (1.0 + pan2) * 0.5;
		vol2 += volk2;
		pan2 += panK2;
	}

}

/// 
/// class envelope :
/// 


void it_envelope::resetNote()
{
	isNoteOn = 1;
	tickPos = 0;
	nodeN = 0;
}

void it_envelope::setNoteOn()
{
	if (isNoteOn == 0)
	{
		resetNote();
	}
}

void it_envelope::setRelease()
{
	isNoteOn = 0;
}

void it_envelope::updata()
{
	yPos += yPosK;
	if (tickPos >= env->nodes[nodeN].tickPos && nodeN < env->nodeCount)
	{
		yPos = env->nodes[nodeN].yPos;
		nodeN++;
		if (env->isUseSustainLoop && isNoteOn)
		{
			if (nodeN == env->sustainLoopEnd + 1)//碰底了要回去了
			{
				nodeN = env->sustainLoopBegining;
				tickPos = env->nodes[nodeN].tickPos;
				yPos = env->nodes[nodeN].yPos;
			}
			else
			{
				yPosK = (float)(env->nodes[nodeN].yPos - env->nodes[nodeN - 1].yPos) /
					(env->nodes[nodeN].tickPos - env->nodes[nodeN - 1].tickPos);
			}
		}
		else if (env->isUseLoop)
		{
			if (nodeN == env->loopEnd + 1)//碰底了要回去了
			{
				nodeN = env->loopBegining;
				tickPos = env->nodes[nodeN].tickPos;
				yPos = env->nodes[nodeN].yPos;
			}
			else
			{
				yPosK = (float)(env->nodes[nodeN].yPos - env->nodes[nodeN - 1].yPos) /
					(env->nodes[nodeN].tickPos - env->nodes[nodeN - 1].tickPos);
			}
		}
		else
		{
			yPosK = (float)(env->nodes[nodeN].yPos - env->nodes[nodeN - 1].yPos) /
				(env->nodes[nodeN].tickPos - env->nodes[nodeN - 1].tickPos);
		}
	}
	else if (nodeN >= env->nodeCount)
	{
		yPos = env->nodes[nodeN-1].yPos;
		yPosK = 0;
	}
	if (nodeN < env->nodeCount)tickPos++;
}

void it_envelope::setEnvelope(ItInstrument::it_envelope* env)
{
	this->env = env;
	printf("\nisUseEnv:%s isUseLoop:%s isUseSusLoop:%s nodesNum:%2d\n",
		env->isUseEnve ? "true " : "false",
		env->isUseLoop ? "true " : "false",
		env->isUseSustainLoop ? "true " : "false",
		env->nodeCount);
	printf("loopStart:%3d,loopEnd:%3d,susLoopStart:%3d,susLoopEnd:%3d\n",
		env->loopBegining,
		env->loopEnd,
		env->sustainLoopBegining,
		env->sustainLoopEnd);
	for (int i = 0; i < env->nodeCount; ++i)
	{
		printf("-nodeNum%3d: Y:%3d , Tick:%3d\n", i, env->nodes[i].yPos, env->nodes[i].tickPos);
	}
}

float it_envelope::getYPos()
{
	return yPos;
}

float it_envelope::getYPosK()
{
	return yPosK;
}

int it_envelope::getNodeN()
{
	return nodeN;
}
