#include <iostream>
#include <fstream>
#include "assembler.h"
using namespace std;

ifstream in;
ofstream out;
Assembler* assembler;

int main(int argc, char* argv[]) {
	in.open(argv[1]);
	out.open(argv[2]);

	if (in.is_open() && out.is_open()) {
		assembler = new Assembler();
		assembler->pass(1);
		cout << "Pass 1 OK!" << endl;
		assembler->pass(2);
		cout << "Pass 2 OK!" << endl;
		assembler->print();
		cout << "Predmetni program generisan u tekstualnoj datoteci " << argv[2] << endl;
		
	}
	else {
		cout << "GRESKA! Zadati fajl ne postoji ili se ne moze otvoriti." << endl;
	}

	

	in.close();
	out.close();
	delete assembler;

	return 1;
}