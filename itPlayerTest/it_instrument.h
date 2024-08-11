#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "it_sampler.h"
#include "WaveOutDef.h"

#define TickToSampleNum(tempo) (2.5 / tempo * SampleRate) //计算一个tick的采样数


class it_envelope//诶呀it放我一马
{
private:
	ItInstrument::it_envelope* env;
	int tickPos;
	int nodeN;
	bool isNoteOn;
	float yPos, yPosK;
public:
	void setEnvelope(ItInstrument::it_envelope* env);
	void resetNote();
	void setNoteOn();
	void setRelease();
	void updata();
	float getYPos();
	float getYPosK();
	int getNodeN();
};

class tracker_instrument
{
private:
public:
};
class it_instrument :public tracker_instrument
{
private:
	it_handle* hit;
	it_sampler sampler;
	int isSampleOK;
	ItInstrument::it_instrument* ins;
	uint8_t kbTable[256];
	int initNote;//起始音符
	float note;//用来调Pitch的
	bool isNoteOn = 0;

	it_envelope volEnve;
	it_envelope panEnve;
	it_envelope pitchEnve;
public:
	it_instrument();
	void resetNote();
	void setNoteOn();
	void setRelease();
	void setPitch(float note);
	void setInstrument(it_handle* hit, int instrumentNum);
	void processBlock(int16_t* outl, int16_t* outr, int length);
};