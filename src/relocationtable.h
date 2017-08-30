#ifndef _relocationtable_h_
#define _relocationtable_h_


#include "relocation.h"


class RelocationTable {
private:
    Relocation *head, *tail;
public:
    RelocationTable();
    ~RelocationTable();
    
    void add(Relocation* relocation);
	void remove();

    void print();
};

#endif