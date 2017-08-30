#ifndef _symboltable_h_
#define _symboltable_h_

#include "symbol.h"


class SymbolTable {
private:
    Symbol *head, *tail;
	int size;
	int numOfSections;

	int insert(Symbol* symbol);
	int addLast(Symbol* symbol);
public:
    SymbolTable();
    ~SymbolTable();
    
	int add(Symbol* symbol);
	
	Symbol* getSymbol(int index);
	Symbol* getSymbol(string name);

    void print();
};

#endif