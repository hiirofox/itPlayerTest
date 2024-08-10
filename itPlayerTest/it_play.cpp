#include "it_play.h"

std::string noteToString(int note)
{
	std::string str = "";
	if (note % 12 == 0) str = "C-";
	if (note % 12 == 1) str = "C#";
	if (note % 12 == 2) str = "D-";
	if (note % 12 == 3) str = "D#";
	if (note % 12 == 4) str = "E-";
	if (note % 12 == 5) str = "F-";
	if (note % 12 == 6) str = "F#";
	if (note % 12 == 7) str = "G-";
	if (note % 12 == 8) str = "G#";
	if (note % 12 == 9) str = "A-";
	if (note % 12 == 10) str = "A#";
	if (note % 12 == 11) str = "B-";
	char tmp[8];
	sprintf(tmp, "%s%d", str.c_str(), note / 12);
	str = tmp;
	return str;
}

