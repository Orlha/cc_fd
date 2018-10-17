#include "decoder.h"

#define MAKESWITCH( f ) switch_map[f] = &Decoder::op##f

Decoder::Decoder(unsigned char * script_data, int * offset) {
	script = script_data;
	position = offset;
    script_begin = * position;
    output = false;
    ret_code = 0;
    lib = dlopen("./libdic.so", RTLD_NOW);
    if (lib == NULL) {
        cerr << dlerror() << endl;
        exit(-1);
    }
    typedef void (*get)(unsigned char * script_data, int * offset);
    get dinit = (get)dlsym(lib, "init");
    dinit(script, position);
    initMap();
}

Decoder::~Decoder() {
    dlclose(lib);
}

void Decoder::initMap() {
    for (int index = 0; index <= 0xE9; ++index) {
        if (index == 0x52) continue;
        if (index == 0x53) continue;
        if (index == 0x57) continue;
        if (index == 0x58) continue;
        if (index == 0xDE) continue;
        //if (index == 0xE8) continue;
        char funcName[10];
        sprintf(funcName, "op0x%02X", index);
        switch_map[index] = (intFunc)dlsym(lib, funcName);
    }

    for (int index = 0; index <= 0x85; ++ index) {
        if (index == 0x08) continue;
        if (index == 0x0B) continue;
        if (index == 0x16) continue;
        if (index == 0x1E) continue;
        if (index == 0x1F) continue;
        if (index == 0x20) continue;
        if (index == 0x24) continue;
        if (index == 0x26) continue;
        if (index == 0x27) continue;
        if (index == 0x28) continue;
        if (index == 0x39) continue;
        if (index == 0x4D) continue;
        if (index == 0x51) continue;
        if (index == 0x54) continue;
        if (index == 0x58) continue;
        if (index == 0x5F) continue;
        if (index == 0x6A) continue;
        if (index == 0x7F) continue;
        if (index == 0x83) continue;
        char funcName[10];
        sprintf(funcName, "op0xFE%02X", index);
        fe_map[index] = (intFunc)dlsym(lib, funcName);
    }
    getBuffer = (charFunc)dlsym(lib, "getBuffer");
    buffer = getBuffer();
    getReturn = (intFunc)dlsym(lib, "getReturn");
    getJmp = (vecFunc)dlsym(lib, "getJumps");
    getCll = (vecFunc)dlsym(lib, "getCalls");
    return;
}

vector<unsigned int> * Decoder::getJumps() {
    return getJmp();
}

vector<unsigned int> * Decoder::getCalls() {
    return getCll();
}

int Decoder::decode() {
#ifdef DCMPL_OUT
	printf("0x%04x: ", * position - script_begin);
#endif
	int code = script[* position];	
    int pre_pos = * position;
    int code_length = 0;
    ret_code = 0;

    if(code == 0xA3 || code == 0xC9) {
        printf("Found battle: code %02X\n", code);
        printf("Dump: ");
        for(int i = 0; i < 7; ++i) {
            printf("%02X ", script[(*position) + i]);
        }
        printf("\n");
        //script[(*position) + 6] |= 1;
        if(script[(*position) + 5] & 1) {
            printf("    Flag 0x01 set: Keep field music in battle;\n");
        }
        if(script[(*position) + 5] & 2) {
            printf("    Flag 0x02 set: Battle is losable;\n");
        }
        if(script[(*position) + 5] & 4) {
            printf("    Flag 0x04 set: Dragoon clothes after battle;\n");
        }
        if(script[(*position) + 5] & 8) {
            printf("    Flag 0x08 set: Unknown;\n");
        }
        if(script[(*position) + 5] & 0x10) {
            printf("    Flag 0x10 set: No victory music;\n");
        }
        if(script[(*position) + 5] & 0x20) {
            printf("    Flag 0x20 set: Keep battle music after battle;\n");
        }
        if(script[(*position) + 5] & 0x40) {
            printf("    Flag 0x40 set: Brink of Death music;\n");
        }
        if(script[(*position) + 5] & 0x80) {
            printf("    Flag 0x80 set: Unknown;\n");
        }
        /*
        printf("\n Fix: ");
        for(int i = 0; i < 7; ++i) {
            printf("%02X ", script[(*position) + i]);
        }
        printf("\n");
        */
    }
    //code = 0x11;
    auto search = switch_map.find(code);
    if (search != switch_map.end()) {
        code_length = search->second();
        ret_code = getReturn();
    } else {
        if (code == 0xFE) {
            (*position) ++;
            unsigned char subOpcode = script[* position];
            auto fe_search = fe_map.find(subOpcode);
            if (fe_search != fe_map.end()) {
                code_length = fe_search->second();
                ret_code = getReturn();
            } else {
                sprintf(buffer, "FATAL: Unknown opcode 0xFE%02X", subOpcode);
#ifdef DEBUG
                printf("FATAL: Unknown opcode 0xFE%02X", subOpcode);
#endif
                code_length = 1;
                ret_code = 4;
            }
        } else {
            sprintf(buffer, "FATAL: unknown opcode %02X", code);
#ifdef DEBUG
            printf("FATAL: unknown opcode %02X", code);
#endif
            code_length = 1;
            ret_code = 4;
        }
    }
    *position += code_length;
#ifdef DCMPL_OUT
    printf(buffer);
	printf("\n{");
	for (int i = 0; i < * position - pre_pos; i++) {
		printf("%02X ", script[pre_pos + i]);
	}
	printf("}\n");
	fflush(stdout);
#endif
	return ret_code;
}
