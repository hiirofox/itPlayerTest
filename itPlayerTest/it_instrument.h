#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "it_sampler.h"
#include "WaveOutDef.h"

#define TickToSampleNum(tempo) (2.5 / tempo * SampleRate) //计算一个tick的采样数

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
	float note;
	int tickPos;
	int volNodeN, panNodeN, pitchNodeN;
	float vol, pan, pitch;//3个包络的值(filter是和pitch共用一个包络)
	float volK, panK, pitchK;//3个包络值的斜率
	bool isNoteOn = 0;
public:
	it_instrument();
	void resetNote();
	void setNoteOn();
	void setRelease();
	void setPitch(float note);
	void getInstrument(it_handle* hit, int instrumentNum);
	void processBlock(int16_t* outl, int16_t* outr, int length);
};