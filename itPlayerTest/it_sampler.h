#pragma once

#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "WaveOutDef.h"

class it_sampler
{
private:
	ItSample::it_sample_head* smpHead;
	ItSample::it_sample_data* smpData;
	float pos, speed;
	bool isNoteOn = 0;
	bool isIntoLoop = 0;	//在各种loop里了
	bool isMute = 0;
	int loopMode;			//loop模式 0:off 1:on 2:bidi
	int loopState;
	template<typename sample_type> void processBlockAnyType(int16_t* outl, int16_t* outr, int length);
public:
	it_sampler();
	int setSample(it_handle* hit, int sampleNum);
	void resetNote();
	void setNoteOn();
	void setRelease();
	void setPitch(float note);
	void processBlock(int16_t* outl, int16_t* outr, int length);
};