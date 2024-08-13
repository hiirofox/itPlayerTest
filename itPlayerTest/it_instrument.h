#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "it_sampler.h"
#include "WaveOutDef.h"

#define TickToSampleNum(tempo) (2.5 / tempo * SampleRate) //����һ��tick�Ĳ�����


class it_envelope//��Ϊ����envelopeֻ����instrument�õ���������������ֿ���
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

class it_filter
{
private:
	float tmp1, out1;
public:
	float lpf(float vin, float ctof, float reso);
	void reset();
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
	float note;//������Pitch��
	bool isNoteOn = 0;
	bool isUnUse;//��ʹ��

	it_envelope volEnve;
	it_envelope panEnve;
	it_envelope pitchEnve;

	it_filter filtL, filtR;
	float ctof, reso;
	void setFilterParam(int ctof, int reso);//ctof:0-127(ZXXЧ����) reso:0-127

	float volume;
public:
	it_instrument();
	void resetNote();
	void setNoteOn();
	void setRelease();//===
	void setNoteCut();//^^ 
	void setUnUse(bool isUnUse);
	void setPitch(float note);
	void setVolume(float volume);//0->64
	void setInstrument(it_handle* hit, int instrumentNum);
	int getNewNoteAction();
	void processBlock(float* outl, float* outr, int length);
};