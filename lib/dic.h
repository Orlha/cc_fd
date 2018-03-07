#ifndef dic_h
#define dic_h

#include <cstdio>
#include <vector>
#include <cstring>

using namespace std;
int ret_code = 0;
char descBuffer[100];
char tempBuffer[20];
vector<unsigned int> jumps;

int * position;
unsigned char * script;

extern "C" void init(unsigned char * script_data, int * offset);

extern "C" char * getBuffer();

unsigned short int read16u(int idx);
signed short int read16s(int idx);
char * getVar16u(int idx);
char * getVar16s(int idx, int arg1, int arg2);
char * readCharacter(int offset);
char * getVarName(int varIdx);
bool isNumeric(const char* string);

extern "C" int op0x00();
extern "C" int op0x01();
extern "C" int op0x02();
extern "C" int op0x03();
extern "C" int op0x04();
extern "C" int op0x05();
extern "C" int op0x06();
extern "C" int op0x07();
extern "C" int op0x08();
extern "C" int op0x09();
extern "C" int op0x0A();
extern "C" int op0x0B();
extern "C" int op0x0C();
extern "C" int op0x0D();
extern "C" int op0x0E();
extern "C" int op0x0F();
extern "C" int op0x10();
extern "C" int op0x11();
extern "C" int op0x12();
extern "C" int op0x13();
extern "C" int op0x14();
extern "C" int op0x15();
extern "C" int op0x16();
extern "C" int op0x17();
extern "C" int op0x18();
extern "C" int op0x19();
extern "C" int op0x1A();
extern "C" int op0x1B();
extern "C" int op0x1C();
extern "C" int op0x1D();
extern "C" int op0x1E();
extern "C" int op0x1F();
#endif