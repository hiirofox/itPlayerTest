#pragma once

#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "it_pattern.h"
#include "it_sampler.h"
#include "it_instrument.h"
#include "WaveOutDef.h"

std::string noteToString(int note);

class it_player//tick在这里面才细化，底层实现的processBlock就是更新一次tick的，需要提前确定好tick的采样数量!
{
private:
	it_handle* hit;
public:
	it_player();
	void open(it_handle* hit);
	void processBlock(float* outl, float *outr, int length);
};