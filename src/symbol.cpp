#include "symbol.h"
#include <fstream>

extern ifstream in;
extern ofstream out;


Symbol::Symbol(string nazivsimb,string tipsimb, Section* section, int value, bool local, int size, bool isSection) {
	this->nazivsimb = nazivsimb;
	this->tipsimb = tipsimb;
	this->section = section;
	this->value = value;
	this->local = local;
	this->size = size;
	this->next = NULL;
	this->isSection = isSection;
	
	
	
	
	
}

