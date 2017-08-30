# twoPassAssembler
Implementation of a two pass assembler I made as a school project.

Opis problema:
Potrebno je implementirati dvoprolazni asembler. Ulaz je tekstualni fajl u skladu sa sintaksom definisanom postavkom domaćeg zadatka. Izlaz je tekstualni fajl, koji predstavlja predmetni program koji je takođe definisan postavkom domaćeg zadatka, sličan ELF formatu. Projekat se radi pod operativnim sistemom Linux (Ubuntu 12.04 32bit). Programski jezici koji se mogu koristiti su C, C++, asembler, ili njihova kombinacija.

Rešenje problema: 
U prilozenom rešenju realizovane su tri strukture podataka: 

-Tabela simbola 
-Tabela sekcija
-Tabela relokacija

Ove strukture se koriste u klasi  “Assembler.cpp” koja simulira ponašanje dvoprolaznog asemblera, čije su karakteristike opisane u prilogu. “Prolaz” je implementiran kao funkcija koja kao argument prima broj prolaza, 1 ili 2.

Prvi argument koji se zadaje prilikom pokretanja programa predstavlja ulaznu tekstualnu datoteku, koja predstavlja asemblerski program koji je potrebno obraditi.

Cilj prvog prolaza je (delimicno) formiranje tabele simbola, utvrdjivanje pocetnih adresa sekcija i njihovih velicina, odnosno prikupljanje osnovnih informacija koje će se koristiti u ponovnom, drugom prolazu asemblera.
Cilj drugog prolaza je kompletiranje finalnog izgleda tabele simbola, određivanja sadržaja sekcija koje imaju sadržaj, kao i spremanje informacija za linker u vidu formiranja tabele relokacija. 

Klase koje su navedene na početku izlaganja rešenja problema sadrže funkcije koje vrše upis u tekstualnu datoteku, koja je navedena kao drugi argument prilikom pokretanja programa. Ispis tabele simbola, sekcija i relokacija je u skladu sa postavkom domaćeg zadatka. Pored priloženog rešenja, dostupna 3 test fajla, kao i njihovi odgovarajući izlazi.

Komande za pokretanje:
Iz terminala, potrebno je prebaciti se u direktorijum gde se nalaze fajlovi. U istom direktorijumu treba da budu cpp i h fajlovi kao i test primeri. Izlaz programa je output.bin fajl. Po potrebi, potrebno je unaprediti alate koji se koriste sledećim komandama:

sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt-get install gcc-5 g++-5
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5

kompajliranje:
g++ symboltable.cpp symbol.cpp sectiontable.cpp section.cpp relocationtable.cpp relocation.cpp assembler.cpp main.cpp -std=c++0x -g -o asm.out –fpermissive

pokretanje:
./asm.out ulaznadatoteka.txt izlaznadatoteka.txt
