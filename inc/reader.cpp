#include "reader.h"

Reader::Reader() {
	length = 0;
	script = NULL;
	pos = 0;
    rel_pos = 0;
    cur_ent = 0;
    cur_scr = 0;
	entity_counter = 0;
	entity = NULL;
}

Reader::~Reader() {
	delete decoder;
	delete[] script;
	delete[] entity;
}

void Reader::checkEntity() {
    for (int ent = 0; ent < entity_counter; ent++) {
        for (int scr = 0; scr < 32; scr++) {
            int value = entity[ent].script[scr];
            if (value == pos - rel_pos && (value != 0x0000 || scr == 0)) {
            // TODO: more clever check
                cur_ent = ent;
                cur_scr = scr;
#ifdef DCMPL_OUT
                printf("---------------------------\n");
                printf("| Entity %d script %d\n", cur_ent, cur_scr);
                printf("---------------------------\n");
#endif
            }
        }
    }
}
/*
int Reader::atJump() {
    for(vector<unsigned int>::iterator it = jumps->begin(); it != jumps->end(); ++it) {
        int value = * it;
        if (value == pos - rel_pos) {
            return value;
        }
    }
    return -1;
}
*/

void Reader::checkCall() {
    for(vector<unsigned int>::iterator it = calls->begin(); it != calls->end(); ++it) {
        int value = * it;
        if (value == pos - rel_pos) {
#ifdef DCMPL_OUT
            printf("---------------------------\n");
            printf("| Sub function 0x%04X\n", value);
            printf("---------------------------\n");
#endif
            return;
        }
    }
}

int Reader::getClosestEnt() {
    int cur_pos = pos - rel_pos;
    int res = 0;
    for (int ent = 0; ent < entity_counter; ent++) {
        for (int scr = 0; scr < 32; scr++) {
            int value = entity[ent].script[scr];
            if (value >= cur_pos && (value < res || res == 0)) {
                res = value;
            }
        }
    }
    return res;
}

int Reader::getClosestJump() {
    int cur_pos = pos - rel_pos;
    int res = 0;
    for(vector<unsigned int>::iterator it = jumps->begin(); it != jumps->end(); ++it) {
        int value = * it;
        if (value >= cur_pos && (value < res || res == 0)) {
            res = value;
        }
    }
    return res;
}

int Reader::getClosestCall() {
    int cur_pos = pos - rel_pos;
    int res = 0;
    for(vector<unsigned int>::iterator it = calls->begin(); it != calls->end(); ++it) {
        int value = * it;
        if (value >= cur_pos && (value < res || res == 0)) {
            res = value;
        }
    }
    return res;
}

int Reader::initFile(string filename) {
    printf("File %s;\n", filename.c_str());

    char subbuff[6];
    memcpy(subbuff, &filename.c_str()[7], 5);
    subbuff[5] = '\0';
    printf("%s\n", subbuff);

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

int Reader::writeFin(string filename) {
    ofstream outfile(filename.c_str(), ofstream::binary);
    outfile.write((const char *)script, length);
    outfile.close();
    return 0;
}

int Reader::readHeader() {
	// Read 32 words from header
	while (pos < 32 * 4)
    {
    	int value;
    	memcpy(&value, &script[pos], sizeof(value));
#ifdef DCMPL_OUT
    	printf("local[%02d] = ", pos / 4);
    	printf("0x%08X\n", value);
#endif
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
    jumps = decoder->getJumps();
    calls = decoder->getCalls();
	return 0;
}

int lowestNotNull(int a, int b) {
    if(a == 0 && b == 0) {
        return -1;
    }
    if(a == 0 && b != 0) {
        return b;
    }
    if(a != 0 && b == 0) {
        return a;
    }
    if(a < b) {
        return a;
    } else {
        return b;
    }
}

int Reader::decompile() {
	
	if (pos - rel_pos < entity[0].script[0]) {
#ifdef DCMPL_OUT
		printf("0x0000: Pre-entity data: \n{");
#endif
		while(pos - rel_pos < entity[0].script[0]) {	
#ifdef DCMPL_OUT
			printf("%02X ", script[pos]);
#endif
			pos++;
		}
#ifdef DCMPL_OUT
		printf("}\n");
#endif
	}
		
	while(pos < length) {
        //printf("pos/length = %d/%d\n", pos, length);
        
        checkEntity();
        checkCall();
        
        int result = decoder->decode();
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
			
			int block = 0;
			bool fin = false;
			
			while(block == 0) {
                int nearEnt = getClosestEnt();
                int nearJump = getClosestJump();
                int nearCall = getClosestCall();

                //printf("js %lX\n", jumps->size());
                /*
                if(pos == 7194) {
                    for(unsigned long i = 0; i < jumps->size(); ++i) {
                        printf("Jump %X\n", (*jumps)[i]);
                    }
                    printf("Jump %X\n", (*jumps)[jumps->size() - 1]);
                    printf("Call %X\n", (*calls)[calls->size() - 1]);
                }
                */

                int addr = lowestNotNull(nearJump, nearCall);
                //printf("ADDR: %X\n", addr);
                if (addr == -1) {
                    if (nearEnt == 0) {
                        fin = true;
					    block = 0;
					    break;
                    } else {
                        block = nearEnt;
                    }
                } else {
                    block = lowestNotNull(addr, nearEnt);
                }
			}
			if (block != 0) {
			    pos = block + rel_pos;
			}
			int code_length = pos - pre_pos;

            if(code_length > length - pos) code_length = length - pos;
			
			if (fin) {
				code_length = length - pre_pos;
				pos += code_length;
			}
#ifdef DCMPL_OUT
			if (code_length > 0) {
				printf("0x%04x: Yet another garbage", pre_pos - rel_pos);
				printf("\n{");
				for (int i = 0; i < code_length; i++) {
					printf("%02X ", script[pre_pos + i]);
				}
				printf("}\n");
			}
#endif
		}
	}
    //printf("pos fin %X\n", pos);
	return 0;
}
