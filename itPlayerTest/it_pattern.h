#pragma once

#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "it_file.h"
#include "WaveOutDef.h"

#define MaxChannel 64
#define MaxRowPerChannel 200
std::string noteToString(int note);

typedef struct IT_ROW_DATA
{
	int16_t row;
	int8_t note;
	int8_t instrument;
	int8_t vol;
	int8_t volCmd;//逆天it之vol还包含声像控制
	int8_t cmd;
	int8_t cmdValue;
	bool isNoteChange;
	bool isInsChange;
	bool isVolChange;
	bool isCmdChange;
}it_row_data;

class tracker_pattern
{
private:
public:
};

class it_pattern :public tracker_pattern
{
private:
	int rowCount;
	int channelCount;
	int patternNum;
	it_row_data patternData[64][MaxRowPerChannel];
public:
	void unpackPattern(it_handle* hit, int patternN);
	void printPatternInfo(int channelNum);
	it_row_data getRowData(int channelNum, int rowNum);
	int getRowCount();
	int getChannelCount();
};