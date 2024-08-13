#include "it_sampler.h"


template<typename sample_type>
void it_sampler::processBlockAnyType(float* outl, float* outr, int length)//test
{
	if (isMute)//如果要静音的话
	{
		for (int i = 0; i < length; ++i)
		{
			outl[i] = 0;
			outr[i] = 0;
		}
		return;
	}
	int sampleLen = smpHead->sampleLen;
	sample_type* datl = (sample_type*)smpData->sampleData;
	sample_type* datr;
	if (smpHead->isStereo) datr = (sample_type*)smpData->sampleData + sampleLen;
	else datr = (sample_type*)smpData->sampleData;

	int16_t volume = 1;
	if (sizeof(sample_type) == sizeof(int8_t))
	{
		volume = 256;
	}

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
		outl[i] = volume * datl[(uint32_t)pos];
		outr[i] = volume * datr[(uint32_t)pos];

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

it_sampler::it_sampler()
{

}

int it_sampler::setSample(it_handle* hit, int sampleNum)
{
	if (sampleNum <= 0)
	{
		isMute = 1;
		return -1;
	}
	else isMute = 0;
	smpHead = &hit->itSampleHead[sampleNum - 1];
	smpData = &hit->itSampleData[sampleNum - 1];
	if (smpHead == NULL || smpData == NULL)
	{
		isMute = 1;
		return -1;
	}
}
void it_sampler::resetNote()
{
	pos = 0;
	isNoteOn = 1;
}
void it_sampler::setNoteOn()
{
	if (isNoteOn == 0)
	{
		resetNote();
	}
}
void it_sampler::setRelease()
{
	isNoteOn = 0;
}
void it_sampler::setPitch(float note)
{
	speed = powf(2.0f, (note - 12 * 5) / 12.0f);
	speed *= (float)smpHead->C5Speed / SampleRate;
	//printf("speed:%.5f\n", speed);
}

void it_sampler::setMute(bool isMute)
{
	this->isMute = isMute;
}

void it_sampler::processBlock(float* outl, float* outr, int length)
{
	if (smpHead->is16Bit)
	{
		processBlockAnyType<int16_t>(outl, outr, length);
	}
	else
	{
		processBlockAnyType<int8_t>(outl, outr, length);
	}
}