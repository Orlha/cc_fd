#ifndef reader_h
#define reader_h

#include <fstream>
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
        vector<int> * jumps;
        
        bool atEntity();
        bool atJump();
    public:
    	Reader();
    	~Reader();
    	int initFile(string filename);
    	
    	int readHeader();
    	int readEntCounter();
    	int readEntities();
    	
    	int initDecoder();
        int decompile();
};
#endif
