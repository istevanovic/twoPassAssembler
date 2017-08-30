#include "relocation.h"
#include "assembler.h"
#include <fstream>
using namespace std;

extern ifstream in;
extern ofstream out;
extern Assembler* assembler;


RelocationTable::RelocationTable() {
	head = tail = 0;
}

RelocationTable::~RelocationTable() {
	Relocation *cur = head, *old = NULL;
	while (cur) {
		old = cur;
		cur = cur->next;
		delete(old);
	}
}



void RelocationTable::add(Relocation* relocation) {
	if (head) {
		tail->next = relocation;
		tail = relocation;
	}
	else
		head = tail = relocation;
}

void RelocationTable::remove() {
	Relocation *cur = head, *old1 = NULL, *old2 = NULL;
	while (cur) {
		old2 = old1;
		old1 = cur;
		cur = cur->next;
	}

	if (old1) {
		old2->next = NULL;
		delete old1;
	}

	cur = head, old1 = NULL, old2 = NULL;
	while (cur) {
		old2 = old1;
		old1 = cur;
		cur = cur->next;
	}

	if (old1) {
		old2->next = NULL;
		delete old1;
	}
}

void RelocationTable::print() {
	Section *curSection = assembler->getSectionTable()->getFirst();
	curSection = curSection->next;
	while (curSection) {
		out << "#rel" << assembler->getSymbolTable()->getSymbol(curSection->simb)->nazivsimb << endl;
		out << "#offset\t#type\t#symbol ID" << endl;
		Relocation* curRelocation = assembler->getRelocationTable()->head;
		while (curRelocation) {
			if (curRelocation->sekcija == curSection) {
				out << "0x"<<hex<< curRelocation->pomeraj << "\t" ;

				if (curRelocation->tip == 'r')
					out << "R";
				if (curRelocation->tip == 'a')
					out << "A";
				
				out << "\t" << curRelocation->simb << endl;
			}
			curRelocation = curRelocation->next;
		}

		curSection = curSection->next;
		out << endl;
	}
}