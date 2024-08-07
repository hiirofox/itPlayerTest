#include "it_file.h"

int itReadFromFile(it_handle* hit, const char* fileName)
{
	using namespace ItHead;
	using namespace ItInstrument;
	using namespace ItSample;
	using namespace ItPattern;
	if (sizeof(it_head) != 0x00C0)
	{
		printf("log: 内存对齐无效!\n");
		return 1;
	}

	FILE* pfr = fopen(fileName, "rb");
	fread(&hit->itHead, 1, 0x00C0, pfr);

	printf("song name:%s\n", hit->itHead.name);
	printf("ordN:%d insN:%d smpN:%d patN:%d\n", hit->itHead.ordNum, hit->itHead.insNum, hit->itHead.smpNum, hit->itHead.patNum);

	hit->itHeadData.orders = (uint8_t*)malloc(sizeof(uint8_t) * hit->itHead.ordNum);
	hit->itHeadData.insOffsets = (uint32_t*)malloc(sizeof(uint32_t) * hit->itHead.insNum);
	hit->itHeadData.smpOffsets = (uint32_t*)malloc(sizeof(uint32_t) * hit->itHead.smpNum);
	hit->itHeadData.patOffsets = (uint32_t*)malloc(sizeof(uint32_t) * hit->itHead.patNum);

	fread(hit->itHeadData.orders, sizeof(uint8_t), hit->itHead.ordNum, pfr);
	fread(hit->itHeadData.insOffsets, sizeof(uint32_t), hit->itHead.insNum, pfr);
	fread(hit->itHeadData.smpOffsets, sizeof(uint32_t), hit->itHead.smpNum, pfr);
	fread(hit->itHeadData.patOffsets, sizeof(uint32_t), hit->itHead.patNum, pfr);

	for (int i = 0; i < hit->itHead.ordNum; ++i)
	{
		printf("%d ", hit->itHeadData.orders[i]);
	}
	printf("\n");

	hit->itInstruments = (it_instrument*)malloc(sizeof(it_instrument) * hit->itHead.insNum);
	for (int i = 0; i < hit->itHead.insNum; ++i)
	{
		fseek(pfr, hit->itHeadData.insOffsets[i], SEEK_SET);
		fread(&hit->itInstruments[i], sizeof(it_instrument), 1, pfr);
		printf("ins%02d impi:%.4s envNodeN:%d name:%s\n", i + 1, hit->itInstruments[i].IMPI, hit->itInstruments[i].envelope.nodeCount, hit->itInstruments[i].instrumentName);
	}

	hit->itSampleHead = (it_sample_head*)malloc(sizeof(it_sample_head) * hit->itHead.smpNum);
	hit->itSampleData = (it_sample_data*)malloc(sizeof(it_sample_data) * hit->itHead.smpNum);
	for (int i = 0; i < hit->itHead.smpNum; ++i)
	{
		fseek(pfr, hit->itHeadData.smpOffsets[i], SEEK_SET);
		fread(&hit->itSampleHead[i], sizeof(it_sample_head), 1, pfr);
		fread(&hit->itSampleData[i], sizeof(it_sample_head) - sizeof(void*), 1, pfr);
		if (hit->itSampleHead[i].sampleLen > 0)
		{
			printf("smp%02d imps:%.4s %s %s %s smpLen:%6d name:%s\n", i + 1,
				hit->itSampleHead[i].IMPS,
				hit->itSampleHead[i].is16Bit ? "16bit" : " 8bit",
				hit->itSampleHead[i].isStereo ? "stereo" : " mono ",
				hit->itSampleHead[i].isCompressed ? "  Comp" : "NoComp",
				hit->itSampleHead[i].sampleLen,
				hit->itSampleHead[i].sampleName);
		}
		//sample data read
		if (hit->itSampleHead[i].sampleLen > 0)
		{
			int sampByte = hit->itSampleHead[i].sampleLen;
			if (hit->itSampleHead[i].is16Bit)	sampByte *= 2;
			if (hit->itSampleHead[i].isStereo)	sampByte *= 2;
			hit->itSampleData[i].sampleData = malloc(sampByte);
			fseek(pfr, hit->itSampleHead[i].samplePointer, SEEK_SET);
			fread(hit->itSampleData[i].sampleData, 1, sampByte, pfr);
		}
	}

	hit->itPatternHead = (it_pattern_head*)malloc(sizeof(it_pattern_head) * hit->itHead.patNum);
	hit->itPatternData = (it_pattern_data*)malloc(sizeof(it_pattern_data) * hit->itHead.patNum);
	for (int i = 0; i < hit->itHead.patNum; ++i)
	{
		fseek(pfr, hit->itHeadData.patOffsets[i], SEEK_SET);
		fread(&hit->itPatternHead[i], sizeof(it_pattern_head), 1, pfr);
		hit->itPatternData[i].patternData = malloc(hit->itPatternHead[i].patternLen);
		fread(hit->itPatternData[i].patternData, 1, hit->itPatternHead[i].patternLen, pfr);
		printf("pat%2d patternLen:%4d\n", i + 1, hit->itPatternHead[i].patternLen);
	}
	return 0;
}