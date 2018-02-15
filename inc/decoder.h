#ifndef decoder_h
#define decoder_h

#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <vector>

using namespace std;

class Decoder {
    private:
    	int script_begin;
    	int * position;
    	unsigned char * script;
        char tempBuffer[20];
        vector<int> jumps;
    	

    public:
        Decoder(unsigned char * script_data, int * offset);
        
        vector<int> * getJumps();

    	int decode();
    	unsigned short int read16u(int idx);
    	signed short int read16s(int idx);
    	char * getValueOrVarU(int idx);
    	char * getValueOrVarS(int idx, int arg1, int arg2);
    	char * readCharacter(int offset);
    	char* getVarName(int varIdx);
    	bool isNumeric(const char* string);
};
#endif
