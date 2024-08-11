#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "it_sampler.h"
#include "WaveOutDef.h"

#define TickToSampleNum(tempo) (2.5 / tempo * SampleRate) //����һ��tick�Ĳ�����

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
	int initNote;//��ʼ����
	float note;
	int tickPos;
	int volNodeN, panNodeN, pitchNodeN;
	float vol, pan, pitch;//3�������ֵ(filter�Ǻ�pitch����һ������)
	float volK, panK, pitchK;//3������ֵ��б��
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