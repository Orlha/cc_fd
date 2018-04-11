#include "dic.h"

extern "C" void init(unsigned char * script_data, int * offset) {
    script = script_data;
	position = offset;
    //printf("Library initializated;\n");
}

extern "C" char * getBuffer() {
    return descBuffer;
}

extern "C" int getReturn() {
    int ret = ret_code;
    ret_code = 0;
    return ret;
}

extern "C" vector<unsigned int> * getJumps() {
    return &jumps;
}

extern "C" vector<unsigned int> * getCalls() {
    return &calls;
}

unsigned short int read16u(int idx)
{
	return *(short unsigned int*)(script + * position + idx);
}

signed short int read16s(int idx)
{
	return *(short signed int*)(script + * position + idx);
}

bool isNumeric(const char* string)
{
	for(unsigned int i=0; i<strlen(string); i++)
	{
		if((string[i] < '0') || (string[i] > '9'))
		{
			return false;
		}
	}

	return true;
}

char * getVar16u(int idx)
{
	unsigned short int var = read16u(idx);
	if(!(var & 0x8000))
	{
		sprintf(tempBuffer, "var[0x%04X]", var & 0x7FFF);
	}
	else
	{
		sprintf(tempBuffer, "%d", var & 0x7FFF);
	}

	return tempBuffer;
}

char* getVar16s(int idx, int arg1, int arg2)
{
    // 4C 15 80 20 12 01 25 12 FE 2B 11 03 
	if(arg1 & arg2)
	{
		short int var = read16s(idx);
		sprintf(tempBuffer, "%d", var);
	}
	else
	{
		short int var = read16s(idx);
		sprintf(tempBuffer, "var[0x%04X]", var);
	}

	return tempBuffer;
}

char* readCharacter(int offset)
{
	unsigned char val = script[* position + offset];
	switch(val)
	{
        case 0xFF:
            sprintf(tempBuffer, "PC0(%02X)", val);
            break;
        case 0xFE:
            sprintf(tempBuffer, "PC1(%02X)", val);
            break;
        case 0xFD:
            sprintf(tempBuffer, "PC2(%02X)", val);
            break;
        case 0xFC:
            sprintf(tempBuffer, "PC3(%02X)", val);
            break;
        case 0xFB:
            sprintf(tempBuffer, "THIS(%02X)", val);
            break;
        default:
            sprintf(tempBuffer, "Character(%d)", val);
            break;
	}

	return tempBuffer;
}

char tempVarName[256];

char* getVarName(int varIdx)
{
	switch(varIdx)
	{
        case 0x02:
            sprintf(tempVarName, "ROOM_PARAM(0x%04X)", varIdx);
            break;
        case 0x04:
            sprintf(tempVarName, "MULTI_CHOICE_RESULT(0x%04X)", varIdx);
            break;
        case 0x06:
            sprintf(tempVarName, "MAP_ID(0x%04X)", varIdx);
            break;
        case 0x08:
            sprintf(tempVarName, "PREVIOUS_MAP_ID(0x%04X)", varIdx);
            break;
        case 0x0C:
            sprintf(tempVarName, "PARTY_MEMBER_0(0x%04X)", varIdx);
            break;
        case 0x0E:
            sprintf(tempVarName, "PARTY_MEMBER_1(0x%04X)", varIdx);
            break;
        case 0x10:
            sprintf(tempVarName, "PARTY_MEMBER_2(0x%04X)", varIdx);
            break;
        case 0x18:
            sprintf(tempVarName, "DIGIT_ENTRY_RESULT(0x%04X)", varIdx);
            break;
        case 0x2A:
            sprintf(tempVarName, "USED_KEY_ITEM(0x%04X)", varIdx);
            break;
        case 0x32:
            sprintf(tempVarName, "BATTLE_RESULT(0x%04X)", varIdx);
            break;
        case 0x34:
            sprintf(tempVarName, "DEBUG_MODE(0x%04X)", varIdx);
            break;
        case 0x44:
            sprintf(tempVarName, "CURRENT_FRAME(0x%04X)", varIdx);
            break;
        case 0x4C:
            sprintf(tempVarName, "SAVE_RESULT(0x%04X)", varIdx);
            break;
        default:
            if(varIdx >= 0x400)
            {
                sprintf(tempVarName, "roomVar[0x%02X]", (varIdx-0x400) / 2);
            }
            else
            {
                sprintf(tempVarName, "var[0x%04X]", varIdx);
            }
            break;
	}
	return tempVarName;
}

