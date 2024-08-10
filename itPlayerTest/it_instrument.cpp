#include "it_instrument.h"

void it_instrument::resetNote()
{
	isNoteOn = 1;
	pos = 0;
	sampler.resetNote();
}

void it_instrument::setNoteOn()
{
	if (isNoteOn == 0)
	{
		isNoteOn = 1;
		pos = 0;
		sampler.setNoteOn();
	}
}

void it_instrument::setRelease()
{
	isNoteOn = 0;
}

void it_instrument::setPitch(float note)
{
	this->note = note;
}

void it_instrument::getInstrument(it_handle* hit, int instrumentNum)
{
	ins = hit->itInstruments[instrumentNum - 1];
	sampler.getSample(hit, ins.numOfSamples);
}
