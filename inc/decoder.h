#ifndef decoder_h
#define decoder_h

#include <cstdio>
#include <stdlib.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <dlfcn.h>
#include "constants.h"

using namespace std;

class Decoder {
    private:
        void * lib;

        typedef int(*intFunc)();
        typedef char*(*charFunc)();
        typedef vector<unsigned int>*(*vecFunc)();

        vecFunc getJmp;
        vecFunc getCll;

        intFunc getReturn;

        charFunc getBuffer;

        char * buffer;

    	int script_begin;
    	int * position;
    	unsigned char * script;

        map<int, intFunc> switch_map;
        map<int, intFunc> fe_map;

        int ret_code;

        bool output;

        void initMap();
    public:
        Decoder(unsigned char * script_data, int * offset);
        vector<unsigned int> * getJumps();
        vector<unsigned int> * getCalls();
        int decode();
};
#endif
