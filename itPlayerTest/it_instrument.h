#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "it_sampler.h"
#include "WaveOutDef.h"

#define TempoToSampleNum(tempo) (((2500.0 / tempo) * 0.001) * SampleRate) //计算一个tick的采样数

class tracker_instrument
{
private:
public:
};
class it_instrument :public tracker_instrument
{
private:
	it_sampler sampler;
	ItInstrument::it_instrument ins;
	float note;
	float pos;
	bool isNoteOn = 0;
public:
	void resetNote();
	void setNoteOn();
	void setRelease();
	void setPitch(float note);
	void getInstrument(it_handle* hit, int instrumentNum);
};