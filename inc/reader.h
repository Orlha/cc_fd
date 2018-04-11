#ifndef reader_h
#define reader_h

#include <cstdio>
#include <fstream>
#include <cstring>
#include <string>

#include "entity.h"
#include "decoder.h"

using namespace std;

class Reader {
    private:
		int length;
		unsigned char * script;
		int pos;
		int rel_pos;
        int entity_counter;
        
        int cur_ent;
        int cur_scr;
		
		Entity * entity;
        Decoder * decoder;
        vector<unsigned int> * jumps;
        vector<unsigned int> * calls;
        
        void checkEntity();
        //int atJump();
        void checkCall();
    public:
    	Reader();
    	~Reader();
    	int initFile(string filename);
    	
    	int readHeader();
    	int readEntCounter();
    	int readEntities();
    	
    	int initDecoder();
        int decompile();

        int getClosestJump();
        int getClosestEnt();
        int getClosestCall();
};
#endif