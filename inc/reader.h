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
		
		Entity * entity;
		Decoder * decoder;
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
