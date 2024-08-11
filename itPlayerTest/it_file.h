#pragma once

// hiirofox 240805
// 1) ITTECH.txt
// 2) https://fileformats.fandom.com/wiki/Impulse_tracker
// 3) https://wiki.multimedia.cx/index.php/Impulse_Tracker#Axx

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#pragma pack(push, 1) //msvc的内存对齐

namespace ItHead
{
	typedef struct IT_HEAD
	{
		char		IMPM[4];
		char		name[26];
		uint16_t	patHilight;
		uint16_t	ordNum;
		uint16_t	insNum;
		uint16_t	smpNum;
		uint16_t	patNum;
		uint16_t	createdWithTracker;
		uint16_t	compatibleWithTracker;
		//uint16_t	flags;
		bool		isStereo : 1;
		bool		isVol0NoMix : 1;
		bool		isUseInstruments : 1;
		bool		isLinearSlider : 1;
		bool		isOldEffects : 1;
		bool		isLinkEffect : 1;
		bool		isUseMidiPitchController : 1;
		bool		isRequestMidiCfg;
		uint16_t	special;
		uint8_t		globalVolume;
		uint8_t		mixVolume;
		uint8_t		initSpeed;
		uint8_t		initTempo;
		uint8_t		panSeparation;
		uint8_t		pitchWheelDepth;
		uint16_t	msgLength;
		uint32_t	msgOffset;
		char		reserved[4];			// OpenMPT writes "OMPT" here.(mptm)
		uint8_t		initChannelPan[64];
		uint8_t		initChannelVol[64];
		//0x00C0
	}it_head;
	typedef struct IT_HEAD_DATA
	{
		uint8_t* orders;					// bytes=ordNum
		uint32_t* insOffsets;				// bytes=insNum*4
		uint32_t* smpOffsets;				// bytes=smpNum*4
		uint32_t* patOffsets;				// bytes=patNum*4
	}it_head_data;
}

namespace ItInstrument
{
	typedef struct IT_NOTE_SAMPLE_KEYBORAD_TABLE
	{
		uint8_t		note;
		uint8_t		sample;
	}it_keyborad_table;

	typedef struct IT_NODE_POINTS
	{
		int8_t	yPos;
		int16_t	tickPos;
	}it_node_points;

	typedef struct IT_ENVELOPE
	{
		//uint8_t		flag;
		bool		isUseEnve : 1;
		bool		isUseLoop : 1;
		bool		isUseSustainLoop : 1;
		bool		rs1 : 1;
		bool		rs2 : 1;
		bool		rs3 : 1;
		bool		rs4 : 1;
		bool		isUseFilter : 1;//filter和pitch二选一(pitchEnve有效)

		uint8_t		nodeCount;
		uint8_t		loopBegining;
		uint8_t		loopEnd;
		uint8_t		sustainLoopBegining;
		uint8_t		sustainLoopEnd;
		it_node_points  nodes[25];
		char		reserved3[1];
	}it_envelope;

	typedef struct IT_INSTRUMENT
	{
		char		IMPI[4];
		char		dosFileName[12];
		uint8_t		reserved1;
		uint8_t		newNoteAction;			//0:note cut,1:continue,2:note off,3:note fade
		uint8_t		duplicateCheckType;		//0:off,1:note,2:sample,3:instrument
		uint8_t		duplicateCheckAction;	//0:note cut,1:note off,2:note fade
		int16_t		fadeOut;
		int8_t		pitchPanSeperation;		//-32 to 32
		uint8_t		pitchPanCentre;			//0 to 119
		uint8_t		globalVolume;
		uint8_t		defaultPan;
		uint8_t		randomVolumeVariation;
		uint8_t		randomPanVariation;
		uint16_t	trackerVersion;
		uint8_t		numOfSamples;
		uint8_t		reserved2;
		char		instrumentName[26];
		int8_t		initFilterCtof;
		int8_t		initFilterReso;
		int8_t		midiChannel;
		uint8_t		midiProgram;
		uint16_t	midiBank;
		it_keyborad_table kbTable[120];
		it_envelope volEnve;
		it_envelope panEnve;
		it_envelope pitchEnve;
	}it_instrument;
}

namespace ItSample
{
	typedef struct IT_SAMPLE_HEAD
	{
		char			IMPS[4];
		char			dosFileName[12];
		uint8_t			reserved1;
		uint8_t			globalVolume;
		//uint8_t		flags;
		bool			isAssociatedWithHeader : 1;
		bool			is16Bit : 1;
		bool			isStereo : 1;
		bool			isCompressed : 1;
		bool			isUseLoop : 1;
		bool			isUseSustainLoop : 1;
		bool			loopMode : 1;
		bool			sustainLoopMode : 1;

		uint8_t			defaultVolume;
		char			sampleName[26];
		uint8_t			convertFlags;
		uint8_t			defaultPan;
		uint32_t		sampleLen;
		uint32_t		loopBegining;
		uint32_t		loopEnd;
		uint32_t		C5Speed;
		uint32_t		sustainLoopBegining;
		uint32_t		sustainLoopEnd;
		uint32_t		samplePointer;
		uint8_t			vibratoSpeed;
		uint8_t			vibratoDepth;
		uint8_t			vibratoSweep;
		uint8_t			vibratoWaveform;
	}it_sample_head;
	typedef struct IT_SAMPLE_DATA
	{
		uint32_t		fileSize;
		uint16_t		fileDate;
		uint16_t		time;
		uint8_t			fmt;
		void* sampleData;
	}it_sample_data;
}

namespace ItPattern
{
	typedef struct IT_PATTERN_HEAD
	{
		uint16_t		patternLen;
		int16_t			rowCount;
		char			reserved[4];
	}it_pattern_head;

	typedef struct IT_PATTERN_DATA
	{
		void* patternData;
	}it_pattern_data;
}

typedef struct IT_HANDLE
{
	ItHead::it_head				 itHead;
	ItHead::it_head_data		 itHeadData;
	ItInstrument::it_instrument* itInstruments;
	ItSample::it_sample_head* itSampleHead;
	ItSample::it_sample_data* itSampleData;
	ItPattern::it_pattern_head* itPatternHead;
	ItPattern::it_pattern_data* itPatternData;
}it_handle;

#pragma pack(pop)

int itReadFromFile(it_handle* hit, const char* fileName);