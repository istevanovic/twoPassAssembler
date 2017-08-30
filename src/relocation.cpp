#include "relocation.h"
#include <fstream>
using namespace std;

extern ifstream in;
extern ofstream out;

Relocation::Relocation(Section* sekcija, int pomeraj, char tip, int simb) {
	//adresa, tip, redni broj simbola,
	
	this->pomeraj = pomeraj;
	this->tip = tip;
	this->simb = simb;
	this->sekcija = sekcija;
	this->next = NULL;
}