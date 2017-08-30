#ifndef _relocation_h_
#define _relocation_h_

#include"section.h"



class Relocation {
public:
	Section* sekcija;
	
	int pomeraj;
	char tip;
	int simb;
	Relocation* next;

	Relocation(Section* sekcija, int pomeraj, char tip, int simb);
};

#endif