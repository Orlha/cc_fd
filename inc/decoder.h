#ifndef decoder_h
#define decoder_h

#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <vector>
#include <map>

#include "constants.h"

using namespace std;

class Decoder {
    private:
        typedef int(Decoder::*opFunc)();

    	int script_begin;
    	int * position;
    	unsigned char * script;
        char tempBuffer[20];
        vector<unsigned int> jumps;

        map<int, opFunc> switch_map;

        void initMap();

    public:
        Decoder(unsigned char * script_data, int * offset);
        
        vector<unsigned int> * getJumps();

        int op0x00();

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
