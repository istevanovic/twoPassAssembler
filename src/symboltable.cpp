#include "symbol.h"
#include "symboltable.h"
#include "section.h"
#include <fstream>

extern ifstream in;
extern ofstream out;


int SymbolTable::insert(Symbol* symbol) {
	Symbol *cur = head;
	int index = 0;
	  
	
		while (cur->next->isSection) {
			cur = cur->next;
			index++;
		}

		symbol->next = cur->next;
		cur->next = symbol;
		symbol->index = ++index;
		size++;
		cur = cur->next->next;
		while (cur) {
			cur->index++;
			cur = cur->next;
		}

		return index;
	}

int SymbolTable::addLast(Symbol* symbol) {
	symbol->index = size++;
	if (head) {
		tail->next = symbol;
		tail = tail->next;
	}
	else
		head = tail = symbol;

	return size - 1;
}

int  SymbolTable::add(Symbol* symbol) {
	if ((symbol->isSection && size > 1) && (numOfSections != size))
	{
		return insert(symbol); numOfSections++;
	}
	else {
		if (symbol->isSection == true) numOfSections++;
		return addLast(symbol);
	}
}


SymbolTable::SymbolTable() {
	head = tail = NULL;
	size = 0;
	numOfSections = 0;
	Symbol* und = new Symbol("UND","SEG/SYM", NULL, 0, true, 0, true);
	add(und);
}

SymbolTable::~SymbolTable() {
	Symbol *oldCur = NULL;
	Symbol *cur = head;
	while (cur) {
		oldCur = cur;
		cur = cur->next;
		delete(oldCur);
	}
}



Symbol*  SymbolTable::getSymbol(int index) {
	Symbol *cur = head;
	while (cur) {
		if (cur->index == index)
			return cur;
		cur = cur->next;
	}

	return NULL;
}

Symbol*  SymbolTable::getSymbol(string name) {
	Symbol *cur = head;
	while (cur) {
		if (!cur->nazivsimb.compare(name))
			return cur;
		cur = cur->next;
	}

	return NULL;
}

void SymbolTable::print() {
	Symbol *cur = head;
	out << "#TabelaSimbola" << endl;
	out <<"#symbol type\t#ID\t#name\t\t#section\t#start adr\t#size\t#flags" << endl;
	while (cur) {
		int sizeSection = 0;
		if (cur->tipsimb == "SEG") {  sizeSection = cur->size - cur->value; }
		if (cur->tipsimb == "SEG/SYM") { cur->isSection = false; }
		
		out << cur->tipsimb << "\t\t" << cur->index << "\t" << cur->nazivsimb << "\t\t"; string lokalitet;
		if (!cur-> isSection) {
			
			if (cur->local == true) lokalitet = 'l';
			else lokalitet = 'g';
		}
		out << (cur->section ? cur->section->simb : 0) << "\t\t" << "0x"<<hex << cur->value << "\t\t" << "0x"<<hex << sizeSection << "\t" << (cur->isSection ? cur->section->flags : lokalitet) << endl;
		cur = cur->next;
	}
}
