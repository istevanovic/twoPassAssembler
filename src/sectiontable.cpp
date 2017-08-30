#include "section.h"
#include "sectiontable.h"
#include "assembler.h"
#include <fstream>
#include  <iomanip>
using namespace std;

extern ifstream in;
extern ofstream out;
extern Assembler* assembler;




SectionTable::SectionTable() {
	head = tail = 0;
	add(new Section(0));
}

SectionTable::~SectionTable() {
	Section *cur = head, *old = NULL;
	while (cur) {
		old = cur;
		cur = cur->next;
		delete(old);
	}
}

void SectionTable::add(Section* section) {
	if (head) {
		tail->next = section;
		tail = tail->next;
	}
	else
		head = tail = section;
}

Section* SectionTable::getFirst() {
	return head;
}

void SectionTable::print() {
	Section *cur = head;
	out << endl;
	out << "#SadrzajSekcija" << endl;
	cur = cur->next;
	while (cur) {
		Symbol* sectionSymbol = assembler->getSymbolTable()->getSymbol(cur->simb);
		out << "<" << sectionSymbol->nazivsimb << ">" << endl;
		int i = 0;
		while (i < (sectionSymbol->section->contentSize)) {
			if (cur->sadrzajsek[i] != '?')
			out << hex << (((int)cur->sadrzajsek[i++]) & 0xff);
			else out  << (cur->sadrzajsek[i++]);
			if (i % 2 == 0) out << " ";
			//out << " ";
			if (i % 8 == 0) out << endl;
			//if (i % 2 == 0) out << endl;
		}
		cur = cur->next;
		out << endl;
		out << endl;
	}
}