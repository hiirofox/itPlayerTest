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
class it_player//tick���������ϸ�����ײ�ʵ�ֵ�processBlock���Ǹ���һ��tick�ģ���Ҫ��ǰȷ����tick�Ĳ�������!
{
private:
	it_handle* hit;
	int ordPos;
	int tempo;
	int rowPos, tickCount;
	it_pattern pattern;
	it_channel channels[MaxChannelNum];

	float bufl[MaxBufferLength];//ͨ��buffer
	float bufr[MaxBufferLength];
	float buffer2L[MaxBufferLength];//������processBlock1Tick������
	float buffer2R[MaxBufferLength];
	float buffer3L[ItPlayerBufferSize];//�����Ļ���
	float buffer3R[ItPlayerBufferSize];
	uint64_t readPos, writePos;
public:
	it_player();
	void open(it_handle* hit);
	void processBlock(float* outl, float* outr, int length);
};