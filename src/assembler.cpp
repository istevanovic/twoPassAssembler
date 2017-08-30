#include "assembler.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include <cmath>
using namespace std;

extern ifstream in;
extern ofstream out;
int sectionExpected = 0;
int sectionStartAddress = 0;

int relSemaphore = 0;
int zeroSemaphore = 0;
int globalSymSemaphore = 0;

void Assembler::print() {
	symTab->print();
	secTab->print();
	relTab->print();
}



int Assembler::opCode(string line) {
	if (!line.empty()){
		if (line.find("INT") != string::npos)
			return 0; //1 REC
		if (line.find("RET") != string::npos)
			return 1; //1 REC
		if (line.find("JMP") != string::npos)
			return 2; //1-2 RECI
		if (line.find("CALL") != string::npos)
			return 3;//1-2 RECI
		if (line.find("JZ") != string::npos)
			return 4; //1-2 RECI
		if (line.find("JNZ") != string::npos)
			return 5; // 1-2 RECI
		if (line.find("JGZ") != string::npos)
			return 6; // 1-2 RECI
		if (line.find("JGEZ") != string::npos)
			return 7;  // 1-2 RECI
		if (line.find("JLZ") != string::npos)
			return 8;  // 1-2 RECI
		if (line.find("JLEZ") != string::npos)
			return 9;  // 1-2 RECI
		if (line.find("LOAD") != string::npos) 
			return 10; // 1-2 RECI
		if (line.find("STORE") != string::npos)
			return 11; // 1-2 RECI
		if (line.find("PUSH") != string::npos)
			return 20; //1 REC
		if (line.find("POP") != string::npos)
			return 21; //1 REC
		if (line.find("ADD") != string::npos)
			return 30;
		if (line.find("SUB") != string::npos)
			return 31;
		if  (line.find("MUL") != string::npos)
			return 32;
		if (line.find("DIV") != string::npos)
			return 33; //1 REC
		if (line.find("MOD") != string::npos)
			return 34; //1 REC
		if (line.find("AND") != string::npos)
			return 35;
		if  (line.find("XOR") != string::npos)
			return 37; //1 REC
		if (line.find("OR") != string::npos)
		{
			if (line.find("ORG") != string::npos) return -1;
			else return 36;
		}
			
		if (line.find("NOT") != string::npos)
			return 38; //1 REC
		if (line.find("ASL") != string::npos)
			return 39; //1 REC
		if (line.find("ASR") != string::npos)
			return 58;
	}
	return -1;
}

Assembler::Assembler() {
	symTab = new SymbolTable();
	secTab = new SectionTable();
	relTab = new RelocationTable();

	curSec = NULL;
	curLocCnt = 0;
}

Assembler::~Assembler() {
	delete symTab;
	delete secTab;
	delete relTab;
}

SymbolTable* Assembler::getSymbolTable() {
	return symTab;
}

SectionTable* Assembler::getSectionTable() {
	return secTab;
}

RelocationTable* Assembler::getRelocationTable() {
	return relTab;
}



