#ifndef _sections_h_
#define _sections_h_


class Section {
public:

	int simb;
	char* sadrzajsek;
	Section* next;
	int contentSize;
	char* flags;
	Section(int symbol);
	~Section();

	void write(char *data, int cnt);
};

#endif