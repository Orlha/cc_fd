#include "dic.h"

extern "C" void init(unsigned char * script_data, int * offset) {
    script = script_data;
	position = offset;
    printf("Library initializated;\n");
}

extern "C" char * getBuffer() {
    return descBuffer;
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
		sprintf(tempBuffer, "PC0", val);
		break;
	case 0xFE:
		sprintf(tempBuffer, "PC1", val);
		break;
	case 0xFD:
		sprintf(tempBuffer, "PC2", val);
		break;
	case 0xFC:
		sprintf(tempBuffer, "PC3", val);
		break;
	case 0xFB:
		sprintf(tempBuffer, "THIS", val);
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
            sprintf(tempVarName, "ROOM_PARAM", varIdx);
            break;
        case 0x04:
            sprintf(tempVarName, "MULTI_CHOICE_RESULT", varIdx);
            break;
        case 0x06:
            sprintf(tempVarName, "MAP_ID", varIdx);
            break;
        case 0x08:
            sprintf(tempVarName, "PREVIOUS_MAP_ID", varIdx);
            break;
        case 0x0C:
            sprintf(tempVarName, "PARTY_MEMBER_0", varIdx);
            break;
        case 0x0E:
            sprintf(tempVarName, "PARTY_MEMBER_1", varIdx);
            break;
        case 0x10:
            sprintf(tempVarName, "PARTY_MEMBER_2", varIdx);
            break;
        case 0x18:
            sprintf(tempVarName, "DIGIT_ENTRY_RESULT", varIdx);
            break;
        case 0x2A:
            sprintf(tempVarName, "USED_KEY_ITEM", varIdx);
            break;
        case 0x32:
            sprintf(tempVarName, "BATTLE_RESULT", varIdx);
            break;
        case 0x34:
            sprintf(tempVarName, "DEBUG_MODE", varIdx);
            break;
        case 0x44:
            sprintf(tempVarName, "CURRENT_FRAME", varIdx);
            break;
        case 0x4C:
            sprintf(tempVarName, "SAVE_RESULT", varIdx);
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
extern "C" int op0x00() {
    sprintf(descBuffer, "STOP()");
    ret_code = 1;
    return 1;
}
extern "C" int op0x01() {
    sprintf(descBuffer, "JUMP(0x%04X)", read16u(1));
    jumps.push_back(read16u(1));
    return 3;
}
extern "C" int op0x02() {
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
            sprintf(descBuffer, "IF_JUMP(ALWAYS, 0x%04X) //? most probably a decompiler bug", value1, value2, jumpOffset);
            break;
    }
    jumps.push_back(jumpOffset);
    return 8;
}
extern "C" int op0x03() {
    sprintf(descBuffer, "op03()");
    return 1;
}
extern "C" int op0x04() {
    sprintf(descBuffer, "op04() // kill script like");
	return 1;
}
extern "C" int op0x05() {
    sprintf(descBuffer, "CALL(0x%04X)", read16u(1));
    return 3;
}
extern "C" int op0x06() {
    sprintf(descBuffer, "op06() // kill script like");
    return 1;
}
extern "C" int op0x07() {
    sprintf(descBuffer, "RUN_ENTITY_SCRIPT_ASYNC(%s, script %d, 0x%02X)", readCharacter(1), script[* position + 2]&0xF, script[* position + 2]&0xF0);
	return 3;
}
extern "C" int op0x08() {
    sprintf(descBuffer, "RUN_ENTITY_SCRIPT_UNKMODE(%s, script %d, 0x%02X)", readCharacter(1), script[* position + 2]&0xF, script[* position + 2]&0xF0);
    return 3;
}
extern "C" int op0x09() {
    sprintf(descBuffer, "RUN_ENTITY_SCRIPT_BLOCKING(%s, script %d, 0x%02X)", readCharacter(1), script[* position + 2]&0xF, script[* position + 2]&0xF0);
	return 3;
}
extern "C" int op0x0A() {
    sprintf(descBuffer, "UPDATE_CHARACTER()");
    return 1;
}
extern "C" int op0x0B() {
    sprintf(descBuffer, "UPDATE_CHARACTER_AND_STOP()");
    return 1;
}
extern "C" int op0x0C() {
    sprintf(descBuffer, "op0C() // NOP");
    return 1;
}
extern "C" int op0x0D() {
    sprintf(descBuffer, "OP_RETURN()");
    ret_code = 1;
	return 1;
}
extern "C" int op0x0E() {
    sprintf(descBuffer, "SETUP_PC(%s)", readCharacter(1));
    return 3;
}
extern "C" int op0x0F() {
    sprintf(descBuffer, "SETUP_NPC(%s)", getVar16u(1));
    return 3;
}
extern "C" int op0x10() {
    sprintf(descBuffer, "%s = %s", getVarName(read16u(1)), getVar16s(3, script[* position  + 5], 0x40));
    return 6;
}
extern "C" int op0x11() {
    sprintf(descBuffer, "%s = 1", getVarName(read16u(1)));
    return 3;
}
extern "C" int op0x12() {
    sprintf(descBuffer, "%s = 0", getVarName(read16u(1)));
    return 3;
}
extern "C" int op0x13() {
    sprintf(descBuffer, "%s += %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x14() {
    sprintf(descBuffer, "%s -= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x15() {
    sprintf(descBuffer, "%s |= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x16() {
    sprintf(descBuffer, "%s ^= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x17() {
    sprintf(descBuffer, "%s ++", getVarName(read16u(1)));
    return 3;
}
extern "C" int op0x18() {
    sprintf(descBuffer, "%s --", getVarName(read16u(1)));
    return 3;
}
extern "C" int op0x19() {
    sprintf(descBuffer, "%s &= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x1A() {
    sprintf(descBuffer, "%s |= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x1B() {
    sprintf(descBuffer, "%s ^= %s", getVarName(read16u(1)), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x1C() {
    sprintf(descBuffer, "%s <<= %s", getVarName(read16u(1)), getVar16u(3));
    return 5;
}
extern "C" int op0x1D() {
    sprintf(descBuffer, "%s >>= %s", getVarName(read16u(1)), getVar16u(3));
    return 5;
}
extern "C" int op0x1E() {
    sprintf(descBuffer, "%s = rand()", getVarName(read16u(1)));
    return 3;
}
extern "C" int op0x1F() {
    sprintf(descBuffer, "%s = rand()%%%s", getVarName(read16u(1)), getVar16u(3));
    return 5;
}
extern "C" int op0x20() {
    sprintf(descBuffer, "var[0x%04X] *= %s", read16u(1), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x21() {
    sprintf(descBuffer, "var[0x%04X] /= %s", read16u(1), getVar16s(3, script[* position + 5], 0x40));
    return 6;
}
extern "C" int op0x22() {
    char temp[50];
    sprintf(descBuffer, "%s = sin(%s, ", getVarName(read16u(1)), getVar16s(3, script[* position + 7], 0x40));
    sprintf(temp, "%s)", getVar16s(5, script[* position + 7], 0x20));
    strcpy (descBuffer, temp);
    return 8;
}
extern "C" int op0x23() {
    char temp[50];
    sprintf(descBuffer, "%s = cos(%s, ", getVarName(read16u(1)), getVar16s(3, script[* position + 7], 0x40));
    sprintf(temp, "%s)", getVar16s(5, script[* position + 7], 0x20));
    strcpy (descBuffer, temp);
    return 8;
}
extern "C" int op0x24() {
    char temp[50];
    sprintf(descBuffer, "%s = tan(%s, ", getVarName(read16u(1)), getVar16s(3, script[* position + 7], 0x40));
    sprintf(temp, "%s)", getVar16s(5, script[* position + 7], 0x20));
    strcpy (descBuffer, temp);
    return 8;
}
extern "C" int op0x25() { ///////////////////////////////////////////////////////////////////////////////////
    sprintf(descBuffer, "SET_POSITION_UNSIGNED(TODO)");
    return 8;
}
extern "C" int op0x26() {
    sprintf(descBuffer, "SET_POSITION_SIGNED(TODO)");
    return 8;
}
extern "C" int op0x27() {
    sprintf(descBuffer, "op27(%s)", getVar16u(1));
    return 3;
}
extern "C" int op0x28() {
    sprintf(descBuffer, "op28(%s)", getVar16u(1));
    return 3;
}
extern "C" int op0x29() {
    sprintf(descBuffer, "op29(%s)", getVar16u(1));
}
extern "C" int op0x2A() {
    sprintf(descBuffer, "op2A(%s)", getVar16s(1, script[* position + 3], 0x80));
    return 4;
}
extern "C" int op0x2B() {
    sprintf(descBuffer, "ENABLE_CHARACTER_CONTROLS(0x%02X)", script[* position + 1]);
    return 2;
}
extern "C" int op0x2C() {
    sprintf(descBuffer, "DISABLE_CHARACTER_CONTROLS()");
    return 1;
}
extern "C" int op0x2D() {
    sprintf(descBuffer, "WAIT(%s)", getVar16u(1));
    return 3;
}
extern "C" int op0x2E() {
    sprintf(descBuffer, "TURN_TO_CHARACTER(%s)", readCharacter(1));
    return 2;
}
extern "C" int op0x2F() {
    sprintf(descBuffer, "SET_ROTATION_SIMPLE(0x%02X)", script[* position + 1]);
    return 2;
}
extern "C" int op0x30() {
    sprintf(descBuffer, "SET_ROTATION(%s)", getVar16u(1));
    return 3;
}
extern "C" int op0x31() {
    sprintf(descBuffer, "TURN_TO_DIRECTION(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
extern "C" int op0x32() {
    sprintf(descBuffer, "op32(%s, %s)", readCharacter(1), getVar16u(2));
    return 4;
}
extern "C" int op0x33() {
    sprintf(descBuffer, "Character(%d) turns to Character(%d)", script[* position + 1]&0x7f, script[* position + 2]&0x7f);
    return 3;
}
extern "C" int op0x34() {
    sprintf(descBuffer, "op34(0x%02X)", script[* position + 1]);
    return 2;
}
extern "C" int op0x35() {
    sprintf(descBuffer, "op35(0x%02X, %s)", script[* position + 1], readCharacter(2));
    return 3;
}
extern "C" int op0x36() {
    sprintf(descBuffer, "SET_CHARACTER_VIEWABLE(0x%02X, %s)", script[* position + 1], readCharacter(2));
    return 3;
}
extern "C" int op0x37() {
    sprintf(descBuffer, "op37(0x%02X)", script[* position + 1]);
    return 2;
}
extern "C" int op0x38() {
    sprintf(descBuffer, "op38(0x%02X)", script[* position + 1]);
    return 2;
}
extern "C" int op0x39() {
    sprintf(descBuffer, "op39(0x%02X)", script[* position + 1]);
    return 2;
}
extern "C" int op0x3A() {
    sprintf(descBuffer, "op3A()");
    return 1;
}
extern "C" int op0x3B() {
    sprintf(descBuffer, "STOP_SCRIPT_EXEC()");
    ret_code = 1;
    return 1;
}
extern "C" int op0x3C() {
     sprintf(descBuffer, "SET_ANIMATION(%s)", getVar16u(1));
     return 3;
}
extern "C" int op0x3D() {
    sprintf(descBuffer, "SET_WALK_ANIMATION(0x%02X, %s)", script[* position + 1], getVar16u(2));
    return 4;
}
extern "C" int op0x3E() {
     sprintf(descBuffer, "op3E(%s)", getVar16u(1));
     return 3;
}
extern "C" int op0x3F() {
    if (script[* position + 1] == 0) {
        sprintf(descBuffer, "WALK_TO(0x%02X, %s, %s, %s)", script[* position + 1], getVar16s(2, script[* position + 8], 0x80), getVar16s(4, script[* position + 8], 0x40), getVar16s(6, script[* position + 8], 0x20));
        return 9;
    } else {
        sprintf(descBuffer, "WALK_TO(0x%02X) // wait walk", script[* position + 1]);
        return 2;
    }
}