#ifndef dic_h
#define dic_h

#include <cstring>
#include <cstdio>
#include <vector>

using namespace std;

int ret_code = 0;
char descBuffer[100];
char tempBuffer[20];
vector<unsigned int> jumps;
vector<unsigned int> calls;

int * position;
unsigned char * script;

unsigned short int read16u(int idx);
signed short int read16s(int idx);
char * getVar16u(int idx);
char * getVar16s(int idx, int arg1, int arg2);
char * readCharacter(int offset);
char * getVarName(int varIdx);
bool isNumeric(const char* string);

extern "C" {
void init(unsigned char * script_data, int * offset);
char * getBuffer();

int op0x00();
int op0x01();
int op0x02();
int op0x03();
int op0x04();
int op0x05();
int op0x06();
int op0x07();
int op0x08();
int op0x09();
int op0x0A();
int op0x0B();
int op0x0C();
int op0x0D();
int op0x0E();
int op0x0F();
int op0x10();
int op0x11();
int op0x12();
int op0x13();
int op0x14();
int op0x15();
int op0x16();
int op0x17();
int op0x18();
int op0x19();
int op0x1A();
int op0x1B();
int op0x1C();
int op0x1D();
int op0x1E();
int op0x1F();
}
#endif