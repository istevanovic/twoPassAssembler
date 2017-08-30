#include "section.h"
#include "assembler.h"
#include <fstream>
#include  <iomanip>
using namespace std;

extern ifstream in;
extern ofstream out;
extern Assembler* assembler;

Section::Section(int symbol) {
	this->simb = symbol;
	this->sadrzajsek = new char[4096];
	this->contentSize = 0;
	this->next = NULL;
	this->flags = " ";
}

void Section::write(char *data, int cnt) {
	int i = 0;
	while (i < cnt)
		sadrzajsek[contentSize++] = data[i++];
}

Section::~Section() {
	delete[]sadrzajsek;
}