void Assembler::pass(int pass)
{	

	string line;

	if (pass == 1) currentPass = 1;
	else currentPass = 2;
	
	while (line != ".end" ) {

		
		
		int komentar = line.find_first_of(";", 0);
		if (komentar != -1) { line = line.substr(0, komentar - 1); }
		int spaceremover = line.find_last_not_of(" ");
		line = line.substr(0, spaceremover +1);
		
		
		
		if (sectionExpected == 1)
		{
			if (!(!line.compare(0, 5, ".data") || !line.compare(0, 5, ".text") || !line.compare(0, 4, ".bss")))
				cout << "GRESKA!posle ORG direktive mora uslediti sekcija" << endl;
		}

		if ((line.find("DEF") != std::string::npos))
		{
			int start = 0, end = 0;

			start = line.find_first_of(" ");
			string tok = line.substr(0, start);
			string rest = line.substr(start + 1, line.length());
			start = rest.find_first_of(" ");
			rest = rest.substr(start + 1, line.length());
			int Value = extractValue(rest);


			Symbol* symbol = symTab->getSymbol(tok);
			if (symbol) {
				symbol->value = Value;
				symbol->section = curSec;
			}
			else
				symTab->add(new Symbol(tok, "SYM", curSec, Value, true, 0, false));
		}



		if ((!line.compare(0, 3, "ORG")))
		{
			sectionExpected = 1;
			int start = 0, end = 0;
			start = line.find_first_of(" ", start);
			end = line.length();
			string tok = line.substr(start+1, end - start);
			sectionStartAddress = extractValue(tok);
			
		}


		if ((!line.compare(0, 7, ".global") || !line.compare(0, 7, ".extern")))
		{	
			string directive = line.substr(0, 7);
			line = line.substr(8, string::npos);
			
			int start = 0, end = 0;
			while (end != string::npos)
			{
				end = line.find_first_of(" ,\n", start);
				string tok = line.substr(start, end - start);
				start = end + 2;

				Symbol* symbol = symTab->getSymbol(tok);
				

				if (symbol)
				{
					if (!directive.compare(0, 7, ".extern"))
					{
						if ((pass == 2) && (symbol->section != 0)) cout << "GRESKA: Navedeni simbol '" << tok << "'je definisan a naveden kao eksterni!" << endl;
						symbol->local = false;
					}

					if (!directive.compare(0, 7, ".global"))
					{
						if ((pass == 2) && (symbol->section == 0)) cout << "GRESKA: Navedeni simbol '" << tok << "'se izvozi a nije definisan!" << endl;
						symbol->local = false;
					}

				}

				if (symbol == NULL) 
				{
					if (pass == 1)  symTab->add(new Symbol(tok, "SYM", NULL, 0, false, 0, false));

				}

			}
		}

		if (!line.compare(0, 5, ".data") || !line.compare(0, 5, ".text") || !line.compare(0, 4, ".bss") || !line.compare(0, 7, ".rodata"))
		{

			int start = 0, end = 0;
			end = line.find_first_of("\n", start);
			string tok = line.substr(start, end - start);

			if (pass == 1)
			{
				if (curSec)
					symTab->getSymbol(curSec->simb)->size = curLocCnt;


				Symbol* symbol = new Symbol(tok, "SEG", NULL, sectionStartAddress, true, 0, true);
				Section *section = new Section(symTab->add(symbol));
				symbol->section = section;
				if (!line.compare(0, 5, ".data")) section->flags = "WRITE, ALLOC";
				if (!line.compare(0, 5, ".text")) section->flags = "EXECINSTR, ALLOC";
				if (!line.compare(0, 4, ".bss"))  section->flags = "WRITE, ALLOC"; ;
				if (!line.compare(0, 7, ".rodata")) section->flags = "ALLOC"; ;

				secTab->add(section);

				curSec = section;
			}
			else
				curSec = curSec->next;
			curLocCnt = sectionStartAddress;

			if (sectionExpected == 1)
			{
				sectionExpected = 0;
				sectionStartAddress = 0;
			}

			else
			{
				curLocCnt = sectionStartAddress;
				sectionExpected = 0;
			}

		}

		int end = 0, label = 0, end2 = 0, start = 0;
		if (pass == 1 && ((end = line.find_first_of(":", 0)) != string::npos)) {
			label = 1;
			end2 = line.find_first_of(" ", start);
			string tok = line.substr(0, end);
			if (end2 != -1) line = line.substr(end + 2, line.length());


			Symbol* symbol = symTab->getSymbol(tok);
			if (symbol) {
				symbol->value = curLocCnt;
				symbol->section = curSec;
			}
			else
				symTab->add(new Symbol(tok, "SYM", curSec, curLocCnt, true, 0, false));
		}
		else if (pass == 2 && ((end = line.find_first_of(":", 0)) != string::npos)) {
			end2 = line.find_first_of(" ", start);
			string tok = line.substr(0, end);
			if (end2 != -1) line = line.substr(end + 2, line.length());
		}
		int operationCode = opCode(line);
		if (line[0] != '.' && operationCode != -1) {
			if (pass == 2) {
				char operation[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
				char secondOperation[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

				int labelremove = line.find_first_of(":", 0);
				if (labelremove != -1) {
					line = line.substr(labelremove+2, line.length());
				}

				int start = 0, end = 0;
				start = line.find_first_of(" ", start) + 1;
				string operands = line.substr(start, string::npos);

				if (operationCode == 0)
				{ //INT
					operation[0] = 0;
					operation[1] = 0;
					operation[2] = 0;
					start = operands.find_first_of("R", 0);
					string regIdent = operands.substr(start + 1, operands.length());
					if (regIdent == "10") operation[3] = 0xA;
					else if (regIdent == "11") operation[3] = 0xB;
					else if (regIdent == "12") operation[3] = 0xC;
					else if (regIdent == "13") operation[3] = 0xD;
					else if (regIdent == "14") operation[3] = 0xE;
					else if (regIdent == "15") operation[3] = 0xF;
					else if (regIdent == "PC") { operation[2] = 1; operation[3] = 1; }
					else if (regIdent == "SP") { operation[2] = 1; operation[3] = 0; }
					else operation[3] = stoi(regIdent);

				}
				//^ok
				if (operationCode == 1)
				{ //RET
					operation[1] = 1;
				}
				//^ok
				if (operationCode == 2)
				{ // JMP

					operation[1] = 2;
					operationfix1(operation, secondOperation, operands);


				}

				if (operationCode == 3)
				{ //CALL 
					operation[1] = 3;
					operationfix1(operation, secondOperation, operands);

				}


				if ((operationCode == 4) || (operationCode == 5) || (operationCode == 6) || (operationCode == 7) || (operationCode == 8) || (operationCode == 9))
				{
					if (operationCode == 4) { operation[0] = 0; operation[1] = 4; } // JZ
					if (operationCode == 5) { operation[0] = 0; operation[1] = 5; } // JNZ
					if (operationCode == 6) { operation[0] = 0; operation[1] = 6; } // JGZ
					if (operationCode == 7) { operation[0] = 0; operation[1] = 7; } // JGEZ
					if (operationCode == 8) { operation[0] = 0; operation[1] = 8; } // JLZ
					if (operationCode == 9) { operation[0] = 0; operation[1] = 9; } // JLEZ

					operationfix2(operation, secondOperation, operands);
				}

				if (operationCode == 20) {
					operation[0] = 2;
					operation[1] = 0;
					operation[2] = 0;
					start = operands.find_first_of(" ", 0);
					string regIdent = operands.substr(start + 1, operands.length());
					if (regIdent == "R10") operation[3] = 0xA;
					else if (regIdent == "R11") operation[3] = 0xB;
					else if (regIdent == "R12") operation[3] = 0xC;
					else if (regIdent == "R13") operation[3] = 0xD;
					else if (regIdent == "R14") operation[3] = 0xE;
					else if (regIdent == "R15") operation[3] = 0xF;
					else if (regIdent == "PC") { operation[2] = 1; operation[3] = 1; }
					else if (regIdent == "SP") { operation[2] = 1; operation[3] = 0; }
					else {
						regIdent = regIdent.substr(1, regIdent.length()); operation[3] = stoi(regIdent);
							}	   
					operation[4] = 0;
					operation[5] = 0;
					operation[6] = 0;
					operation[7] = 0;
				}

				if (operationCode == 21) {
					operation[0] = 2;
					operation[1] = 1;
					operation[2] = 0;
					start = operands.find_first_of(" ", 0);
					string regIdent = operands.substr(start + 1, operands.length());
					if (regIdent == "R10") operation[3] = 0xA;
					else if (regIdent == "R11") operation[3] = 0xB;
					else if (regIdent == "R12") operation[3] = 0xC;
					else if (regIdent == "R13") operation[3] = 0xD;
					else if (regIdent == "R14") operation[3] = 0xE;
					else if (regIdent == "R15") operation[3] = 0xF;
					else if (regIdent == "PC") { operation[2] = 1; operation[3] = 1; }
					else if (regIdent == "SP") { operation[2] = 1; operation[3] = 0; }
					else {
						regIdent = regIdent.substr(1, regIdent.length()); operation[3] = stoi(regIdent);
					}
					operation[4] = 0;
					operation[5] = 0;
					operation[6] = 0;
					operation[7] = 0;
				}

				if ((operationCode >= 30) && (operationCode <= 58))
				{
					if (operationCode == 30) { operation[0] = 3; operation[1] = 0; }
					if (operationCode == 31) { operation[0] = 3; operation[1] = 1; }
					if (operationCode == 32) { operation[0] = 3; operation[1] = 2; }
					if (operationCode == 33) { operation[0] = 3; operation[1] = 3; }
					if (operationCode == 34) { operation[0] = 3; operation[1] = 4; }
					if (operationCode == 35) { operation[0] = 3; operation[1] = 5; }
					if (operationCode == 36) { operation[0] = 3; operation[1] = 6; }
					if (operationCode == 37) { operation[0] = 3; operation[1] = 7; }
					if (operationCode == 38) {
						operation[0] = 3; operation[1] = 8;
						start = operands.find_first_of("R", 0);
						end = operands.find_first_of(",", 0);
						string regIdent = operands.substr(start + 1, end - 1);

						operation[2] = 0;
						if (regIdent == "10") operation[3] = 0xA;
						if (regIdent == "11") operation[3] = 0xB;
						if (regIdent == "12") operation[3] = 0xC;
						if (regIdent == "13") operation[3] = 0xD;
						if (regIdent == "14") operation[3] = 0xE;
						if (regIdent == "15") operation[3] = 0xF;
						else operation[3] = stoi(regIdent);

						operands = operands.substr(end + 1, operands.length());
						start = operands.find_first_of("R", 0);
						end = operands.find_first_of(",", 0);
						regIdent = operands.substr(start + 1, end - 1);
						//int firstBit, secondBit = 0;
						if (regIdent == "10") { operation[4] = 0xA / 2; operation[5] = (0xA % 2) * 8; }
						if (regIdent == "11") { operation[4] = 0xB / 2;  operation[5] = (0xB % 2) * 8; }
						if (regIdent == "12") { operation[4] = 0xC / 2;  operation[5] = (0xC % 2) * 8; }
						if (regIdent == "13") { operation[4] = 0xD / 2;  operation[5] = (0xD % 2) * 8; }
						if (regIdent == "14") { operation[4] = 0xE / 2;  operation[5] = (0xE % 2) * 8; }
						if (regIdent == "15") { operation[4] = 0xF / 2;  operation[5] = (0xF % 2) * 8; }
						else {
							operation[4] = stoi(regIdent) / 2; operation[5] = (stoi(regIdent) % 2) * 8;
						}
					}
					if (operationCode == 39) { operation[0] = 3; operation[1] = 9; }
					if (operationCode == 58) { operation[0] = 3; operation[1] = 0xA; }

					if (operationCode != 38) {
						start = operands.find_first_of("R", 0);
						end = operands.find_first_of(",", 0);
						string regIdent = operands.substr(start + 1, end - 1);

						operation[2] = 0;
						if (regIdent == "10") operation[3] = 0xA;
						if (regIdent == "11") operation[3] = 0xB;
						if (regIdent == "12") operation[3] = 0xC;
						if (regIdent == "13") operation[3] = 0xD;
						if (regIdent == "14") operation[3] = 0xE;
						if (regIdent == "15") operation[3] = 0xF;
						else operation[3] = stoi(regIdent);

						operands = operands.substr(end + 1, operands.length());
						start = operands.find_first_of("R", 0);
						end = operands.find_first_of(",", 0);
						regIdent = operands.substr(start + 1, end - 2);
						int firstBit, secondBit = 0;
						if (regIdent == "10") {
							operation[4] = 0xA >> 1;  firstBit = 0xA % 10;
						}
						if (regIdent == "11") { operation[4] = 0xB / 2;  firstBit = 0xB % 2; }
						if (regIdent == "12") { operation[4] = 0xC / 2;  firstBit = 0xC % 2; }
						if (regIdent == "13") { operation[4] = 0xD / 2;  firstBit = 0xD % 2; }
						if (regIdent == "14") { operation[4] = 0xE / 2;   firstBit = 0xE % 2; }
						if (regIdent == "15") { operation[4] = 0xF / 2;   firstBit = 0xF % 2; }
						else {
							operation[4] = stoi(regIdent) / 2; firstBit = stoi(regIdent) % 2;
						}



						operands = operands.substr(end + 1, operands.length());
						start = operands.find_first_of("R", 0);
						regIdent = operands.substr(start + 1, operands.length());

						if (regIdent == "10") { operation[5] = (0xA >> 2) + firstBit * 8;  secondBit = 0xA % 2; firstBit = (0xA / 2) % 2; operation[6] = firstBit * 8 + secondBit * 4; }
						if (regIdent == "11") { operation[5] = (0xB >> 2) + firstBit * 8;  secondBit = 0xB % 2; firstBit = (0xB / 2) % 2; operation[6] = firstBit * 8 + secondBit * 4; }
						if (regIdent == "12") { operation[5] = (0xC >> 2) + firstBit * 8;  secondBit = 0xC % 2; firstBit = (0xC / 2) % 2; operation[6] = firstBit * 8 + secondBit * 4; }
						if (regIdent == "13") { operation[5] = (0xD >> 2) + firstBit * 8;  secondBit = 0xD % 2; firstBit = (0xD / 2) % 2; operation[6] = firstBit * 8 + secondBit * 4; }
						if (regIdent == "14") { operation[5] = (0xE >> 2) + firstBit * 8;  secondBit = 0xE % 2; firstBit = (0xE / 2) % 2; operation[6] = firstBit * 8 + secondBit * 4; }
						if (regIdent == "15") { operation[5] = (0xF >> 2) + firstBit * 8;  secondBit = 0xF % 2; firstBit = (0xF / 2) % 2; operation[6] = firstBit * 8 + secondBit * 4; }
						else { operation[5] = (stoi(regIdent) >> 2) + firstBit * 8;  secondBit = stoi(regIdent) % 2; firstBit = (stoi(regIdent) / 2) % 2; operation[6] = firstBit * 8 + secondBit * 4; }





					}
				}


				if ((operationCode == 10) || (operationCode == 11)) {
					if (operationCode == 10) { 
						operation[0] = 1; operation[1] = 0;
						if (line.find("LOADUB") != string::npos) { operation[6] = 3; operation[7] = 8; }
						if (line.find("LOADSB") != string::npos) { operation[6] = 1; operation[7] = 8; }
						if (line.find("LOADUW") != string::npos) { operation[6] = 0; operation[7] = 8; }
						if (line.find("LOADSW") != string::npos) { 
							operation[6] = 2; operation[7] = 8; }
					}
					if (operationCode == 11) { 
						operation[0] = 1; operation[1] = 1;
						if (line.find("STOREUB") != string::npos) { operation[6] = 3; operation[7] = 8; }
						if (line.find("STORESB") != string::npos) { operation[6] = 1; operation[7] = 8; }
						if (line.find("STOREUW") != string::npos) { operation[6] = 0; operation[7] = 8; }
						if (line.find("STORESW") != string::npos) { 
							operation[6] = 2; operation[7] = 8; }
					}

					

					operationfix2(operation, secondOperation, operands);




				}






				curSec->write(operation, 8);
				
				curLocCnt += 4;
				int tester = 0;
				for (int i = 0; i < 8; i++) tester += secondOperation[i];
				if ((tester > 0 && globalSymSemaphore == 0 ) || ( zeroSemaphore == 1 )) {
					//lilendian(secondOperation);
					curSec->write(secondOperation, 8); 
					zeroSemaphore = 0; curLocCnt += 4;
					globalSymSemaphore = 0;
				}

				if (tester < 0) {
					for (int i = 0; i < 8; i++) { secondOperation[i] += 0xf; if ((i == 7) && (secondOperation[7] != 15)) secondOperation[i]++; 
					//lilendian(secondOperation);
					}
					if(tester>0) lilendian(secondOperation);
					curSec->write(secondOperation, 8);


				}

				if (globalSymSemaphore == 1) {
					secondOperation[0] = 0; 
					secondOperation[1] = 0;
					secondOperation[2] = 0;
					secondOperation[3] = 0;
					secondOperation[4] = 0;
					secondOperation[5] = 0;
					secondOperation[6] = 0;
					secondOperation[7] = 0;
					globalSymSemaphore = 0;
					curSec->write(secondOperation, 8);  curLocCnt += 4;
				
				}

				}
				else if (pass == 1) {
					curLocCnt += 4;
					int adrIdent1 = 0; int adrIdent2 = 0;
					if (line.find("$") != string::npos)  adrIdent1 = 1;
					if (line.find("#") != string::npos)  adrIdent2 = 1;
					string operand;
					if (line.find(",") != string::npos)
					{
						operand = line.substr(line.find_first_of(",", 0), line.length());
					}
					operand = operand.substr(operand.find_first_of(" ", 0) + 1, operand.length());
					if (((operationCode>= 2) && (operationCode<=11)) && (((adrIdent1 != 0) || (adrIdent2!=0)) ||
						((operand !="R0") && (operand != "R1") && (operand != "R2") && (operand != "R3") && (operand != "R4") && (operand != "R5") && (operand != "R6") &&
							(operand != "R7") && (operand != "R8") && (operand !="R9"))))
						
						curLocCnt += 4;
					
				}
			
			}




			if (!line.compare(0, 2, "DD")) {
				
				int numrep = 1;

				if (line.find("DUP") != std::string::npos) {
					
					line = line.substr(3, line.length());
					int start = line.find_first_of(" ");
					string rep = line.substr(0, start);
					 numrep = extractValue(rep);
					start = line.find_first_of(" ");
					line = line.substr(start + 3, line.length());
				}


				line = line.substr(2, string::npos);
				int test = line.find_first_of(" ");
				if (test == 0) line = line.substr(1, line.length());

				int start = 0, end = 0;
				while (end != string::npos) {
					end = line.find_first_of(",\n", start);
					string tok = line.substr(start, end - start);
					start = end + 2;

					if (pass == 1)
						for (int i = 0; i < numrep; i++)
							curLocCnt += 8;

					else if (pass == 2)
						for (int i = 0; i < numrep; i++)
					{
						char content[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
						
						if (tok == "?") { curSec->write(content, 16); curLocCnt += 8; }

						else {

							if (nosymbols(tok))
							{
								if (!isnumber(tok))
								{
									symTab->add(new Symbol(tok, "SYM", 0, 0, false, 0, 0));
									relTab->add(new Relocation(curSec, curLocCnt, 'a', symTab->getSymbol(tok)->index));
									curSec->write(content, 16);
									curLocCnt += 8;
								}

								if (isnumber(tok))
								{

									int value = extractValue(tok);

									int i = 15;
									while (value != 0) {
										int bajt = value % 16;
										content[i] = bajt;
										i = i - 1;
										value = value / 16;
									}
									lilendian4b(content);

									curSec->write(content, 16);
									curLocCnt += 8;
								}
							}

							else {
								int vrednost = extractValue(tok);
								Symbol* s = findSymbolOutsideSection(tok);


								if ((s == NULL)) {
									Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', curSec->simb);
									relTab->add(relokacija);
									curLocCnt += 8;

									int i = 15;
									while (vrednost != 0) {
										int bajt = vrednost % 16;
										content[i] = bajt;
										i = i - 1;
										vrednost = vrednost / 16;
									}
									lilendian4b(content);

								}

								else if (s->local == false) {

									Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', s->index);
									relTab->add(relokacija);
									curLocCnt += 8;

								}

								else if (s->local == true) {

									if (s->section == curSec) {
										Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', curSec->simb);
										relTab->add(relokacija);
										curLocCnt += 8;

										int i = 15;
										while (vrednost != 0) {
											int bajt = vrednost % 16;
											content[i] = bajt;
											i = i - 1;
											vrednost = vrednost / 16;
										}
										lilendian4b(content);

									}

									if (s->section != curSec) {
										Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', s->section->simb);
										relTab->add(relokacija);
										curLocCnt += 8;

										int i = 15;
										while (vrednost != 0) {
											int bajt = vrednost % 16;
											content[i] = bajt;
											i = i - 1;
											vrednost = vrednost / 16;
										}
										lilendian4b(content);

									}



								}
								curSec->write(content, 16);
							}



						}


					}
				}
			}

			
			if (!line.compare(0, 2, "DW")) {

				int numrep = 1;

				if (line.find("DUP") != std::string::npos) {

					line = line.substr(3, line.length());
					int start = line.find_first_of(" ");
					string rep = line.substr(0, start);
					numrep = extractValue(rep);
					start = line.find_first_of(" ");
					line = line.substr(start + 3, line.length());
				}

				line = line.substr(2, string::npos);
				int test = line.find_first_of(" ");
				if (test == 0) line = line.substr(1, line.length());

				int start = 0, end = 0;
				while (end != string::npos) {
					end = line.find_first_of(",\n", start);
					string tok = line.substr(start, end - start);
					start = end + 2;

					if (pass == 1)
						for (int i = 0; i < numrep; i++)
							curLocCnt += 4;

					else if (pass == 2) {
						for (int i = 0; i < numrep; i++) {
							char content[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

							if (tok == "?") { curSec->write(content, 8); curLocCnt += 4; }

							else {
								if (nosymbols(tok))
								{
									if (!isnumber(tok))
									{
										symTab->add(new Symbol(tok, "SYM", 0, 0, false, 0, 0));
										relTab->add(new Relocation(curSec, curLocCnt, 'a', symTab->getSymbol(tok)->index));
										curSec->write(content, 8);
										curLocCnt += 4;;
									}

									if (isnumber(tok))
									{

										int value = extractValue(tok);

										int i = 7;
										while (value != 0) {
											int bajt = value % 16;
											content[i] = bajt;
											i = i - 1;
											value = value / 16;
										}
										lilendian(content);

										curSec->write(content, 8);
										curLocCnt += 4;
									}
								}

								else {
									int vrednost = extractValue(tok);
									Symbol* s = findSymbolOutsideSection(tok);


									if (s == NULL) {
										Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', curSec->simb);
										relTab->add(relokacija);
										curLocCnt += 4;

										int i = 7;
										while (vrednost != 0) {
											int bajt = vrednost % 16;
											content[i] = bajt;
											i = i - 1;
											vrednost = vrednost / 16;
										}
										lilendian(content);

									}

									else if (s->local == false) {

										Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', s->index);
										relTab->add(relokacija);
										curLocCnt += 4;

									}

									else if (s->local == true) {

										if (s->section == curSec) {
											Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', curSec->simb);
											relTab->add(relokacija);
											curLocCnt += 4;

											int i = 7;
											while (vrednost != 0) {
												int bajt = vrednost % 16;
												content[i] = bajt;
												i = i - 1;
												vrednost = vrednost / 16;
											}
											lilendian(content);

										}

										if (s->section != curSec) {
											Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', s->section->simb);
											relTab->add(relokacija);
											curLocCnt += 4;

											int i = 7;
											while (vrednost != 0) {
												int bajt = vrednost % 16;
												content[i] = bajt;
												i = i - 1;
												vrednost = vrednost / 16;
											}
											lilendian(content);

										}



									}
									curSec->write(content, 8);
								}


							}



						}
					}
				}
			}

			if (!line.compare(0, 2, "DB")) {

				int numrep = 1;

				if (line.find("DUP") != std::string::npos) {

					line = line.substr(3, line.length());
					int start = line.find_first_of(" ");
					string rep = line.substr(0, start);
					numrep = extractValue(rep);
					start = line.find_first_of(" ");
					line = line.substr(start + 3, line.length());
				}

				line = line.substr(2, string::npos);
				int test = line.find_first_of(" ");
				if (test == 0) line = line.substr(1, line.length());

				int start = 0, end = 0;
				while (end != string::npos) {
					end = line.find_first_of(",\n", start);
					string tok = line.substr(start, end - start);
					start = end + 2;

					if (pass == 1)

						for (int i = 0; i < numrep; i++)
							curLocCnt += 2;

					else if (pass == 2) {
						for (int i = 0; i < numrep; i++) {
							char content[4] = { 0, 0, 0, 0 };

							if (tok == "?") { curSec->write(content, 4); curLocCnt += 2; }

							else {

								if (nosymbols(tok))
								{
									if (!isnumber(tok))
									{
										symTab->add(new Symbol(tok, "SYM", 0, 0, false, 0, 0));
										relTab->add(new Relocation(curSec, curLocCnt, 'a', symTab->getSymbol(tok)->index));
										curSec->write(content, 4);
										curLocCnt += 2;
									}

									if (isnumber(tok))
									{

										int value = extractValue(tok);

										int i = 3;
										while (value != 0) {
											int bajt = value % 16;
											content[i] = bajt;
											i = i - 1;
											value = value / 16;
										}
										lilendianb(content);

										curSec->write(content, 4);
										curLocCnt += 2;
									}
								}

								else {
									int vrednost = extractValue(tok);
									Symbol* s = findSymbolOutsideSection(tok);


									if (s == NULL) {
										Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', curSec->simb);
										relTab->add(relokacija);
										curLocCnt += 2;

										int i = 3;
										while (vrednost != 0) {
											int bajt = vrednost % 16;
											content[i] = bajt;
											i = i - 1;
											vrednost = vrednost / 16;
										}
										lilendianb(content);

									}

									else if (s->local == false) {

										Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', s->index);
										relTab->add(relokacija);
										curLocCnt += 2;

									}

									else if (s->local == true) {

										if (s->section == curSec) {
											Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', curSec->simb);
											relTab->add(relokacija);
											curLocCnt += 2;

											int i = 2;
											while (vrednost != 0) {
												int bajt = vrednost % 16;
												content[i] = bajt;
												i = i - 1;
												vrednost = vrednost / 16;
											}
											lilendianb(content);

										}

										if (s->section != curSec) {
											Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', s->section->simb);
											relTab->add(relokacija);
											curLocCnt += 2;

											int i = 3;
											while (vrednost != 0) {
												int bajt = vrednost % 16;
												content[i] = bajt;
												i = i - 1;
												vrednost = vrednost / 16;
											}
											lilendianb(content);

										}



									}
									curSec->write(content, 4);
								}


							}




						}
					}
				}
			}


			








			getline(in, line);
			int linuxfix = line.find_first_of("\r",0);
			line=line.substr(0,linuxfix);

		}
	

		if (pass == 1) {
			if (curSec)
				symTab->getSymbol(curSec->simb)->size = curLocCnt;
			curSec = secTab->getFirst();
			curLocCnt = 0;
			in.clear();
			in.seekg(0, ios::beg);
		}
		
	}
	





void Assembler::operationfix1(char operationcode[], char operationcode2[], string operandi) {
	int dir = 0, ind = 0, mem = 1, pom = 0;

	if (!(operandi.find("R0") == string::npos) || !(operandi.find("R1") == string::npos)
		|| !(operandi.find("R2") == string::npos) || !(operandi.find("R3") == string::npos)
		|| !(operandi.find("R4") == string::npos) || !(operandi.find("R5") == string::npos)
		|| !(operandi.find("R6") == string::npos) || !(operandi.find("R7") == string::npos)
		|| !(operandi.find("R8") == string::npos) || !(operandi.find("R9") == string::npos)
		|| !(operandi.find("R10") == string::npos) || !(operandi.find("R11") == string::npos)
		|| !(operandi.find("R12") == string::npos) || !(operandi.find("R13") == string::npos)
		|| !(operandi.find("R14") == string::npos) || !(operandi.find("R15") == string::npos)
		|| !(operandi.find("PC") == string::npos) || !(operandi.find("SP") == string::npos)) {
		dir = 1; mem = 0;
	}

	if (!(operandi.find("[") == string::npos) && !(operandi.find("]") == string::npos))
	{
		ind = 1; dir = 0; mem = 0;
		if (!(operandi.find("+") == string::npos)) pom = 1;
	}
	int pcrel = 0;
	if (!(operandi.find("$") == string::npos)) {
		ind = 0; dir = 0; mem = 0; pcrel = 1; pom = 1;
	}

	if ((dir == 1) && (ind == 0) && (mem == 0) && (pom == 0)) { //registarsko direktno
		operandi[2] = 0;
		int start = operandi.find_first_of("R", 0);
		string regIdent = operandi.substr(start + 1, operandi.length());
		if (regIdent == "10") operationcode[3] = 0xA;
		else if (regIdent == "11") operationcode[3] = 0xB;
		else if (regIdent == "12") operationcode[3] = 0xC;
		else if (regIdent == "13") operationcode[3] = 0xD;
		else if (regIdent == "14") operationcode[3] = 0xE;
		else if (regIdent == "15") operationcode[3] = 0xF;
		else if (regIdent == "PC") { operationcode[2] = 1; operationcode[3] = 1; }
		else if (regIdent == "SP") { operationcode[2] = 1; operationcode[3] = 0; }
		else operationcode[3] = stoi(regIdent);
		operandi = operandi.substr(operandi.find_first_of(",", 0) + 1, operandi.length());
	//	if (operandi != "") mem = 1;
	}

	if ((ind == 1) && (pom == 0)) { //registarsko indirektno
	
		
		operationcode[2] = 4;
		int start = operandi.find_first_of("R", 0);
		int end = operandi.find_first_of("]", 0);
		string regIdent = operandi.substr(start + 1, end - 2);
		if (regIdent == "10") operationcode[3] = 0xA;
		if (regIdent == "11") operationcode[3] = 0xB;
		if (regIdent == "12") operationcode[3] = 0xC;
		if (regIdent == "13") operationcode[3] = 0xD;
		if (regIdent == "14") operationcode[3] = 0xE;
		if (regIdent == "15") operationcode[3] = 0xF;
		else operationcode[3] = stoi(regIdent);

		
	}

	if ((mem == 1)) {  //memorijsko direktno
		operationcode[2] = 14;
	   
		
			Symbol* s = findSymbolOutsideSection(operandi);

			if (s == NULL) {

				int adresa = extractValue(operandi);
				if (adresa == 0) zeroSemaphore = 1;
				int k = 7;
				while (adresa != 0) {
					int bajt = adresa % 16;
					operationcode2[k] = bajt;
					k = k - 1;
					adresa = adresa / 16;

				}


			}


			if (s != NULL) {
				if (s->local == false) {
					Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', s->index);
					relTab->add(relokacija);
					operationcode2[0] = 15; operationcode2[1] = 15; operationcode2[2] = 15; operationcode2[3] = 15;
					operationcode2[4] = 15; operationcode2[5] = 15; operationcode2[6] = 15; operationcode2[7] = 12;
					//zeroSemaphore = 1;
				}
				if (s->local == true) {
					Relocation* relokacija = new Relocation(curSec, curLocCnt, 'a', s->section->simb);
					relTab->add(relokacija);
					int adresa = extractValue(operandi) - 4;
					if (adresa == 0) zeroSemaphore = 1;
					int k = 7;
					while (adresa != 0) {
						int bajt = adresa % 16;
						operationcode2[k] = bajt;
						k = k - 1;
						adresa = adresa / 16;

					}




				}
				lilendian(operationcode2);

			}

	}


	

	if ((pom == 1)) {
		if (pcrel == 1) {
			operandi = operandi.substr(1, operandi.length());
			operationcode[2] = 0xF;
			operationcode[3] = 1;
			if (isnumber(operandi))
			{
				int pom = extractValue(operandi); int ind = 7;
				pom = pom - curLocCnt;
				if (pom == 0) zeroSemaphore = 1;
				while (pom != 0) {
					int cifra = pom % 16;
					pom = pom / 16;
					operationcode2[ind] = cifra;
					ind--;
				}
			}

			else if (!isnumber(operandi)) {
				Symbol* s = findSymbolOutsideSection(operandi);

				if (s == NULL) {

					int adresa = extractValue(operandi);
					adresa = adresa - curLocCnt-4;
					if (adresa == 0) zeroSemaphore = 1;
					int k = 7;
					while (adresa != 0) {
						int bajt = adresa % 16;
						operationcode2[k] = bajt;
						k = k - 1;
						adresa = adresa / 16;

					}


				}


				if (s != NULL) {
					if (s->local == false) {
						Relocation* relokacija = new Relocation(curSec, curLocCnt, 'r', s->index);
						relTab->add(relokacija);
						operationcode2[0] = 15; operationcode2[1] = 15; operationcode2[2] = 15; operationcode2[3] = 15;
						operationcode2[4] = 15; operationcode2[5] = 15; operationcode2[6] = 15; operationcode2[7] = 12;
						
					}
					if (s->local == true) {
						Relocation* relokacija = new Relocation(curSec, curLocCnt, 'r', s->section->simb);
						relTab->add(relokacija);
						int adresa = extractValue(operandi) - 4;
						if (adresa == 0) zeroSemaphore = 1;
						int k = 7;
						while (adresa != 0) {
							int bajt = adresa % 16;
							operationcode2[k] = bajt;
							k = k - 1;
							adresa = adresa / 16;

						}




					}
					

				}


			}

			lilendian(operationcode2);


		}
		else {
			operationcode[2] = 0xE;
			int start = operandi.find_first_of("[", 0);
			int end = operandi.find_first_of("+", 0);
			string regIdent = operandi.substr(start + 1, end - 1);
			if (regIdent == "R10") operationcode[3] = 0xA;
			else if (regIdent == "R11") operationcode[3] = 0xB;
			else if (regIdent == "R12") operationcode[3] = 0xC;
			else if (regIdent == "R13") operationcode[3] = 0xD;
			else if (regIdent == "R14") operationcode[3] = 0xE;
			else if (regIdent == "R15") operationcode[3] = 0xF;
			else if (regIdent == "PC") { operationcode[2] = 0xF; operationcode[3] = 1; }
			else if (regIdent == "SP") { operationcode[2] = 0xF; operationcode[3] = 0; }
			else operationcode[3] = stoi(regIdent.substr(1,regIdent.length()));
			operandi = operandi.substr(end + 1, operandi.length() - 1);
			end = operandi.find_first_of("]", 0);
			operandi = operandi.substr(0, end); //nije hteo da skine ]


			int adresa = extractValue(operandi);

			int i = 1, k = 7;
			while (adresa != 0) {
				int bajt = adresa % 16;
				operationcode2[k] = bajt;
				k = k - 1;
				adresa = adresa / 16;
			}
			lilendian(operationcode2);
		}


	}




}





void Assembler::operationfix2(char operationcode[], char operationcode2[], string operandi)  {
		
		int start = operandi.find_first_of("R", 0);
		int end = operandi.find_first_of(",", 0);
		string regIdent = operandi.substr(0, end);
		if (regIdent == "R10") operationcode[3] = 0xA;
		else if (regIdent == "R11") operationcode[3] = 0xB;
		else if (regIdent == "R12") operationcode[3] = 0xC;
		else if (regIdent == "R13") operationcode[3] = 0xD;
		else if (regIdent == "R14") operationcode[3] = 0xE;
		else if (regIdent == "R15") operationcode[3] = 0xF;
		else if (regIdent == "PC") { operationcode[3] = 1; }
		else if (regIdent == "SP") { operationcode[3] = 0; }
		else if (stoi(regIdent.substr(1, regIdent.length())) < 10) {
			regIdent = regIdent.substr(1, regIdent.length());
			operationcode[3] = stoi(regIdent);
		}
		operandi = operandi.substr(operandi.find_first_of(",", 0)+2, operandi.length());

	int dir = 0, ind = 0, mem = 1, pom = 0, imm = 0;

	if (!(operandi.find("R0") == string::npos) || !(operandi.find("R1") == string::npos)
		|| !(operandi.find("R2") == string::npos) || !(operandi.find("R3") == string::npos)
		|| !(operandi.find("R4") == string::npos) || !(operandi.find("R5") == string::npos)
		|| !(operandi.find("R6") == string::npos) || !(operandi.find("R7") == string::npos)
		|| !(operandi.find("R8") == string::npos) || !(operandi.find("R9") == string::npos)
		|| !(operandi.find("R10") == string::npos) || !(operandi.find("R11") == string::npos)
		|| !(operandi.find("R12") == string::npos) || !(operandi.find("R13") == string::npos)
		|| !(operandi.find("R14") == string::npos) || !(operandi.find("R15") == string::npos)
		|| !(operandi.find("PC") == string::npos) || !(operandi.find("SP") == string::npos)) {
		dir = 1; mem = 0;
	}

	if (!(operandi.find("[") == string::npos) && !(operandi.find("]") == string::npos))
	{
		ind = 1; dir = 0; mem = 0; 
		if (!(operandi.find("+") == string::npos)) pom = 1;
	}

	if (!(operandi.find("$") == string::npos)) { 
		dir = 0; pom = 1; ind = 0; mem = 0; }
	if (!(operandi.find("#") == string::npos)) { 
		ind = 0; dir = 0; mem = 0; pom = 0; imm = 1; }

	if ((dir == 1) && (ind == 0) && (mem == 0) && (pom == 0)) { //registarsko direktno
		operandi[2] = 0;
		regIdent = operandi;
		
		if (regIdent == "R10") { operationcode[4] = 0xA / 2; operationcode[5] = (0xA % 2) * 8; }
		if (regIdent == "R11") { operationcode[4] = 0xB / 2;  operationcode[5] = (0xB % 2) * 8; }
		if (regIdent == "R12") { operationcode[4] = 0xC / 2;  operationcode[5] = (0xC % 2) * 8; }
		if (regIdent == "R13") { operationcode[4] = 0xD / 2;  operationcode[5] = (0xD % 2) * 8; }
		if (regIdent == "R14") { operationcode[4] = 0xE / 2;  operationcode[5] = (0xE % 2) * 8; }
		if (regIdent == "R15") { operationcode[4] = 0xF / 2;  operationcode[5] = (0xF % 2) * 8; }
		else {
			regIdent = regIdent.substr(1, regIdent.length());
			operationcode[4] = stoi(regIdent) / 2; operationcode[5] = (stoi(regIdent) % 2) * 8;
		}
		
		
		
		
	}

	if ((ind == 1) && (pom == 0)) { //registarsko indirektno
	
		operationcode[2] = 4;
		int start = operandi.find_first_of("R", 0);
		int end = operandi.find_first_of("]", 0);
		string regIdent = operandi.substr(start + 1, end - 2);
		if (regIdent == "10") { operationcode[4] = 0xA / 2; operationcode[5] = (0xA % 2) * 8; }
		if (regIdent == "11") { operationcode[4] = 0xB / 2;  operationcode[5] = (0xB % 2) * 8; }
		if (regIdent == "12") { operationcode[4] = 0xC / 2;  operationcode[5] = (0xC % 2) * 8; }
		if (regIdent == "13") { operationcode[4] = 0xD / 2;  operationcode[5] = (0xD % 2) * 8; }
		if (regIdent == "14") { operationcode[4] = 0xE / 2;  operationcode[5] = (0xE % 2) * 8; }
		if (regIdent == "15") { operationcode[4] = 0xF / 2;  operationcode[5] = (0xF % 2) * 8; }
		else {
			operationcode[4] = stoi(regIdent) / 2; operationcode[5] = (stoi(regIdent) % 2) * 8;
		}

		}
	

	if ((mem == 1)&&(ind ==0)) {  //memorijsko direktno
		operationcode[2] = 14;
		relSemaphore = 1;

		if (!isnumber(operandi) && symTab->getSymbol(operandi) == NULL) {
			Symbol* s = findSymbolOutsideSection(operandi);
			if (s->section == 0) { 
				relTab->add(new Relocation(s->section, curLocCnt, 'a', s->index)); }
		}

		int adresa = extractValue(operandi);
		if (adresa == 0) zeroSemaphore = 1;
		int i = 1, k = 7;
		while (adresa != 0) {
			int bajt = adresa % 16;
			operationcode2[k] = bajt;
			k = k - 1;
			adresa = adresa / 16;
		}
		lilendian(operationcode2);

		if ((pom == 1)) {
			
			operationcode[2] = 0xE;
			int start = operandi.find_first_of("[", 0);
			int end = operandi.find_first_of("+", 0);
			string regIdent = operandi.substr(start + 1, end - 2);
			if (regIdent == "R10") operationcode[3] = 0xA;
			if (regIdent == "R11") operationcode[3] = 0xB;
			if (regIdent == "R12") operationcode[3] = 0xC;
			if (regIdent == "R13") operationcode[3] = 0xD;
			if (regIdent == "R14") operationcode[3] = 0xE;
			if (regIdent == "R15") operationcode[3] = 0xF;
			if (regIdent == "SP") { operationcode[2] = 0xF; operationcode[3] = 0; }
			if (regIdent == "PC") { operationcode[2] = 0xF; operationcode[3] = 1; }
			else operationcode[3] = stoi(regIdent.substr(start+1,end));

		}

	}


	

	if ((pom == 1)) {
		if (!(operandi.find("$") == string::npos)) {
			operationcode[2] = 0xE;

			operationcode[4] = 8;
			operationcode[5] = 8;

			start = operandi.find_first_of("$", 0);
			operandi = operandi.substr(start + 1, operandi.length());
			Symbol* s = findSymbolOutsideSection(operandi);

			if (isnumber(operandi))
			{
				int pom = extractValue(operandi); int ind = 7;
				pom = pom - curLocCnt;
				if (pom == 0) zeroSemaphore = 1;
				while (pom != 0) {
					int cifra = pom % 16;
					pom = pom / 16;
					operationcode2[ind] = cifra;
					ind--;
				}
				
			}

			else if (!isnumber(operandi)) {
				Symbol* s = findSymbolOutsideSection(operandi);

				if (s == NULL) {
					int adresa = extractValue(operandi);
					adresa = adresa - curLocCnt; // -4;
					if (adresa == 0) zeroSemaphore = 1;
					int k = 7;
					while (adresa != 0) {
						int bajt = adresa % 16;
						operationcode2[k] = bajt;
						k = k - 1;
						adresa = adresa / 16;

					}


				}


				if (s != NULL) {
					if (s->local == false) {
						Relocation* relokacija = new Relocation(curSec, curLocCnt, 'r', s->index);
						relTab->add(relokacija);
						operationcode2[0] = 15; operationcode2[1] = 15; operationcode2[2] = 15; operationcode2[3] = 15;
						operationcode2[4] = 15; operationcode2[5] = 15; operationcode2[6] = 15; operationcode2[7] = 12;

					}
					if (s->local == true) {
						Relocation* relokacija = new Relocation(curSec, curLocCnt, 'r', s->section->simb);
						relTab->add(relokacija);
						int adresa = extractValue(operandi) - 4;
						if (adresa == 0) zeroSemaphore = 1;
						int k = 7;
						while (adresa != 0) {
							int bajt = adresa % 16;
							operationcode2[k] = bajt;
							k = k - 1;
							adresa = adresa / 16;

						}




					}


				}


			}

		
			lilendian(operationcode2);

		}

		else {
			operationcode[2] = 0xE;
			int start = operandi.find_first_of("[", 0);
			int end = operandi.find_first_of("+", 0);
			string regIdent = operandi.substr(start + 1, end - 1);
			if (regIdent == "R10") { operationcode[4] = 0xA / 2; operationcode[5] = (0xA % 2) * 8; }
			else if (regIdent == "R11") { operationcode[4] = 0xB / 2;  operationcode[5] = (0xB % 2) * 8; }
			else if (regIdent == "R12") { operationcode[4] = 0xC / 2;  operationcode[5] = (0xC % 2) * 8; }
			else if (regIdent == "R13") { operationcode[4] = 0xD / 2;  operationcode[5] = (0xD % 2) * 8; }
			else if (regIdent == "R14") { operationcode[4] = 0xE / 2;  operationcode[5] = (0xE % 2) * 8; }
			else if (regIdent == "R15") { operationcode[4] = 0xF / 2;  operationcode[5] = (0xF % 2) * 8; }
			else if (regIdent == "PC") {  operationcode[4] = 8;  operationcode[5] = (0x1 % 2) * 8; }
			else if (regIdent == "SP") { operationcode[4] = 8;  operationcode[5] = 0; }
			else {
				operationcode[4] = stoi(regIdent.substr(1,regIdent.length())) / 2; operationcode[5] = (stoi(regIdent.substr(1, regIdent.length())) % 2) * 8;
			}
			operandi = operandi.substr(end + 1, operandi.length() - 1);
			end = operandi.find_first_of("]", 0);
			operandi = operandi.substr(0, end); //nije hteo da skine ]


			int adresa = extractValue(operandi);
			if (adresa == 0) zeroSemaphore = 1;
			int i = 1, k = 7;
			while (adresa != 0) {
				int bajt = adresa % 16;
				operationcode2[k] = bajt;
				k = k - 1;
				adresa = adresa / 16;
			}
			lilendian(operationcode2);


		}


	}


	if (imm == 1) {
		if (regIdent == "S" || regIdent == "P") {
			operationcode[2] = 9;
			if (regIdent == "P") operationcode[3] = 1;
	
		}
		else
		{
			operationcode[2] = 8;
		}
		int start =operandi.find_first_of("#", 0);
		string pomeraj = operandi.substr(start + 1, operandi.length());
		if (!isnumber(pomeraj)) relSemaphore = 1;
		int immValue = extractValue(pomeraj);
		if (immValue == 0) zeroSemaphore = 1;
		int i = 7;
		while (immValue != 0) {
			int bajt = immValue % 16;
			operationcode2[i] = bajt;
			i = i - 1;
			immValue = immValue / 16;
		}
		lilendian(operationcode2);

	}


}



int Assembler::extractValue(string operandi) {
	int value = 0;
	while (operandi != "") {

		if (operandi.find("/") != string::npos) {
			int pos = operandi.find_first_of("/");
			string op1 = operandi.substr(0, pos );
			string op2 = operandi.substr(pos + 1, operandi.length());
			operandi = "";
			int a = extractValue(op1);
			int b = extractValue(op2);
			value += a / b;
		}

		if (operandi.find("*") != string::npos) {
			int pos = operandi.find_first_of("*");
			string op1 = operandi.substr(0, pos);
			string op2 = operandi.substr(pos + 1, operandi.length());
			operandi = "";
			int a = extractValue(op1);
			int b = extractValue(op2);
			value += a * b;
		}






		if (operandi.find_first_of("?") == 0) {
			value = 0;
			operandi = "";
		}

		while (operandi.find_first_of(")") == 0) {
			operandi = operandi.substr(1, operandi.length());
		}

		int multiplier = 1; string ident = ""; int i;
		if (operandi.find_first_of("-") == 0) 
		{ multiplier = -1; operandi = operandi.substr(1, operandi.length()); }

		if (operandi.find_first_of("+") == 0)
		{
			multiplier = 1; operandi = operandi.substr(1, operandi.length());
		}

		int zagrada = operandi.find_first_of("(");
		int plus = operandi.find_first_of("+");
		int minus = operandi.find_first_of("-");
	

		if (operandi.find_first_of("(") == 0)
		{
			string rekOp = operandi.substr(1, operandi.length());
			int otvaranje = rekOp.find_first_of("(");
			int zatvaranje = rekOp.find_first_of(")");
			if ((zatvaranje < otvaranje) || (otvaranje == -1)) {
				rekOp = rekOp.substr(0, zatvaranje );
				int valuerek = extractValue(rekOp);
				value += valuerek*multiplier;
				operandi = operandi.substr(zatvaranje + 1, operandi.length());
			}
			else if ((zatvaranje > otvaranje) && (otvaranje>-1)) {
				int otvorene = 1; int zatvorene = 0; int iterator = 1; string iteratorstring = operandi.substr(1,operandi.length());
				while (otvorene != zatvorene) {
					string tok = iteratorstring.substr(0, 1);
					if (tok == "(") otvorene++;
					if (tok == ")") zatvorene++;
					iteratorstring = iteratorstring.substr(1, iteratorstring.length());
					iterator++;

				}
				string rekOp = operandi.substr(1, iterator-2);
				int valuerek = extractValue(rekOp);
				value += valuerek*multiplier;
				operandi = operandi.substr(iterator, operandi.length());
				
			}

		}

		


		else if (((plus < zagrada) || (minus < zagrada) || (zagrada == -1) ) && (operandi != "?")) {
			if (plus == -1) plus = 9999; // :)
			if (minus == -1) minus = 9999; // :)

			
			
			if (plus < minus && operandi != "") { ident = operandi.substr(0, plus); operandi = operandi.substr(plus + 1, operandi.length()); }
			if (minus < plus && operandi != "") { ident = operandi.substr(0, minus); operandi = operandi.substr(minus, operandi.length()); }
			if (minus == plus & operandi != "") { ident = operandi; operandi = ""; }

			if ((ident.substr(0, 1) == "0") || (ident.substr(0, 1) == "1") || (ident.substr(0, 1) == "2") || (ident.substr(0, 1) == "3") || (ident.substr(0, 1) == "4") ||
				(ident.substr(0, 1) == "5") || (ident.substr(0, 1) == "6") || (ident.substr(0, 1) == "7") || (ident.substr(0, 1) == "8") || (ident.substr(0, 1) == "9") ||
				(ident.substr(0, 2) == "0x") || (ident.substr(0, 2) == "0b")) 
			{
				if (ident.substr(0, 2) == "0x")
				{

					ident = ident.substr(2, ident.length());
					i = ident.length() - 1;
					while (ident != "")
					{
						string cifra = ident.substr(0, 1);
						ident = ident.substr(1, ident.length());
						if (cifra == "A") { value += 10 * pow(16, i)*multiplier; i = i - 1; }
						else if (cifra == "B") { value += 11 * pow(16, i)*multiplier; i = i - 1; }
						else if (cifra == "C") { value += 12 * pow(16, i)*multiplier; i = i - 1; }
						else if (cifra == "D") { value += 13 * pow(16, i)*multiplier; i = i - 1; }
						else if (cifra == "E") { value += 14 * pow(16, i)*multiplier; i = i - 1; }
						else if (cifra == "F") { value += 15 * pow(16, i)*multiplier; i = i - 1; }
						else if (cifra == "0") { value += 0 * pow(2, i)*multiplier; i = i - 1; }
						else {

							value += stoi(cifra) * pow(16, i)*multiplier; i = i - 1;
						}

					}
				}


				else if (ident.substr(0, 2) == "0b")
				{

					ident = ident.substr(2, ident.length());
					i = ident.length() - 1;
					while (ident != "")
					{
						string cifra = ident.substr(0, 1);
						ident = ident.substr(1, ident.length());
						if (cifra == "0") { value += 0 * pow(2, i)*multiplier; i = i - 1; }
						else if (cifra == "1") { value += 1 * pow(2, i)*multiplier; i = i - 1; }

					}
				}



				else { value += stoi(ident)*multiplier; }
			}

			else if (symTab->getSymbol(ident) != NULL)

			{

				value += symTab->getSymbol(ident)->value*multiplier;
				if (relSemaphore == 1) {
					Symbol* s = symTab->getSymbol(ident);

					if (s->local == false) {

						Relocation* relokacija = new Relocation(curSec, curLocCnt+4, 'a', s->index);
						relTab->add(relokacija);
						globalSymSemaphore = 1;

					}

					else if (s->local == true) {

						if (s->section == curSec) {
							Relocation* relokacija = new Relocation(curSec, curLocCnt + 4, 'a', curSec->simb);
							relTab->add(relokacija);

							//	lilendian(content, vrednost);
						}

						if (s->section != curSec) {
							Relocation* relokacija = new Relocation(curSec, curLocCnt + 4, 'a', s->section->simb);
							relTab->add(relokacija);

							//	lilendian(content, vrednost);

						}

					}

				}
				relSemaphore = 0;
			

			}
		}
		
	}
	return value;
}




Symbol * Assembler::findSymbolOutsideSection(string s) {

	Symbol* simb = NULL;
	string teksimb;
	int saddodat = 0;

	while (s != "") {

		int plus = s.find_first_of("+");
		int minus = s.find_first_of("-");
		int kraj = s.length();

		if (plus == -1) plus = 9999;
		if (minus == -1) minus = 9999;

		if ((plus < minus) && (plus < kraj)) {
			teksimb = s.substr(0, plus);  s = s.substr(plus + 1, kraj);
		}
		else if ((minus < plus) && (minus < kraj)) {
			teksimb = s.substr(0, minus); s = s.substr(minus + 1, kraj);
		}

		else {
			teksimb = s.substr(0, kraj); s = s.substr(kraj, s.length());
		}

		if (teksimb != "") {
			if (!isnumber(teksimb)) {
				Symbol* dohvacen = symTab->getSymbol(teksimb);
				if (dohvacen == NULL) {
					symTab->add(new Symbol(teksimb, "SYM", 0, 0, false, 0, false));
					simb = symTab->getSymbol(teksimb);
					dohvacen = simb;
					saddodat = 1;
				}

				if ((dohvacen->section != curSec) && (saddodat == 0))  {
					if (simb == NULL) simb = dohvacen;
					else {
						cout << "GRESKA! Neregularan izraz u sekciji='" << symTab->getSymbol(curSec->simb)->nazivsimb << "'" << endl; break;
						
					}
				}
				saddodat = 0;
			}
		}
		


	}
	return simb;
}

bool Assembler::nosymbols(string s) {
	  
	string teksimb;
	bool flag = true;
	while (s != "") {

		int plus = s.find_first_of("+");
		int minus = s.find_first_of("-");
		int kraj = s.length();

		if (plus == -1) plus = 9999;
		if (minus == -1) minus = 9999;

		if ((plus < minus) && (plus < kraj)) {
			teksimb = s.substr(0, plus);  s = s.substr(plus + 1, kraj);
		}
		else if ((minus < plus) && (minus < kraj)) {
			teksimb = s.substr(0, minus); s = s.substr(minus + 1, kraj);
		}

		else {
			teksimb = s.substr(0, kraj); s = s.substr(kraj, s.length());
		}

		if (teksimb != "") {
			if (symTab->getSymbol(teksimb) != NULL) flag = false;
		}
	}
		return flag;
		
}

void Assembler::lilendian(char* content) {
	char swap[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	swap[0] = content[6];
	swap[1] = content[7];
	swap[2] = content[4];
	swap[3] = content[5];
	swap[4] = content[2];
	swap[5] = content[3];
	swap[6] = content[0];
	swap[7] = content[1];

	for (int i = 0; i < 8; i++) content[i] = swap[i];
	

	



}

void Assembler::lilendian4b(char* content) {
	
	char swap[16] = { 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0 };

	swap[0] = content[14];
	swap[1] = content[15];
	swap[2] = content[12];
	swap[3] = content[13];
	swap[4] = content[10];
	swap[5] = content[11];
	swap[6] = content[8];
	swap[7] = content[9];

	swap[8] = content[6];
	swap[9] = content[7];
	swap[10] = content[4];
	swap[11] = content[5];
	swap[12] = content[2];
	swap[13] = content[3];
	swap[14] = content[0];
	swap[15] = content[1];

	for (int i = 0; i < 16; i++) content[i] = swap[i];

}

void Assembler::lilendianb(char* content) {
	char swap[4] = { 0, 0, 0, 0 };

	swap[0] = content[2];
	swap[1] = content[3];
	swap[2] = content[0];
	swap[3] = content[1];
	
	for (int i = 0; i < 4; i++) content[i] = swap[i];

}





bool Assembler::isnumber(string s) {
	bool flag = true;

	while (s != "") {
		

		string tok = s.substr(0,1);
		string tok2 = s.substr(0, 2);
		if ((tok != "0") && (tok != "1") && (tok != "2") && (tok != "3") && (tok != "4") && (tok != "5") && (tok != "6") && (tok != "7") && (tok != "8") && (tok != "9")
			&& (tok2 != "0x") && (tok2 != "0b") && (tok != "+") && (tok != "-") && (tok != ")") && (tok != "(") && (tok != "A") && (tok != "B") && (tok != "C") && (tok != "D") && (tok != "E") && (tok != "F")) {
			flag = false;
		}
		if ((tok2 == "0x") || (tok2 == "0b")) { s = s.substr(2, s.length()); }
		else s = s.substr(1, s.length());

	}
	return flag;

}


