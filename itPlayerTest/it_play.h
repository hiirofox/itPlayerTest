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

#define MaxInsPerChannel 24
#define MaxBufferLength (SampleRate)


class it_channel
{
private:
	it_handle* hit;
	it_instrument ins[MaxInsPerChannel];
	int insPos;
	float* bufl;
	float* bufr;

	int lastNote, lastVol, lastIns, lastCmd, lastCmdV;
	int lastPan;
public:
	it_channel();
	void setItHandle(it_handle* hit);
	void updataRow(it_row_data unit);
	void processBlock(float* outl, float* outr, int length);
};

#define ItPlayerBufferSize SampleRate
class it_player//tick在这里面才细化，底层实现的processBlock就是更新一次tick的，需要提前确定好tick的采样数量!
{
private:
	it_handle* hit;
	int ordPos;
	int tempo;
	int rowPos, tickCount;
	it_pattern pattern;
	it_channel channels[MaxChannelNum];

	float bufl[MaxBufferLength];//通用buffer
	float bufr[MaxBufferLength];
	float buffer2L[MaxBufferLength];//用来存processBlock1Tick出来的
	float buffer2R[MaxBufferLength];
	float buffer3L[ItPlayerBufferSize];//真正的缓存
	float buffer3R[ItPlayerBufferSize];
	uint64_t readPos, writePos;
public:
	it_player();
	void open(it_handle* hit);
	void processBlock(float* outl, float* outr, int length);
};