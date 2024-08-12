#include "it_instrument.h"


float it_filter::lpf(float vin, float ctof, float reso) {//低通
	float fb = reso + reso / (1.0 - ctof);
	tmp1 += ctof * (vin - tmp1 + fb * (tmp1 - out1));
	out1 += ctof * (tmp1 - out1);
	return out1;
}
void it_filter::reset()
{
	tmp1 = 0;
	out1 = 0;
}

it_instrument::it_instrument()
{
	isSampleOK = 0;
}

void it_instrument::setFilterParam(int ctof, int reso)
{
	this->ctof = 130.82498201149443 * pow(1.0293015223785236, ctof) / 5600.0;
	this->reso = 0.9 * reso / 256.0;
}
void it_instrument::resetNote()
{
	//printf("reset ins\n");
	isNoteOn = 1;
	if (sampler.setSample(hit, kbTable[(int)note]) == -1) isSampleOK = 0;
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
	//printf("pitch:%.5f %.5f\n", pitchEnve.getYPos(), pitchEnve.getYPosK());

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
	if (isUseFilter)
	{
		float ctof2 = (pitch + 32) * 2;
		setFilterParam(ctof2, (uint8_t)ins->initFilterReso);
		printf("ctof:%2.5f , reso:%2.5f\n", ctof, reso);
		for (int i = 0; i < length; ++i)
		{
			float tmpl = outl[i];
			float tmpr = outr[i];
			tmpl = filtL.lpf(tmpl, ctof, reso);
			tmpr = filtR.lpf(tmpr, ctof, reso);
			if (tmpl > 32767.0)tmpl = 32767.0;
			if (tmpl < -32767.0)tmpl = -32767.0;
			if (tmpr > 32767.0)tmpr = 32767.0;
			if (tmpr < -32767.0)tmpr = -32767.0;
			outl[i] = tmpl;
			outr[i] = tmpr;
		}
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
	if (!env->isUseEnve)
	{
		yPos = 0;
		yPosK = 0;
		return;
	}
	yPos += yPosK;
	if (tickPos >= env->nodes[nodeN].tickPos && nodeN < env->nodeCount)
	{
		int nextNodeN = nodeN + 1, isBack = 0;
		if (env->isUseSustainLoop && isNoteOn)
		{
			if (nodeN == env->sustainLoopEnd)//碰底了要回去了
			{
				nextNodeN = env->sustainLoopBegining;
				tickPos = env->nodes[nextNodeN].tickPos;
				isBack = 1;
			}
		}
		else if (env->isUseLoop)
		{
			if (nodeN == env->loopEnd)//碰底了要回去了
			{
				nextNodeN = env->loopBegining;
				tickPos = env->nodes[nextNodeN].tickPos;
				isBack = 1;
			}
		}
		if (isBack)//如果有要回去的
		{
			yPos = env->nodes[nextNodeN].yPos;
			yPosK = 0;
			nodeN = nextNodeN;
		}
		else
		{
			yPos = env->nodes[nodeN].yPos;
			yPosK = (float)(env->nodes[nextNodeN].yPos - env->nodes[nodeN].yPos) / (env->nodes[nextNodeN].tickPos - env->nodes[nodeN].tickPos);
			nodeN = nextNodeN;
		}
	}
	else if (nodeN >= env->nodeCount)
	{
		yPos = env->nodes[nodeN - 1].yPos;
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
