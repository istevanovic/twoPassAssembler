#ifndef _sectiontable_h_
#define _sectiontable_h_

#include "section.h"

class SectionTable {
private:
    Section *head, *tail;
	int numOfSections;
public:
    SectionTable();
    ~SectionTable();
    
    void add(Section* section);
	Section* getFirst();
    
    void print();
};

#endif