/////////////////////////////////////////////////////////////////////
extern "C" {

int op0x00() {
    sprintf(descBuffer, "STOP()");
    ret_code = 1;
    return 1;
}
int op0x01() {
    sprintf(descBuffer, "JUMP(0x%04X)", read16u(1));
    jumps.push_back(read16u(1));
    return 3;
}
int op0x02() {
    unsigned char flag = script[* position + 5];
    char value1[256];
    char value2[256];
    unsigned short int jumpOffset = read16u(6);

    if(flag & 0x80)
    {
        sprintf(value1, "%d", read16s(1));
    }
    else
    {
        strcpy(value1, getVar16u(1));
    }

    if(flag & 0x40)
    {
        sprintf(value2, "%d", read16s(3));
    }
    else
    {
        strcpy(value2, getVar16u(3));
    }

    switch(flag & 0xF)
    {
        case 0x0:
            sprintf(descBuffer, "IF_JUMP(%s == %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x1:
            sprintf(descBuffer, "IF_JUMP(%s != %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x2:
            sprintf(descBuffer, "IF_JUMP(%s > %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x3:
            sprintf(descBuffer, "IF_JUMP(%s < %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x4:
            sprintf(descBuffer, "IF_JUMP(%s >= %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x5:
            sprintf(descBuffer, "IF_JUMP(%s <= %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x6:
            sprintf(descBuffer, "IF_JUMP(%s & %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x7:
            sprintf(descBuffer, "IF_JUMP(%s != %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x8:
            sprintf(descBuffer, "IF_JUMP(%s | %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0x9:
            sprintf(descBuffer, "IF_JUMP(%s & %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        case 0xA:
            sprintf(descBuffer, "IF_JUMP(%s ~ %s, 0x%04X)", value1, value2, jumpOffset);
            break;
        default:
            sprintf(descBuffer, "IF_JUMP(%s ? %s, 0x%04X) //? most probably a decompiler bug", value1, value2, jumpOffset);
            break;
    }
    jumps.push_back(jumpOffset);
    //printf("pushed jump %04X", jumpOffset);
    return 8;
}
int op0x03() {
    sprintf(descBuffer, "op03()");
    return 1;
}
int op0x04() {
    sprintf(descBuffer, "op04() // kill script like");
	return 1;
}
int op0x05() {
    sprintf(descBuffer, "CALL(0x%04X)", read16u(1));
    calls.push_back(read16u(1));
    return 3;
}
int op0x06() {
    sprintf(descBuffer, "op06() // kill script like");
    return 1;
}
int op0x07() {
    sprintf(descBuffer, "RUN_ENTITY_SCRIPT_ASYNC(%s, script %d, 0x%02X)", readCharacter(1), script[* position + 2]&0xF, script[* position + 2]&0xF0);
	return 3;
}
int op0x08() {
    sprintf(descBuffer, "RUN_ENTITY_SCRIPT_UNKMODE(%s, script %d, 0x%02X)", readCharacter(1), script[* position + 2]&0xF, script[* position + 2]&0xF0);
    return 3;
}
int op0x09() {
    sprintf(descBuffer, "RUN_ENTITY_SCRIPT_BLOCKING(%s, script %d, 0x%02X)", readCharacter(1), script[* position + 2]&0xF, script[* position + 2]&0xF0);
	return 3;
}
int op0x0A() {
    sprintf(descBuffer, "UPDATE_CHARACTER()");
    return 1;
}
int op0x0B() {
    sprintf(descBuffer, "UPDATE_CHARACTER_AND_STOP()");
    return 1;
}
int op0x0C() {
    sprintf(descBuffer, "op0C() // NOP");
    return 1;
}
int op0x0D() {
    sprintf(descBuffer, "OP_RETURN()");
    ret_code = 1;
	return 1;
}
int op0x0E() {
    sprintf(descBuffer, "SETUP_PC(%s)", readCharacter(1));
    return 3;
}
int op0x0F() {
    sprintf(descBuffer, "SETUP_NPC(%s)", getVar16u(1));
    return 3;
}
int op0x10() {
    sprintf(descBuffer, "%s = %s", getVarName(read16u(1)), getVar16s(3, script[* position  + 5], 0x40));
    return 6;
}
int op0x11() {
    sprintf(descBuffer, "%s = 1", getVarName(read16u(1)));
    return 3;
}
int op0x12() {
    sprintf(descBuffer, "%s = 0", getVarName(read16u(1)));
    return 3;
}
int op0x13() {
    sprintf(descBuffer, "%s += %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x14() {
    sprintf(descBuffer, "%s -= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x15() {
    sprintf(descBuffer, "%s |= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x16() {
    sprintf(descBuffer, "%s ^= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x17() {
    sprintf(descBuffer, "%s ++", getVarName(read16u(1)));
    return 3;
}
int op0x18() {
    sprintf(descBuffer, "%s --", getVarName(read16u(1)));
    return 3;
}
int op0x19() {
    sprintf(descBuffer, "%s &= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x1A() {
    sprintf(descBuffer, "%s |= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x1B() {
    sprintf(descBuffer, "%s ^= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x1C() {
    sprintf(descBuffer, "%s <<= %s", getVarName(read16u(1)), getVar16u(3));
    return 5;
}
int op0x1D() {
    sprintf(descBuffer, "%s >>= %s", getVarName(read16u(1)), getVar16u(3));
    return 5;
}
int op0x1E() {
    sprintf(descBuffer, "%s = rand()", getVarName(read16u(1)));
    return 3;
}
int op0x1F() {
    sprintf(descBuffer, "%s = rand()%%%s", getVarName(read16u(1)), getVar16u(3));
    return 5;
}
int op0x20() {
    sprintf(descBuffer, "var[0x%04X] *= %s", read16u(1), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x21() {
    sprintf(descBuffer, "var[0x%04X] /= %s", read16u(1), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0x22() {
    char temp[50];
    sprintf(descBuffer, "%s = sin(%s, ", getVarName(read16u(1)), getVar16s(3, script[* position + 7], 0x40));
    sprintf(temp, "%s)", getVar16s(5, script[* position + 7], 0x20));
    strcat (descBuffer, temp);
    return 8;
}
int op0x23() {
    char temp[50];
    sprintf(descBuffer, "%s = cos(%s, ", getVarName(read16u(1)), getVar16s(3, script[* position + 7], 0x40));
    sprintf(temp, "%s)", getVar16s(5, script[* position + 7], 0x20));
    strcat (descBuffer, temp);
    return 8;
}
int op0x24() {
    char temp[50];
    sprintf(descBuffer, "%s = tan(%s, ", getVarName(read16u(1)), getVar16s(3, script[* position + 7], 0x40));
    sprintf(temp, "%s)", getVar16s(5, script[* position + 7], 0x20));
    strcat (descBuffer, temp);
    return 8;
}
int op0x25() { ///////////////////////////////////////////////////////////////////////////////////
    sprintf(descBuffer, "SET_POSITION_UNSIGNED(TODO)");
    return 8;
}
int op0x26() {
    sprintf(descBuffer, "SET_POSITION_SIGNED(TODO)");
    return 8;
}
int op0x27() {
    sprintf(descBuffer, "op27(%s)", getVar16u(1));
    return 3;
}
int op0x28() {
    sprintf(descBuffer, "op28(%s)", getVar16u(1));
    return 3;
}
int op0x29() {
    sprintf(descBuffer, "op29(%s)", getVar16u(1));
    return 3; // re-check
}
int op0x2A() {
    sprintf(descBuffer, "op2A(%s)", getVar16s(1, script[* position + 3], 0x80));
    return 4;
}
int op0x2B() {
    sprintf(descBuffer, "ENABLE_CHARACTER_CONTROLS(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x2C() {
    sprintf(descBuffer, "DISABLE_CHARACTER_CONTROLS()");
    return 1;
}
int op0x2D() {
    sprintf(descBuffer, "WAIT(%s)", getVar16u(1));
    return 3;
}
int op0x2E() {
    sprintf(descBuffer, "TURN_TO_CHARACTER(%s)", readCharacter(1));
    return 2;
}
int op0x2F() {
    sprintf(descBuffer, "SET_ROTATION_SIMPLE(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x30() {
    sprintf(descBuffer, "SET_ROTATION(%s)", getVar16u(1));
    return 3;
}
int op0x31() {
    sprintf(descBuffer, "TURN_TO_DIRECTION(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0x32() {
    sprintf(descBuffer, "op32(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0x33() {
    sprintf(descBuffer, "Character(%d) turns to Character(%d)", script[* position + 1]&0x7f, script[* position + 2]&0x7f);
    return 3;
}
int op0x34() {
    sprintf(descBuffer, "op34(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x35() {
    sprintf(descBuffer, "op35(0x%02X, %s)", script[* position + 1], readCharacter(2));
    return 3;
}
int op0x36() {
    sprintf(descBuffer, "SET_CHARACTER_VIEWABLE(0x%02X, %s)", script[* position + 1], readCharacter(2));
    return 3;
}
int op0x37() {
    sprintf(descBuffer, "op37(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x38() {
    sprintf(descBuffer, "op38(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x39() {
    sprintf(descBuffer, "op39(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x3A() {
    sprintf(descBuffer, "op3A()");
    return 1;
}
int op0x3B() {
    sprintf(descBuffer, "STOP_SCRIPT_EXEC()");
    //ret_code = 1;
    return 1;
}
int op0x3C() {
     sprintf(descBuffer, "SET_ANIMATION(%s)", getVar16u(1));
     return 3;
}
int op0x3D() {
    sprintf(descBuffer, "SET_WALK_ANIMATION(0x%02X, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
int op0x3E() {
     sprintf(descBuffer, "op3E(%s)", getVar16u(1));
     return 3;
}
int op0x3F() {
    if (script[* position + 1] == 0) {
        sprintf(descBuffer, "WALK_TO(0x%02X, %s, %s, %s)", script[* position + 1], getVar16s(2, script[* position + 8], 0x80), getVar16s(4, script[* position + 8], 0x40), getVar16s(6, script[* position + 8], 0x20));
        return 9;
    } else {
        sprintf(descBuffer, "WALK_TO(0x%02X) // wait walk", script[* position + 1]);
        return 2;
    }
}
int op0x40() {
    if(script[* position + 1] == 0) {
        sprintf(descBuffer, "WALK_TO_2(0x%02X, %s, %s, %s, %s)", script[* position + 1], getVar16s(2, script[* position + 8], 0x80), getVar16s(4, script[* position + 8], 0x40), getVar16s(6, script[* position + 8], 0x20), getVar16u(9));
        return 11;
    } else {
        sprintf(descBuffer, "WALK_TO_2(0x%02X) // wait walk", script[* position + 1]);
        return 2;
    }
}
int op0x41() {
    sprintf(descBuffer, "SET_MOVEMENT_SPEED(%s)", getVar16u(1));
    return 3;
}
int op0x42() {
    if(script[* position + 1] != 1) {
        sprintf(descBuffer, "WALK_TO_POSITION(0x%02X, %s, %s, %s)", script[* position + 1], getVar16s(2, script[* position + 6], 0x80), getVar16s(4, script[* position + 6], 0x40), getVar16s(6, script[* position + 6], 0x20));
        return 9; // we are missing something here
    } else {
        sprintf(descBuffer, "WALK_TO_POSITION(0x%02X)", script[* position + 1]);
        return 2;
    }
}
int op0x43() {
    if(script[* position + 1] != 1) {
        sprintf(descBuffer, "WALK_TO_POSITION_2(0x%02X, %s, %s, %s)", script[* position + 1], getVar16s(2, script[* position + 6], 0x80), getVar16s(4, script[* position + 6], 0x40), getVar16u(7));
        return 11;
    } else {
        sprintf(descBuffer, "WALK_TO_POSITION_2(0x%02X)", script[* position + 1]);
        return 2;
    }
}
int op0x44() {
    if(script[* position + 1] != 1) {
        sprintf(descBuffer, "op44(0x%02X, %s) //?", script[* position + 1], readCharacter(2));
        return 6;
    } else {
        printf("op44(0x%02X, %s) //?", script[* position + 1], readCharacter(2));
        return 6;
    }
}
int op0x45() {
    if(script[* position + 1] != 1) {
        sprintf(descBuffer, "op45(0x%02X, %s, %s) //?", script[* position + 1], readCharacter(2), getVar16u(3));
        return 8;
    } else {
        sprintf(descBuffer, "op45(0x%02X, %s, %s) //?", script[* position + 1], readCharacter(2), getVar16u(3));
        return 8;
    }
}
int op0x46() {
    if(script[* position + 1] == 1) {
        sprintf(descBuffer, "MOVE(0x%02X) // wait movement completion", script[* position + 1]);
        return 2;
    } else {
        sprintf(descBuffer, "MOVE(0x%02X, %s, %s)", script[* position + 1], getVar16u(2), getVar16u(4));
        // direction
        // length in frame
        return 6;
    }
}
int op0x47() {
    sprintf(descBuffer, "op47(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x48() {
    // setup 3 variables
    sprintf(descBuffer, "GET_POSITION(%s, 0x%04X, 0x%04X, 0x%04X)", readCharacter(1), read16u(2), read16u(4), read16u(6));
	return 8;
}
int op0x49() {
    sprintf(descBuffer, "%s = TARGET_ROTATION & 0xFFF\n", getVarName(read16u(1)));
    return 3;
}
int op0x4A() {
    // setup 3 variables
    sprintf(descBuffer, "op4A(%s, 0x%04X)", readCharacter(1), read16u(2));
    return 4;
}
int op0x4B() {
    sprintf(descBuffer, "DEBUG(%s)", getVarName(read16u(1)));
    return 3;
}
int op0x4C() {
    sprintf(descBuffer, "op4C(0x%04X, %s, %s, %s, %s)", read16u(1), getVar16s(3, script[* position + 0xB], 0x40), getVar16s(5, script[* position + 0xB], 0x20), getVar16s(7, script[* position + 0xB], 0x10), getVar16s(9, script[* position + 0xB], 0x08));
    return 0xC;
}
int op0x4D() {
    char temp[50];
    sprintf(descBuffer, "%s = op4D(", getVarName(read16u(1)));
    sprintf(temp, "%s, ", getVar16s(3, script[* position + 0xF], 0x40));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(5, script[* position + 0xF], 0x20));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(7, script[* position + 0xF], 0x10));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(9, script[* position + 0xF], 0x08));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(11, script[* position + 0xF], 0x04));
    strcat(descBuffer, temp);

    sprintf(temp, "%s)", getVar16s(13, script[* position + 0xF], 0x04));
    strcat(descBuffer, temp);
    return 0x10;
}
int op0x4E() {
    sprintf(descBuffer, "%s = COMPUTE_CHARACTER_DISTANCE(%s, %s)", getVarName(read16u(1)), readCharacter(3), readCharacter(4));
    return 5;
}
int op0x4F() { // Check this shit;
    sprintf(descBuffer, "op4F(0x%02X, %s, %s)", read16u(1), readCharacter(3), readCharacter(4));
    return 5; 
}
int op0x50() {
    sprintf(descBuffer, "IF (scriptVar3 && 0x%02X) == 0 JUMP 0x%02X", read16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
int op0x51() {
    sprintf(descBuffer, "IF (scriptVar2 && 0x%02X) == 0 JUMP 0x%02X", read16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
// 52
// 53
int op0x54() {
    sprintf(descBuffer, "scriptVar2 = 0");
    return 1;
}
int op0x55() {
    sprintf(descBuffer, "op55(%d) JUMP 0x%02X", script[* position + 1], read16u(2));
    jumps.push_back(read16u(2));
	return 4;
}
int op0x56() {
    sprintf(descBuffer, "op56(%d) JUMP 0x%02X", script[* position + 1], read16u(2));
    jumps.push_back(read16u(2));
    return 4;
}
// 57
// 58
int op0x59() {
    sprintf(descBuffer, "op59()");
    return 1;
}
int op0x5A() {
    sprintf(descBuffer, "SET_NEXT_ROOM(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0x5B() {
    if(script[* position + 1] == 0) {
        char temp[50];
        sprintf(descBuffer, "op5B(%d, ", script[* position + 1]);
        sprintf(temp, "%s, ", getVar16s(2, script[* position + 10], 0x80));
        strcat(descBuffer, temp);

        sprintf(temp, "%s, ", getVar16s(4, script[* position + 10], 0x40));
        strcat(descBuffer, temp);

        sprintf(temp, "%s, ", getVar16s(6, script[* position + 10], 0x20));
        strcat(descBuffer, temp);

        sprintf(temp, "%s)", getVar16s(8, script[* position + 10], 0x10));
        return 0xB;
    } else {
        printf("op5B(%d)", script[* position + 1]);
        return 2;
    }
}
int op0x5C() {
    switch(script[* position + 1] & 3) {
        case 0:
        case 1:
        case 2:
        {
            char temp[50];
            sprintf(descBuffer, "JUMP_MOVE(%d, ", script[* position + 1] & 3);
            sprintf(temp, "%s, ", getVar16s(2, script[* position + 8], 0x80));
            strcat(descBuffer, temp);

            sprintf(temp, "%s, ", getVar16s(4, script[* position + 8], 0x40));
            strcat(descBuffer, temp);

            sprintf(temp, "%s, ", getVar16s(6, script[* position + 8], 0x20));
            strcat(descBuffer, temp);

            sprintf(temp, "%s)", getVar16u(9));
            strcat(descBuffer, temp);

            return 0xB;
        }
        case 3:
        {
            sprintf(descBuffer, "JUMP_MOVE(%d)", script[* position + 1] & 3);
            return 0x2;
        }
        default:
        {
            //assert(0);
            printf("JUMP_MOVE() //? Unrecognized argument, unable to determine length");
            ret_code = 4;
            return 0x2;
        }
        
    }
}
int op0x5D() {
    char temp[50];
    sprintf(descBuffer, "SET_CAMERA_POSITION(");
	sprintf(temp, "0x%02X, ", script[* position + 1]);
    strcat(descBuffer, temp);

	sprintf(temp, "%s, ", getVar16s(2, script[* position + 8], 0x80));
    strcat(descBuffer, temp);

	sprintf(temp, "%s, ", getVar16s(4, script[* position + 8], 0x40));
    strcat(descBuffer, temp);

	sprintf(temp, "%s)", getVar16s(6, script[* position + 8], 0x20));
	strcat(descBuffer, temp);
	return 0xB;
}
int op0x5E() {
    if (script[* position + 1] == 0) {
        sprintf(descBuffer, "SET_CAMERA_TO_ACTOR(0x%02X, %s)", script[* position + 1], getVar16u(2));
        return 4;
    } else {
        sprintf(descBuffer, "SET_CAMERA_TO_ACTOR(0x%02X)", script[* position + 1]);
        return 2;
    }
}
int op0x5F() {
    sprintf(descBuffer, "op5F(%s)", readCharacter(1));
	return 2;
}
int op0x60() {
    sprintf(descBuffer, "op60()");
    return 1;
}
int op0x61() {
    switch(script[* position + 1]) {
        case 0x00:
        {
            char temp[50];
            sprintf(descBuffer, "CAMERA_FOLLOW_CHARACTER(%d, ", script[* position + 1]);
            sprintf(temp, "%s, ", readCharacter(2));
            strcat(descBuffer, temp);

            sprintf(temp, "%s)", getVar16u(3));
            strcat(descBuffer, temp);

            sprintf(temp, " //? Check PC1 in CD0-373-Temporal Vortex - Confusion Complex");
            strcat(descBuffer, temp);
            return 5;
        }
        case 0x01:
        {
            sprintf(descBuffer, "CAMERA_FOLLOW_CHARACTER(%d) // wait for camera to reach destination", script[* position + 1]);
            return 2;
        }
        default:
        {
            //assert(0);
            sprintf(descBuffer, "CAMERA_FOLLOW_CHARACTER() //? Unrecognized argument, unable to determine length");
            ret_code = 4;
            return 2;
        }
    }
}
int op0x62() {
    char temp[50];
    sprintf(descBuffer, "SETUP_DIALOG_WINDOW(");
    sprintf(temp, "%s, ", getVar16u(1));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(3));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(5));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(7));
    strcat(descBuffer, temp);

    sprintf(temp, "%s)", getVar16u(9));
    strcat(descBuffer, temp);
    return 11;
}
int op0x63() {
    sprintf(descBuffer, "op63(%s)", getVar16u(1));
    return 3;
}
int op0x64() {
    sprintf(descBuffer, "op64(%s)", getVar16s(1, script[* position + 3], 0x80));
    return 4;
}
int op0x65() {
    sprintf(descBuffer, "op65(0x%04X)", read16u(1));
    return 3;
}
int op0x66() {
    sprintf(descBuffer, "op66(0x%02X, %s)", script[* position + 1], getVar16s(2, script[* position + 4], 0x80));
    return 5;
}
int op0x67() {
    sprintf(descBuffer, "SET_SHADOW_COLOR(0x%04X)", read16u(1));
    return 3;
}
int op0x68() {
    char temp[50];
    sprintf(descBuffer, "SET_SCREEN_RGB(");
    sprintf(temp, "%s, ", getVar16u(1));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(3));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(5));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(7));
    strcat(descBuffer, temp);

    sprintf(temp, "%s)", getVar16u(9));
    strcat(descBuffer, temp);
    return 11;
}
int op0x69() {
    unsigned short int jumpOffset = read16u(3);
    sprintf(descBuffer, "IF %s == SCENARIO_FLAG JUMP 0x%04X", getVar16u(1), jumpOffset);
    jumps.push_back(read16u(jumpOffset));
    return 5;
}
int op0x6A() {
    unsigned short int jumpOffset = read16u(3);
    sprintf(descBuffer, "IF %s < SCENARIO_FLAG JUMP 0x%04X", getVar16u(1), jumpOffset);
    jumps.push_back(read16u(jumpOffset));
    return 5;
}
int op0x6B() {
    unsigned short int jumpOffset = read16u(3);
    sprintf(descBuffer, "IF %s != SCENARIO_FLAG JUMP 0x%04X", getVar16u(1), jumpOffset);
    jumps.push_back(read16u(jumpOffset));
    return 5;
}
int op0x6C() {
    sprintf(descBuffer, "SCENARIO_FLAG = %s", getVar16u(1));
    return 3;
}
int op0x6D() {
    sprintf(descBuffer, "%s = SCENARIO_FLAG", getVarName(read16u(1)));
    return 3;
}
int op0x6E() {
    sprintf(descBuffer, "SWAP(var[0x%02X], var[0x%02X])", read16u(1), read16u(3));
    return 5;
}
int op0x6F() {
    unsigned int arrayOffset = read16u(1);
    sprintf(descBuffer, "%s = array_0x%02X[%s]", getVarName(read16u(3)), arrayOffset, getVar16u(5));
    //assert((decompileArray[arrayOffset]._type == TYPE_UNK) || (decompileArray[arrayOffset]._type == TYPE_ARRAY_8));
    //decompileArray[arrayOffset]._type = TYPE_ARRAY_8;
    return 7;
}
int op0x70() {
    unsigned int arrayOffset = read16u(1);
    sprintf(descBuffer, "%s = array_0x%02X[%s] %d", getVarName(read16u(3)), arrayOffset, getVar16u(5), script[* position + 7]);
    //assert((decompileArray[arrayOffset]._type == TYPE_UNK) || (decompileArray[arrayOffset]._type == TYPE_ARRAY_16));
    //decompileArray[arrayOffset]._type = TYPE_ARRAY_16;
    return 8;
}
int op0x71() {
    sprintf(descBuffer, "SET_VAR_BIT(%s)", getVar16s(1, script[* position + 3], 0x80));
    return 4;
}
int op0x72() { //////////////////////////////// Look at it closely
    const char* varBitNumAsString = getVar16s(1, script[* position + 3], 0x80);
    sprintf(descBuffer, "CLEAR_VAR_BIT(%s)", varBitNumAsString);
    return 4;
}
int op0x73() {
    const char* varBitNumAsString = getVar16s(1, script[* position + 3], 0x80);
    sprintf(descBuffer, "JUMP 0x%04X IF VAR_BIT[%s] CLEAR", read16u(4), varBitNumAsString);
    jumps.push_back(read16u(4));
    return 6;
}
int op0x74() {
    char temp[50];
    sprintf(descBuffer, "op74(");
    sprintf(temp, "%s, ", getVar16s(1, script[* position + 3], 0x80));
    strcat(descBuffer, temp);

    sprintf(temp, "%s)", readCharacter(4));
    strcat(descBuffer, temp);
    return 5;
}
int op0x75() {
    sprintf(descBuffer, "op75(0x%02X, 0x%02X, %s)", script[* position + 1], script[* position + 2], getVar16u(3));
    return 5;
}
int op0x76() {
    sprintf(descBuffer, "op76(0x%02X, 0x%02X, %s)", script[* position + 1], script[* position + 2], getVar16u(3));
    return 5;
}
int op0x77() {
    sprintf(descBuffer, "op77(0x%02X, 0x%02X, 0x%04X)", script[* position + 1], script[* position + 2], read16u(3));
    return 5;
}
int op0x78() {
    sprintf(descBuffer, "op78(0x%02X, 0x%02X, %s)", script[* position + 1], script[* position + 2], getVar16u(3));
    return 5;
}
int op0x79() {
    char temp[50];
    sprintf(descBuffer, "IF op79(%s, ", readCharacter(1));
    sprintf(temp, "%s, ", getVar16u(2));
    strcat(descBuffer, temp);

    sprintf(temp, ") JUMP 0x%04X", read16u(4));
    strcat(descBuffer, temp);
    jumps.push_back(read16u(4));
    return 6;
}
int op0x7A() {
    char temp[50];
    sprintf(descBuffer, "op7A(");
    sprintf(temp, "%s, ", readCharacter(1));
    strcat(descBuffer, temp);

    sprintf(temp, "%s", getVar16u(2));
    strcat(descBuffer, temp);

    sprintf(temp, ") JUMP 0x%04X", read16u(4));
    strcat(descBuffer, temp);
    jumps.push_back(read16u(4));
    return 6;
}
int op0x7B() {
    sprintf(descBuffer, "ADD_CHARATER_TO_PARTY(%s)", getVar16u(1));
    return 3;
}
int op0x7C() {
    sprintf(descBuffer, "EXECUTE_CHARACTER_SCRIPT()");
    return 1;
}
int op0x7D() {
    sprintf(descBuffer, "REMOVE_FROM_PARTY(%s)", readCharacter(1));
    return 2;
}
int op0x7E() {
    sprintf(descBuffer, "PLAY_MOVIE(%s)", getVar16u(1));
    return 3;
}
int op0x7F() {
    sprintf(descBuffer, "WAIT_MOVIE()");
    return 1;
}
int op0x80() {
    sprintf(descBuffer, "ENABLE_BACKGROUND_LAYER(%s)", getVar16u(1)); // max is 0x1F background layer
    return 3;
}
int op0x81() {
    sprintf(descBuffer, "DISABLE_BACKGROUND_LAYER(%s)", getVar16u(1)); // max is 0x1F background layer
    return 3;
}
int op0x82() {
    sprintf(descBuffer, "AXIS_CHARACTER_SCALE(0x%02X, %s, %s)", script[* position + 1], readCharacter(2), getVar16u(3));
    return 5;
}
int op0x83() {
    char temp[50];
    sprintf(descBuffer, "op83(");
    sprintf(temp, "%s, ", getVar16s(1, script[* position + 9], 0x80));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(3, script[* position + 9], 0x40));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(5, script[* position + 9], 0x20));
    strcat(descBuffer, temp);

    sprintf(temp, "%s)", getVar16s(7, script[* position + 9], 0x10));
    strcat(descBuffer, temp);
    return 10;
}
int op0x84() {
    char temp[50];
    sprintf(descBuffer, "op84(");
    sprintf(temp, "%s, ", getVar16s(1, script[* position + 0xD], 0x80));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(3, script[* position + 0xD], 0x40));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(5, script[* position + 0xD], 0x20));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(7, script[* position + 0xD], 0x10));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(9, script[* position + 0xD], 0xF));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16s(0xB, script[* position + 0xD], 0xE));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(0xE));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(0x10));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, ", getVar16u(0x12));
    strcat(descBuffer, temp);

    sprintf(temp, "0x%02X", script[* position + 0x14]);
    strcat(descBuffer, temp);

    sprintf(temp, "0x%02X", script[* position + 0x15]);
    strcat(descBuffer, temp);

    sprintf(temp, "0x%02X", script[* position + 0x16]);
    strcat(descBuffer, temp);

    sprintf(temp, ")");
    strcat(descBuffer, temp);
    return 0x17;
}
int op0x85() {
    sprintf(descBuffer, "op85()");
    return 1;
}
int op0x86() {
    sprintf(descBuffer, "op86(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x87() {
    sprintf(descBuffer, "op87(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
int op0x88() {
    sprintf(descBuffer, "PLAY_SONG(%s)", getVar16u(1));
    return 3;
}
int op0x89() {
    sprintf(descBuffer, "op89(%s)", getVar16u(1)); // not using all the bytes...
    return 5;
}
int op0x8A() {
    sprintf(descBuffer, "op8A() // wait something");
    return 1;
}
int op0x8B() {
    sprintf(descBuffer, "op8B(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0x8C() {
    sprintf(descBuffer, "op8C(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0x8D() {
    sprintf(descBuffer, "op8D(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x8E() {
    sprintf(descBuffer, "op8E(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x8F() {
    char temp[50];
    sprintf(descBuffer, "POLYGON(");
    sprintf(temp, "%s, %s, %s, %s, ", getVar16u(1), getVar16u(3), getVar16u(5), getVar16u(7));
    strcat(descBuffer, temp);

    sprintf(temp, "%s, %s, %s, %s)", getVar16u(9), getVar16u(0xB), getVar16u(0xD), getVar16u(0xF));
    strcat(descBuffer, temp);
    return 0x11;
}
int op0x90() {
    switch(script[* position + 1]) {
        case 0:
        {
            sprintf(descBuffer, "BG_WAVE_OFF(0x%02X, %s)", script[* position + 1], getVar16u(2));
            return 4;
        }	
        case 1:
        case 2:
        case 3:

            sprintf(descBuffer, "BG_WAVE_OFF(0x%02X)", script[* position + 1]);
            return 2;
        default:
        {
            //assert(0);
            sprintf(descBuffer, "BG_WAVE_OFF() //? Unrecognized argument, unable to determine length");
            ret_code = 4;
            return 2;
        }
    }
}
int op0x91() {
    unsigned char param = script[* position + 1];
    switch(param) {
        case 0x01:
        {
            sprintf(descBuffer, "op91(0x%02X)", param);
            return 2;
        }
        default:
        {
            //assert(0);
            sprintf(descBuffer, "op91(0x%02X) //? Unrecognized argument, unable to determine length", param);
            ret_code = 4;
            return 2;
        }
    }
}
int op0x92() {
    sprintf(descBuffer, "op92(0x%02X, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
int op0x93() {
    sprintf(descBuffer, "op93(0x%02X)", script[* position + 1]);
    return 2;
}
int op0x94() {
    sprintf(descBuffer, "IF Character(%s) not in party, JUMP 0x%04X", getVar16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
int op0x95() {
    sprintf(descBuffer, "IF UNKCHECK_Character_op95(%s) JUMP 0x%04X", getVar16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
int op0x96() {
    sprintf(descBuffer, "ADD_CHARACTER_TO_ROSTER(%s)", getVar16u(1));
    return 3;
}
int op0x97() {
    sprintf(descBuffer, "REMOVE_CHARACTER_FROM_ROSTER(%s)", getVar16u(1));
    return 3;
}
int op0x98() {
    sprintf(descBuffer, "%s = FIND_CHARACTER_SLOT_IN_PARTY(%s)", getVarName(read16u(1)), getVar16u(3));
    return 5;
}
int op0x99() {
    sprintf(descBuffer, "var[0x%04X] = PART_CHARACTER_ID(%s)", read16u(1), getVar16u(3));
    return 5;
}
int op0x9A() {
    sprintf(descBuffer, "op9A(%s, 0x%04X)", getVar16u(1), read16u(3));
    return 5;
}
int op0x9B() {
    sprintf(descBuffer, "op9B(\"%c%c%c%c\", %s)", script[* position + 1], script[* position + 2], script[* position + 3], script[* position + 4], readCharacter(5));
    return 6;
}
int op0x9C() {
    sprintf(descBuffer, "op9C(\"%c%c%c%c\")", script[* position + 1], script[* position + 2], script[* position + 3], script[* position + 4]);
    return 5;
}
int op0x9D() {
    sprintf(descBuffer, "op9D(%s)", readCharacter(1));
    return 2;
}
int op0x9E() {
    sprintf(descBuffer, "SETUP_CURRENT_DIALOG_FACE(%s)", getVar16u(1));
    return 3;
}
int op0x9F() {
    sprintf(descBuffer, "CLEAR_DIALOG_FACE()");
    return 1;
}
int op0xA0() {
    // untested
    sprintf(descBuffer, "opA0(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xA1() {
    sprintf(descBuffer, "CHARACTER_RGB(%s, %s, %s, %s)", readCharacter(1), getVar16u(2), getVar16u(4), getVar16u(6));
    return 8;
}
int op0xA2() {
    sprintf(descBuffer, "opA2(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xA3() {
    sprintf(descBuffer, "BATTLE_opA3(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
int op0xA4() {
    sprintf(descBuffer, "opA4(%s)", getVar16u(1));
    return 3;
}
int op0xA5() {
    // untested
    sprintf(descBuffer, "opA5(%s)", getVar16u(1));
    return 3;
}
int op0xA6() {
    sprintf(descBuffer, "opA6(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xA7() {
    sprintf(descBuffer, "opA7(%s, %s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5), getVar16u(7));
    return 9;
}
int op0xA8() {
    sprintf(descBuffer, "opA8(0x%02X, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
int op0xA9() {
    sprintf(descBuffer, "opA9(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xAA() {
    sprintf(descBuffer, "SET_AXIS_ROTATION(0x%02X, %s, %s)", script[* position + 1], readCharacter(2), getVar16u(3));
    return 5;
}
int op0xAB() {
    sprintf(descBuffer, "opAB(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xAC() {
    sprintf(descBuffer, "opAC(0x%02X, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
int op0xAD() {
    sprintf(descBuffer, "opAD(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xAE() {
    sprintf(descBuffer, "opAE(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xAF() {
    sprintf(descBuffer, "opAF(%s, %s, %s)", getVar16s(1, script[* position + 5], 0x80), getVar16s(3, script[* position + 5], 0x40), getVar16u(6));
    return 8;
}
int op0xB0() {
    sprintf(descBuffer, "opB0(%s)", getVar16s(1, script[* position + 3], 0x80));
    return 4;
}
int op0xB1() {
    sprintf(descBuffer, "opB1(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xB2() {
    switch(script[* position + 1])
    {
        case 0:
        {
            sprintf(descBuffer, "OVERRIDE_FOLLOW?(0x%02X)", script[* position + 1]);
            return 2;
        }
        case 1:
        {
            sprintf(descBuffer, "OVERRIDE_FOLLOW(0x%02X, %s)", script[* position + 1], getVar16u(2));
            return 4;
        }
        default:
        {
            //assert(0);
            sprintf(descBuffer, "OVERRIDE_FOLLOW() //? Unrecognized argument, unable to determine length");
            ret_code = 4;
            return 2;
        }
    }
}
int op0xB3() {
    sprintf(descBuffer, "opB3(%s) JUMP 0x%04X", readCharacter(1), read16u(2));
    jumps.push_back(read16u(2));
    return 4;
}
int op0xB4() {
    sprintf(descBuffer, "IF_HAVE_ITEM(%s) ELSE JUMP 0x%04X", getVar16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
int op0xB5() {
    sprintf(descBuffer, "opB5(%s)", getVar16u(1));
    return 3;
}
int op0xB6() {
    //sprintf(descBuffer, "opB6(\"%c%c%c%c\")", script[* position + 1], script[* position + 2], script[* position + 3], script[* position + 4]);
    sprintf(descBuffer, "opB6(%02X, %02X, %02X, %02X)", script[* position + 1], script[* position + 2], script[* position + 3], script[* position + 4]);
    return 5;
}
int op0xB7() {
    sprintf(descBuffer, "opB7(%s)", getVar16s(1, script[* position + 3], 0x80));
    return 4;
}
int op0xB8() {
    sprintf(descBuffer, "opB8(%s) //?", getVar16u(1));
    return 3;
}
int op0xB9() {
    sprintf(descBuffer, "opB9(%d, %s, %s) // missing something", script[* position + 1], readCharacter(2), getVar16u(3));
    return 8;
}
int op0xBA() {
    int subOpcode = script[* position + 1];
    switch(subOpcode) {
        case 0x00: {
            sprintf(descBuffer, "opBA%02X(%s, %s, %s) //? Setup 3 variables", subOpcode, getVar16u(2), getVar16u(4), getVar16u(6));
            return 8;
        }
        case 0x01: {
            sprintf(descBuffer, "opBA%02X() //? Wait", subOpcode);
            return 2;
        }
        default: {
            sprintf(descBuffer, "opBA%02X() //? Unrecognized argument, unable to determine length", subOpcode);
            ret_code = 4;
            return 2;
        }
    }
}
int op0xBB() {
    sprintf(descBuffer, "RECEIVE_ITEM(0x%02X, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
int op0xBC() {
    sprintf(descBuffer, "IF CHECK_MONEY(%d, %d) JUMP 0x%04X", read16u(1), read16u(3), read16u(5));
    jumps.push_back(read16u(5));
    return 7;
}
int op0xBD() {
    sprintf(descBuffer, "RECEIVE_MONEY(%d, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
int op0xBE() {
    sprintf(descBuffer, "opBE()");
    return 1;
}
int op0xBF() {
    if(script[* position + 1] == 0) {
        sprintf(descBuffer, "opBF(%s) // missing stuff, can't decode properly yet", readCharacter(2));
        return 5;
    } else {
        sprintf(descBuffer, "opBF(%s)", readCharacter(1));
        return 2;
    }
}
int op0xC0() {
    sprintf(descBuffer, "opC0(%s)", readCharacter(1));
    return 2;
}
int op0xC1() {
    if(script[* position + 1] == 1) {
        sprintf(descBuffer, "LOAD_ANIMATION(0x01)");
        return 2;
    } else {
        sprintf(descBuffer, "LOAD_ANIMATION(0x%02X, %s, %s)", script[* position + 1], readCharacter(2), getVar16u(3));
        return 5;
    }
}
int op0xC2() {
    sprintf(descBuffer, "opC2(%s, %s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5), getVar16u(7));
    return 9;
}
int op0xC3() {
    sprintf(descBuffer, "GET_CAMERA_POSITION(VAR[0x%02X], VAR[0x%02X])", read16u(1), read16u(3));
    return 5;
}
int op0xC4() {
    sprintf(descBuffer, "WAIT_ANIMATION_COMPLETED?(%s)", readCharacter(1));
    return 2;
}
int op0xC5() {
    sprintf(descBuffer, "opC5(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
int op0xC6() {
    if(script[* position + 1] == 0) {
        sprintf(descBuffer, "PLAY_SOUNDFX(%s, %s)", getVar16u(2), getVar16u(4));
        return 6;
    } else {
        sprintf(descBuffer, "PLAY_SOUNDFX(%s, %s, %s, %s)", getVar16u(2), getVar16u(4), getVar16u(6), getVar16u(8));
        return 10;
    }
}
int op0xC7() {
    sprintf(descBuffer, "opC7(%s)", getVar16u(1));
    return 3;
}
int op0xC8() {
    char temp[50];
    sprintf(descBuffer, "opC8(%s, %s, %s, ", getVar16u(1), getVar16s(3, script[* position + 9], 0x40), getVar16s(5, script[* position + 9], 0x20));
    sprintf(temp, "%s, %s, %s)", getVar16s(7, script[* position + 9], 0x10), getVar16u(10), getVar16u(12));
    strcat(descBuffer, temp);
    return 14;
}
int op0xC9() {
    sprintf(descBuffer, "BATTLE_opC9(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
int op0xCA() {
    sprintf(descBuffer, "opCA()");
    return 1;
}
int op0xCB() {
    sprintf(descBuffer, "WAIT_ROTATION()");
    return 1;
}
int op0xCC() {
    sprintf(descBuffer, "opCC(%s)", readCharacter(1));
    return 2;
}
int op0xCD() {
    sprintf(descBuffer, "opCD(%s) JUMP 0x%04X", getVar16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
int op0xCE() {
    sprintf(descBuffer, "opCE(%s, 0x%02X)", getVar16u(1), read16u(3));
    return 5;
}
int op0xCF() {
    sprintf(descBuffer, "opCF(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xD0() {
    sprintf(descBuffer, "opD0() JUMP 0x%04X", read16u(1));
    jumps.push_back(read16u(1));
    return 3;
}
int op0xD1() {
    sprintf(descBuffer, "opD1(%s)", getVar16u(1));
    return 3;
}
int op0xD2() {
    sprintf(descBuffer, "opD2()");
    return 1;
}
int op0xD3() {
    sprintf(descBuffer, "opD3(\"%c%c%c%c\") //? Effect related", script[* position + 1], script[* position + 2], script[* position + 3], script[* position + 4]);
    return 5;
}
int op0xD4() {
    sprintf(descBuffer, "opD4(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xD5() {
    sprintf(descBuffer, "var[0x%04X] = NUM_OF_ITEM(%s)", read16u(1), getVar16u(3));
    return 5;
}
int op0xD6() {
    sprintf(descBuffer, "opD6(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xD7() {
    sprintf(descBuffer, "opD7(\"%c%c%c%c\")", script[* position + 1], script[* position + 2], script[* position + 3], script[* position + 4]);
    return 5;
}
int op0xD8() {
    sprintf(descBuffer, "opD8(%s, %s, %s) JUMP 0x%04X", readCharacter(1), getVar16u(2), getVar16u(4), read16u(6));
    jumps.push_back(read16u(6));
    return 8;
}
int op0xD9() {
    sprintf(descBuffer, "SETUP_CHARACTER_MODEL(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xDA() {
    sprintf(descBuffer, "WAIT_SETUP_CHARACTER_MODE()");
    return 1;
}
int op0xDB() {
    sprintf(descBuffer, "%s = GET_CHARACTER_ID(%s)", getVarName(read16u(3)), getVar16u(1));
    return 5;
}
int op0xDC() {
    sprintf(descBuffer, "opDC(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xDD() {
    switch(script[* position + 1]) {
        case 0:
        {
            sprintf(descBuffer, "DATA_READ(%d, %s)", script[* position + 1], getVar16u(2));
            return 4;
        }
        default:
        {
            sprintf(descBuffer, "DATA_READ(%d) // wait completion", script[* position + 1]);
            return 2;
        }
    }
}
// DE
int op0xDF() {
    sprintf(descBuffer, "opDF(%s, %s, %s) // missing a byte at the end", getVar16s(1, script[* position + 5], 0x80), getVar16s(3, script[* position + 5], 0x40), getVar16u(6));
    return 9;
}
int op0xE0() {
    sprintf(descBuffer, "DIALOG(0x%04X, 0x%02X)", read16u(1), script[* position + 3]);
    //getDialog(readU16FromScript(1), fOut);
    return 4;
}
int op0xE1() {
    sprintf(descBuffer, "opE1(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xE2() {
    sprintf(descBuffer, "WAIT_DIALOG()");
    return 1;
}
int op0xE3() {
    char temp[50];
    sprintf(descBuffer, "CHARACTER_DIALOG(");
    sprintf(temp, "Character(%d), ", script[* position + 1]&0x7f);
    strcat(descBuffer, temp);

    sprintf(temp, "0x%04X, 0x%02X)", read16u(2), script[* position + 3]);
    strcat(descBuffer, temp);
    return 5;
}
int op0xE4() {
    sprintf(descBuffer, "opE4(%s, 0x%04X, 0x%02X)", readCharacter(1), read16u(2), script[* position + 4]);
    return 5;
}
int op0xE5() {
    sprintf(descBuffer, "IF opE5(%s), JUMP 0x%04X", getVar16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
int op0xE6() {
    sprintf(descBuffer, "IF Character(%s) not recruited, JUMP 0x%04X", getVar16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
int op0xE7() {
    sprintf(descBuffer, "opE7() //? Related to op9B op9C");
    return 5;
}
// E8
int op0xE9() {
    sprintf(descBuffer, "IF CHECK_FRAME(%s) JUMP 0x%04X", getVar16u(1), read16u(3));
    jumps.push_back(read16u(3));
    return 5;
}
// EA
// EB
// EC
// ED
// EE
// EF
// F0
// F1
// F2
// F3
// F4
// F5
// F6
// F7
// F8
// F9
// FA
// FB
// FC
// FD
// FE
// FF

int op0xFE00() {
    sprintf(descBuffer, "opFE00(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE01() {
    sprintf(descBuffer, "opFE01(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE02() {
    sprintf(descBuffer, "opFE02(%s)", getVar16u(1));
    return 3;
}
int op0xFE03() {
    sprintf(descBuffer, "opFE03(%s)", getVar16s(2, script[* position + 0x4], 0x80));
    return 5;
}
int op0xFE04() {
    sprintf(descBuffer, "opFE04(%d)", script[* position + 1]);
    return 2;
}
int op0xFE05() {
    sprintf(descBuffer, "scriptVar1 = %s", getVar16u(1));
    return 3;
}
int op0xFE06() {
    sprintf(descBuffer, "opFE06(%d)", script[* position + 1]);
    return 2;
}
int op0xFE07() {
    sprintf(descBuffer, "opFE07(%d)", script[* position + 1]);
    return 2;
}
// FE08
int op0xFE09() {
    sprintf(descBuffer, "opFE09(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xFE0A() {
    sprintf(descBuffer, "opFE0A(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
// FEOB
int op0xFE0C() {
    sprintf(descBuffer, "FORCE_CHARACTER_DIRECTION(%d, %s)", script[* position + 1], readCharacter(2));
    return 3;
}
int op0xFE0D() {
    sprintf(descBuffer, "opFE0D(%s)", getVar16u(1));
    return 3;
}
int op0xFE0E() {
    sprintf(descBuffer, "opFE0E(%s)", getVar16u(1));
    return 3;
}
int op0xFE0F() {
    sprintf(descBuffer, "opFE0F(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xFE10() {
    sprintf(descBuffer, "opFE10()");
    return 1;
}
int op0xFE11() {
    sprintf(descBuffer, "opFE11(%s)", getVar16u(1));
    return 3;
}
int op0xFE12() {
    sprintf(descBuffer, "opFE12(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
int op0xFE13() {
    sprintf(descBuffer, "opFE13(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
int op0xFE14() {
    sprintf(descBuffer, "opFE14(%d)", script[* position + 1]);
    return 2;
}
int op0xFE15() {
    sprintf(descBuffer, "opFE15(%s)", getVar16u(1));
    return 3;
}
// FE16
int op0xFE17() {
    sprintf(descBuffer, "opFE17(%d)", script[* position + 1]);
    return 2;
}
int op0xFE18() {
    sprintf(descBuffer, "opFE18(%d)", script[* position + 1]);
    return 2;
}
int op0xFE19() {
    sprintf(descBuffer, "opFE19(%d)", script[* position + 1]);
    return 2;
}
int op0xFE1A() {
    sprintf(descBuffer, "opFE1A()");
    return 1;
}
int opx0xFE1B() {
    sprintf(descBuffer, "SET_CURRENT_FRAME(%s)", getVar16u(1));
    return 3;
}
int op0xFE1C() {
    sprintf(descBuffer, "UPDATE_CURRENT_FRAME()");
    return 1;
}
int op0xFE1D() {
    sprintf(descBuffer, "opFE1D(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
// FE1E
// FE1F
// FE20
int op0xFE21() {
    sprintf(descBuffer, "opFE21(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
int op0xFE22() {
    sprintf(descBuffer, "CHANGE_FX_VOLUME(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
int op0xFE23() {
    sprintf(descBuffer, "opFE23(%s, %s, %s)", getVar16u(1), getVar16u(3), getVar16u(5));
    return 7;
}
// FE24
int op0xFE25() {
    sprintf(descBuffer, "opFE25(0x%02X)", script[* position + 1]);
    return 2;
}
// FE26
// FE27
// FE28
int op0xFE29() {
    sprintf(descBuffer, "op29(%s)", getVar16u(1));
    return 3;
}
int op0xFE2A() {
    sprintf(descBuffer, "opFE2A(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE2B() {
    sprintf(descBuffer, "opFE2B(%d, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
int op0xFE2C() {
    sprintf(descBuffer, "opFE2C(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE2D() {
    sprintf(descBuffer, "opFE2D(%s)", getVar16u(1));
    return 3;
}
int op0xFE2E() {
    sprintf(descBuffer, "opFE2E()");
    return 1;
}
int op0xFE2F() {
    sprintf(descBuffer, "opFE2F()");
    return 1;
}
int op0xFE30() {
    sprintf(descBuffer, "opFE30(UNKNOWN)");
    return 8;
}
int op0xFE31() {
    sprintf(descBuffer, "opFE31(Character(%d))", script[* position + 1]&0x7f);
    return 2;
}
int op0xFE32() {
    sprintf(descBuffer, "opFE32()");
    return 1;
}
int op0xFE33() {
    sprintf(descBuffer, "opFE33(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE34() {
    char temp[50];
    sprintf(descBuffer, "opFE34(%s, %s, ", getVar16s(1, script[* position + 0xB], 0x80), getVar16s(3, script[* position + 0xB], 0x40));
    sprintf(temp, "%s, %s, %s)", getVar16s(5, script[* position + 0xB], 0x20), getVar16s(7, script[* position + 0xB], 0x10), getVar16s(9, script[* position + 0xB], 0x08));
    strcat(descBuffer, temp);
    return 0xC;
}
int op0xFE35() {
    sprintf(descBuffer, "opFE35(%s)", getVar16s(1, script[* position + 3], 0x80));
    return 4;
}
int op0xFE36() {
    sprintf(descBuffer, "HEAL_GROUP(%s)", getVar16u(1));
    return 3;
}
int op0xFE37() {
    sprintf(descBuffer, "opFE37(0x%02X, %s)", read16u(1), getVar16u(3));
    return 5;
}
int op0xFE38() {
    sprintf(descBuffer, "OPTIONAL_LIFE(%s, %s)", getVar16s(1, script[* position + 5], 0x80), getVar16u(3));
    return 6;
}
// FE39
int op0xFE3A() {
    sprintf(descBuffer, "opFE3A(%s, %s)", getVar16s(1, script[* position + 5], 0x80), getVar16u(3));
    return 6;
}
int op0xFE3B() {
    sprintf(descBuffer, "opFE3B(%s, Character(%s))", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xFE3C() {
    sprintf(descBuffer, "opFE3C()");
    return 6;
}
int op0xFE3D() {
    sprintf(descBuffer, "CURE_STATUS_AILEMENT(%s, %s)", getVar16u(1), getVar16u(3));
    // 1 = POISON
    // 8 = BURNS
    // 16 = SPRAIN
    // 32 = FLU
    return 0x5;
}
int op0xFE3E() {
    sprintf(descBuffer, "CURE_ALL_STATUS_AILEMENT(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xFE3F() {
    sprintf(descBuffer, "opFE3F()");
    return 1;
}
int op0xFE40() {
    sprintf(descBuffer, "opFE40(%s)", getVar16u(1));
    return 3;
}
int op0xFE41() {
    sprintf(descBuffer, "opFE41(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE42() {
    // save parameter to external array
    sprintf(descBuffer, "opFE42(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE43() {
    sprintf(descBuffer, "opFE43(%s, %s)", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xFE44() {
    sprintf(descBuffer, "DRAW_SYNC()");
    return 1;
}
int op0xFE45() {
    if(script[* position + 1])
    {
        sprintf(descBuffer, "opFE45(0x%02X, %s, 0x%02X, 0x%02X)", script[* position + 1], readCharacter(2), script[* position + 3], script[* position + 4]);
        return 5;
    }
    else
    {
        sprintf(descBuffer, "opFE45(0x%02X, %s)", script[* position + 1], readCharacter(2));
        return 3;
    }
}
int op0xFE46() {
    sprintf(descBuffer, "opFE46(%s, %s, %s)", getVar16s(1, script[* position + 7], 0x80), getVar16s(3, script[* position + 7], 0x40), getVar16s(5, script[* position + 7], 0x20));
    return 8;
}
int op0xFE47() {
    sprintf(descBuffer, "opFE47(%s, %s, %s)", getVar16s(1, script[* position + 7], 0x80), getVar16s(3, script[* position + 7], 0x40), getVar16s(5, script[* position + 7], 0x20));
    return 8;
}
int op0xFE48() {
    sprintf(descBuffer, "opFE48(%s, 0x%02X, 0x%02X, 0x%02X)", getVar16u(2), read16u(4), read16u(6), read16u(8));
    return 0xA;
}
int op0xFE49() {
    sprintf(descBuffer, "opFE49(%s, %s)", getVar16s(1, script[* position + 5], 0x80), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
int op0xFE4A() {
    sprintf(descBuffer, "RENAME_CHARACTER(%s)", getVar16u(1));
    return 3;
}
int op0xFE4B() {
    sprintf(descBuffer,"opFE4B(%d)", script[* position + 1]);
    return 2;
}
int op0xFE4C() {
    sprintf(descBuffer, "opFE4C(%s)", readCharacter(1));
    return 2;
}
// FE4D
int op0xFE4E() {
    sprintf(descBuffer, "opFE4E(%d)", script[* position + 1]);
    return 2;
}
int op0xFE4F() {
    sprintf(descBuffer, "opFE4F(%d, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
int op0xFE50() {
    sprintf(descBuffer, "opFE50()");
    return 1;
}
// FE51
int op0xFE52() {
    sprintf(descBuffer, "opFE52(%s)", getVar16u(1));
    return 3;
}
int op0xFE53() {
    sprintf(descBuffer, "SET_CHARACTER_HEAD_ANGLE(%s, %s, %s)", getVar16s(1, script[* position + 5], 0x80), getVar16s(3, script[* position + 5], 0x40), readCharacter(6));
    return 7;
}
// FE54
int op0xFE55() {
    sprintf(descBuffer, "CHARACTER_LOOK_AT_CHARACTER(%s, %s)", readCharacter(1), readCharacter(2));
    return 3;
}
int op0xFE56() {
    sprintf(descBuffer, "SET_CHARACTER_HEAD_DEFAULT(%s)", readCharacter(1));
    return 2;
}
int op0xFE57() {
    sprintf(descBuffer, "opFE57(0x%02X, %s, 0x%02X) //?", script[* position + 1], getVar16u(2), script[* position + 4]);
    return 5;
}
// FE58
int op0xFE59() {
    sprintf(descBuffer, "opFE59()");
    return 1;
}
int op0xFE5A() {
    sprintf(descBuffer, "opFE5A(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE5B() {
    sprintf(descBuffer,"SETUP_PARTY_SCREEN()");
    return 1;
}
int op0xFE5C() {
    sprintf(descBuffer, "opFE5C(0x%02X, %s, %s)", script[* position + 1], readCharacter(2), getVar16u(3));
    return 5;
}
int op0xFE5D() {
    sprintf(descBuffer, "opFE5D(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xFE5E() {
    sprintf(descBuffer, "SETUP_ELEMENT_SHOP(%s)", getVar16u(1));
    return 3;
}
// 5F
int op0xFE60() {
    sprintf(descBuffer, "opFE60(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xFE61() {
    sprintf(descBuffer, "opFE61()");
    return 1;
}
int op0xFE62() {
    sprintf(descBuffer, "SET_CHARACTER_HEAD_SPEED(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
int op0xFE63() {
    sprintf(descBuffer, "opFE63() // stop script if some variable has been set");
    return 1;
}
int op0xFE64() {
    sprintf(descBuffer, "opFE64(%d, %s)", script[* position + 1], readCharacter(2));
    return 3;
}
int op0xFE65() {
    if(script[* position + 1] == 0)
    {
        sprintf(descBuffer, "CHANGE_DISC(%d, %s)", script[* position + 1], getVar16u(2));
        return 4;
    }
    else
    {
        sprintf(descBuffer, "CHANGE_DISC(%d)", script[* position + 1]);
        return 2;
    }
}
int op0xFE66() {
    sprintf(descBuffer, "opFE66()");
    return 1;
}
int op0xFE67() {
    sprintf(descBuffer, "opFE67(%s)", getVar16u(1));
    return 3;
}
int op0xFE68() {
    sprintf(descBuffer, "SETUP_WEAPON_SHOP(%s)", getVar16u(1));
    return 3;
}
int op0xFE69() {
    sprintf(descBuffer, "opFE69(0x%02X)", script[* position + 1]);
    return 2;
}
// FE6A
int op0xFE6B() {
    sprintf(descBuffer, "opFE6B(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE6C() {
    sprintf(descBuffer, "opFE6C(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE6D() {
    sprintf(descBuffer, "opFE6D()");
    return 1;
}
int op0xFE6E() {
    sprintf(descBuffer, "%s = opFE6E()", getVarName(read16u(1)));
    return 3;
}
int op0xFE6F() {
    sprintf(descBuffer, "opFE6F(%d)", script[* position + 1]);
    return 2;
}
int op0xFE70() {
    sprintf(descBuffer, "opFE70(%s)", readCharacter(1));
    return 2;
}
int op0xFE71() {
    sprintf(descBuffer, "USE_ALTERNATE_PORTRAIT(%d)", script[* position + 1]);
    return 2;
}
int op0xFE72() {
    sprintf(descBuffer, "opFE72()");
    return 1;
}
int op0xFE73() {
    const char* limitIdAsString = getVar16u(3);
    sprintf(descBuffer, "LEARN_CHARACTER_LIMIT_2(%s, %s)", getVar16u(1), limitIdAsString);
    return 5;
}
int op0xFE74() {
    const char* limitIdAsString = getVar16u(3);
    sprintf(descBuffer, "LEARN_CHARACTER_LIMIT_1(%s, %s)", getVar16u(1), limitIdAsString);
    return 5;
}
int op0xFE75() {
    sprintf(descBuffer, "opFE75(%s) //? SETUP_GRANDSLAM_SCREEN", getVar16u(1));
    return 3;
}
int op0xFE76() {
    sprintf(descBuffer, "%s = opFE76()", getVarName(read16u(1)));
    return 3;
}
int op0xFE77() {
    sprintf(descBuffer, "opFE77() //?");
    return 3;
}
int op0xFE78() {
    sprintf(descBuffer, "opFE78(%s, %s) // this will stall the script until some condition is met", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xFE79() {
    const char* limitIdAsString = getVar16u(3);
    sprintf(descBuffer, "LEARN_CHARACTER_LIMIT_0(%s, %s)", getVar16u(1), limitIdAsString);
    return 5;
}
int op0xFE7A() {
    sprintf(descBuffer, "ADD_FRAME_TO_COLLECTION(%s)", getVar16u(1));
    return 3;
}
int op0xFE7B() {
    sprintf(descBuffer, "TOGGLE_CHARACTER_LIMIT_MODE(%s)", getVar16u(1));
    return 3;
}
int op0xFE7C() {
    sprintf(descBuffer, "opFE7C(%s, %s) // Operation with two variables", getVar16u(1), getVar16u(3));
    return 5;
}
int op0xFE7D() {
    sprintf(descBuffer, "opFE7D(%s)", getVar16u(1));
    return 3;
}
int op0xFE7E() {
    sprintf(descBuffer, "opFE7E()");
    return 1;
}
// FE7F
int op0xFE80() {
    sprintf(descBuffer, "opFE80(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE81() {
    sprintf(descBuffer, "opFE81(0x%02X)", script[* position + 1]);
    return 2;
}
int op0xFE82() {
    sprintf(descBuffer, "opFE82(0x%02X) //?", script[* position + 1]);
    return 2;
}
// FE83
int op0xFE84() {
    sprintf(descBuffer, "SETUP_RENAME_SCREEN()");
    return 1;
}
int op0xFE85() {
    sprintf(descBuffer, "SAVE()");
    return 1;
}

}