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

class it_player//tick���������ϸ�����ײ�ʵ�ֵ�processBlock���Ǹ���һ��tick�ģ���Ҫ��ǰȷ����tick�Ĳ�������!
{
private:
	it_handle* hit;
public:
	it_player();
	void open(it_handle* hit);
	void processBlock(float* outl, float *outr, int length);
};