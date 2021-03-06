#include "reader.h"

Reader::Reader() {
	length = 0;
	script = NULL;
	pos = 0;
	rel_pos = 0;
	entity_counter = 0;
	entity = NULL;
}

Reader::~Reader() {
	delete decoder;
	delete[] script;
	delete[] entity;
}

int Reader::initFile(string filename) {
	ifstream file(filename.c_str(), ifstream::binary);
	if (file) {
		// get length of file:
    	file.seekg (0, file.end);
    	length = file.tellg();
    	file.seekg (0, file.beg);
    	
    	// allocate memory:
    	script = new unsigned char[length];
    	
    	// read data as a block:
    	file.read ((char *)script,length);
    	
    	file.close();
	} else {
		printf("No access to file;");
		return 11;
	}
	if (length == 0 || script == NULL) {
		printf("Invalid file;");
		return 11;
	}
	return 0;
}

int Reader::readHeader() {
	// Read 32 words from header
	while (pos < 32 * 4)
    {
    	int value;
    	memcpy(&value, &script[pos], sizeof(value));
    	printf("local[%02d] = ", pos / 4);
    	printf("0x%08X\n", value);
    	pos += 4;
	}
	return 0;
}

int Reader::readEntCounter() {
	memcpy(&entity_counter, &script[pos], sizeof(int));
	pos += 4;
	return 0;
}

int Reader::readEntities() {
	entity = new Entity[entity_counter];
	for (int ent = 0; ent < entity_counter; ent++) {
		for (int scr = 0; scr < 32; scr++) {
			unsigned short value;
			memcpy(&value, &script[pos], sizeof(value));
			entity[ent].script[scr] = value;
			pos += 2;
		}
	}
	
	// position relative to 0x0000 of script, not whole file:
	rel_pos = pos;
	return 0;
}

int Reader::initDecoder() {
	decoder = new Decoder(script, &pos);
	return 0;
}

int Reader::decompile() {
	
	if (pos - rel_pos < entity[0].script[0]) {
		printf("0x0000: Pre-entity data: \n{");
		while(pos - rel_pos < entity[0].script[0]) {	
			printf("%02X ", script[pos]);
			pos++;
		}
		printf("}\n");
	}
	
	int cur_ent;
	int cur_scr;
	
	while(pos < length) {
		for (int ent = 0; ent < entity_counter; ent++) {
			for (int scr = 0; scr < 32; scr++) {
				int value = entity[ent].script[scr];
				if (value == pos - rel_pos && (value != 0x0000 || scr == 0)) {
					printf("---------------------------\n");
					printf("| Entity %d script %d\n", ent, scr);
					printf("---------------------------\n");
					cur_ent = ent;
					cur_scr = scr;
				}
			}
		}
		int result = decoder->decode();
		
		// succesfully decoded:
		if (result == 0) {
			continue;
		}
		
		// unknown opcode or parameter error:
		if (result == 4) {
			return result;
		}
		
		// returned from STOP { 00}:
		if (result == 1) {
			int pre_pos = pos;
			int next_ent = cur_ent;
			int next_scr = cur_scr;
			
			int block = 0;
			bool fin = false;
			
			while(block == 0) {
				if (next_scr == 31) {
					next_ent ++;
					next_scr = 0;
				} else {
					next_scr ++;
				}
				
				if (next_ent == entity_counter) {
					fin = true;
					block = 0;
					break;
				} else {
					block = entity[next_ent].script[next_scr];
				}
			}
			if (block != 0) {
				pos = block + rel_pos;	
			}
			int code_length = pos - pre_pos;
			
			if (fin) {
				code_length = length - pre_pos;
				pos += code_length;
			}
			
			if (code_length > 0) {
				printf("0x%04x: Yet another garbage", pre_pos - rel_pos);
				printf("\n{");
				for (int i = 0; i < code_length; i++) {
					printf("%02X ", script[pre_pos + i]);
				}
				printf("}\n");
			}
		}
	}
	return 0;
}
