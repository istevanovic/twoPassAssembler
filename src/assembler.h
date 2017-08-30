#ifndef _assembler_h_
#define _assembler_h_

#include "symboltable.h"
#include "relocationtable.h"
#include "sectiontable.h"
#include <string>

class Assembler {
private:
	SymbolTable* symTab;
	SectionTable* secTab;
	RelocationTable* relTab;

	void operationfix1(char operationcode[],char operationcode2[], string operandi);
	void operationfix2(char operationcode[], char operationcode2[], string operandi);
	int extractValue(string operand);
	int opCode(string line);
	void lilendian(char* content);
	void lilendian4b(char* content);
	void lilendianb(char* content);
	bool Assembler::isnumber(string s);
	int simpleMath(string s);
public:
	Assembler();
	~Assembler();
	void print();
	Section* curSec;
	int curLocCnt;
	int currentPass;
	Symbol* findSymbolOutsideSection(string s);
	bool Assembler::nosymbols(string s);
	SymbolTable* getSymbolTable();
	SectionTable* getSectionTable();
	RelocationTable* getRelocationTable();
	void pass(int pass);
	

};

#endif