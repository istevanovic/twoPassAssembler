#ifndef _symbol_h_
#define _symbol_h_

#include "section.h"

#include <string>
using namespace std;

class Symbol {
public:

	int index;
	string nazivsimb;
	string tipsimb;
	Section* section;
	int value;
	bool local;
	int size;
	bool isSection;
	Symbol* next;
	
	Symbol(string nazivsimb,string tipsimb, Section* section, int value, bool local, int size, bool isSection);
};

#endif