#include "decoder.h"

Decoder::Decoder(unsigned char * script_data, int * offset) {
	script = script_data;
	position = offset;
	script_begin = * position;
}

vector<unsigned int> * Decoder::getJumps() {
    return &jumps;
}

unsigned short int Decoder::read16u(int idx)
{
	return *(short unsigned int*)(script + * position + idx);
}

signed short int Decoder::read16s(int idx)
{
	return *(short signed int*)(script + * position + idx);
}

bool Decoder::isNumeric(const char* string)
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


char * Decoder::getValueOrVarU(int idx)
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

char* Decoder::getValueOrVarS(int idx, int arg1, int arg2)
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

char* Decoder::readCharacter(int offset)
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

char* Decoder::getVarName(int varIdx)
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


int Decoder::decode() {
	int & currentScriptPosition = * position;
	int & pos = * position;
	unsigned char * currentScriptData = script;
	
	printf("0x%04x: ", pos - script_begin);
	int code = script[pos];
	
	int ret_code = 0;
	
	int pre_pos = * position;
	switch(code)
	{
		case 0x00: {
			printf("STOP()");
			currentScriptPosition += 1;
			ret_code = 1;
			break;
		}
		case 0x01:
		{
			printf("JUMP(0x%04X)", read16u(1));
            currentScriptPosition += 3;
            jumps.push_back(read16u(1));
			break;
		}
		case 0x02:
		{
			unsigned char flag = currentScriptData[currentScriptPosition + 5];

			char value1[256];
			char value2[256];

            unsigned short int jumpOffset = read16u(6);
            
            jumps.push_back(jumpOffset);

			if(flag & 0x80)
			{
				sprintf(value1, "%d", read16s(1));
			}
			else
			{
				strcpy(value1, getValueOrVarU(1));
			}

			if(flag & 0x40)
			{
				sprintf(value2, "%d", read16s(3));
			}
			else
			{
				strcpy(value2, getValueOrVarU(3));
			}

            /*
			if(strcmp(value1, "USED_KEY_ITEM") == 0)
			{
				if(flag & 0x40)
				{
					sprintf(value2, "\"%s\"", GetItemName(readU16FromScript(3)));
				}
            }
            */

			switch(flag & 0xF)
			{
			case 0x0:
                printf("IF_JUMP(%s == %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x1:
                printf("IF_JUMP(%s != %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x2:
                printf("IF_JUMP(%s > %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x3:
                printf("IF_JUMP(%s < %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x4:
                printf("IF_JUMP(%s >= %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x5:
                printf("IF_JUMP(%s <= %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x6:
                printf("IF_JUMP(%s & %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x7:
                printf("IF_JUMP(%s != %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x8:
                printf("IF_JUMP(%s | %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0x9:
                printf("IF_JUMP(%s & %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			case 0xA:
                printf("IF_JUMP(%s ~ %s, 0x%04X)", value1, value2, jumpOffset);
				break;
			default:
                printf("IF_JUMP(ALWAYS, 0x%04X) //? most probably a decompiler bug", value1, value2, jumpOffset);
				break;
			}
			currentScriptPosition += 8;
			break;
		}
		case 0x03:
		{
			printf("op03()");
			currentScriptPosition += 1;
			break;
		}
		case 0x04:
		{
			printf("op04() // kill script like");
			currentScriptPosition += 1;
			break;
		}
		case 0x05:
		{
			printf("CALL(0x%04X)", read16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x06:
		{
			printf("op06() // kill script like");
			currentScriptPosition += 1;
			break;
		}
		case 0x07:
		{
			printf("RUN_ENTITY_SCRIPT_ASYNC(%s, 0x%02X)", readCharacter(1), currentScriptData[currentScriptPosition + 2]);
			currentScriptPosition += 3;
			break;
		}
		case 0x08:
		{
			printf("RUN_ENTITY_SCRIPT_UNKMODE(%s, 0x%02X)", readCharacter(1), currentScriptData[currentScriptPosition + 2]);
			currentScriptPosition += 3;
			break;
		}
		case 0x09:
		{
			printf("RUN_ENTITY_SCRIPT_BLOCKING(%s, 0x%02X)", readCharacter(1), currentScriptData[currentScriptPosition + 2]);
			currentScriptPosition += 3;
			break;
		}
		case 0x0A:
		{
			printf("op0A()");
			currentScriptPosition += 1;
			break;
		}
		case 0x0B:
		{
			printf("UPDATE_CHARACTER_AND_STOP()");
			currentScriptPosition += 1;
			break;
		}
		case 0x0C:
		{
			printf("op0C() // NOP");
			currentScriptPosition += 1;
			break;
		}
		case 0x0D:
		{
			printf("OP_RETURN()");
			currentScriptPosition += 1;
			break;
		}
		case 0x0E:
		{
			printf("SETUP_PC(%s)", readCharacter(1));
			currentScriptPosition += 3;
			break;
		}
		case 0X0F:
		{
			printf("SETUP_NPC(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x10:
		{
			printf("var[0x%04X] = %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;
		}
		case 0x11:
		{
			printf("var[%04X] = true", read16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x12:
		{
			printf("var[0x%04X] = false", read16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x13:
		{
			printf("var[0x%04X] += %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;			
		}
		case 0x14:
		{
			printf("var[0x%04X] -= %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;			
		}
		case 0x15:
		{
			printf("var[0x%04X] |= %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;			
		}
		case 0x16:
		{
			printf("var[0x%04X] ^= %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;			
		}
		case 0x17:
		{
			printf("var[0x%04X]++", read16u(1));
			currentScriptPosition += 3;
			break;			
		}
		case 0x18:
		{
			printf("var[0x%04X]--", read16u(1));
			currentScriptPosition += 3;
			break;			
		}
		case 0x19:
		{
			printf("var[0x%04X] &= %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;
		}
		case 0x1A:
		{
			printf("var[0x%04X] |= %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;
		}
		case 0x1B:
		{
			printf("%s ^= %s", getVarName(read16u(1)), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;
		}
		case 0x1C:
		{
			printf("var[0x%04X] <<= %s", read16u(1), getValueOrVarU(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x1D:
		{
			printf("%s >>= %s", getVarName(read16u(1)), getValueOrVarU(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x1E:
		{
			printf("var[0x%04X] = rand()", read16u(1));
			currentScriptPosition += 3;
			break;	
		}
		case 0x1F:
		{
			printf("var[0x%04X] = rand()%%%s", read16u(1), getValueOrVarU(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x20:
		{
			printf("var[0x%04X] *= %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;
		}
		case 0x21:
		{
			printf("var[0x%04X] /= %s", read16u(1), getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			currentScriptPosition += 6;
			break;
		}
		case 0x22:
		{
			printf("%s = sin(%s, ", getVarName(read16u(1)), getValueOrVarS(3, currentScriptData[currentScriptPosition + 7], 0x40));
			printf("%s)", getValueOrVarS(5, currentScriptData[currentScriptPosition + 7], 0x20));
			currentScriptPosition += 8;
			break;
		}
		case 0x23:
		{
			printf("%s = cos(%s, ", getVarName(read16u(1)), getValueOrVarS(3, currentScriptData[currentScriptPosition + 7], 0x40));
			printf("%s)", getValueOrVarS(5, currentScriptData[currentScriptPosition + 7], 0x20));
			currentScriptPosition += 8;
			break;
		}
		case 0x24:
		{
			printf("%s = tan(%s, ", getVarName(read16u(1)), getValueOrVarS(3, currentScriptData[currentScriptPosition + 7], 0x40));
			printf("%s)", getValueOrVarS(5, currentScriptData[currentScriptPosition + 7], 0x20));
			currentScriptPosition += 8;
			break;
		}
		case 0x25:
		{
			printf("SET_POSITION_UNSIGNED(TODO)");
			currentScriptPosition += 8;
			break;
		}
		case 0x26:
		{
			printf("SET_POSITION_SIGNED(TODO)");
			currentScriptPosition += 8;
			break;
		}
		case 0x27:
		{
			printf("op27(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x28:
		{
			printf("op28(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x29:
		{
			printf("op29(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x2A:
		{
			printf("op2A(%s)", getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80));
			currentScriptPosition += 4;
			break;
		}
		case 0x2B:
		{
			printf("ENABLE_CHARACTER_CONTROLS(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x2C:
		{
			printf("DISABLE_CHARACTER_CONTROLS()");
			currentScriptPosition += 1;
			break;
		}
		case 0x2D:
		{
			printf("WAIT(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x2E:
		{
			printf("TURN_TO_CHARACTER(%s)", readCharacter(1));
			currentScriptPosition += 2;
			break;
		}
		case 0x2F:
		{
			printf("SET_ROTATION_SIMPLE(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x30:
		{
			printf("SET_ROTATION(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x31:
		{
			printf("TURN_TO_DIRECTION(%s", readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(")");
			currentScriptPosition += 4;
			break;
		}
		case 0x32:
		{
			printf("op32(%s", readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(")");
			currentScriptPosition += 4;
			break;
		}
		case 0x33:
		{
			printf("Character(%d) turns to Character(%d)", currentScriptData[currentScriptPosition + 1]&0x7f, currentScriptData[currentScriptPosition + 2]&0x7f);
			currentScriptPosition += 3;
			break;
		}
		case 0x34:
		{
			printf("op34(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x35:
		{
			printf("op35(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], readCharacter(2));
			currentScriptPosition += 3;
			break;
		}
		case 0x36:
		{
			// setup 3 variables
			printf("SET_CHARACTER_VIEWABLE(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], readCharacter(2));
			currentScriptPosition += 3;
			break;
		}
		case 0x37:
		{
			printf("op37(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x38:
		{
			printf("op38(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x39:
		{
			printf("op39(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x3A:
		{
			printf("op3A()");
			currentScriptPosition += 1;
			break;
		}
		case 0x3B:
		{
			printf("STOP_SCRIPT_EXEC()");
			currentScriptPosition += 1;
			//return 1;
			break;
		}
		case 0x3C:
		{
			printf("SET_ANIMATION(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x3D:
		{
			printf("SET_WALK_ANIMATION(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
			currentScriptPosition += 4;
			break;
		}
		case 0x3E:
		{
			printf("op3E(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x3F:
		{
			if(currentScriptData[currentScriptPosition + 1] == 0)
			{
				printf("WALK_TO(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
				printf(getValueOrVarS(2, currentScriptData[currentScriptPosition + 8], 0x80));
				printf(", ");
				printf(getValueOrVarS(4, currentScriptData[currentScriptPosition + 8], 0x40));
				printf(", ");
				printf(getValueOrVarS(6, currentScriptData[currentScriptPosition + 8], 0x20));
				printf(")");
				currentScriptPosition += 9;
			}
			else
			{
				printf("WALK_TO(0x%02X) // wait walk", currentScriptData[currentScriptPosition + 1]);
				currentScriptPosition += 2;
			}
			break;
		}
		case 0x40:
		{
			if(currentScriptData[currentScriptPosition + 1] == 0)
			{
				printf("WALK_TO_2(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
				printf(getValueOrVarS(2, currentScriptData[currentScriptPosition + 8], 0x80));
				printf(", ");
				printf(getValueOrVarS(4, currentScriptData[currentScriptPosition + 8], 0x40));
				printf(", ");
				printf(getValueOrVarS(6, currentScriptData[currentScriptPosition + 8], 0x20));
				printf(", ");
				printf(getValueOrVarU(9));
				printf(")");
				currentScriptPosition += 11;
			}
			else
			{
				printf("WALK_TO_2(0x%02X) // wait walk", currentScriptData[currentScriptPosition + 1]);
				currentScriptPosition += 2;
			}
			break;
		}
		case 0x41:
		{
			printf("SET_MOVEMENT_SPEED(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x42:
		{
			if(currentScriptData[currentScriptPosition + 1] != 1)
			{
				printf("WALK_TO_POSITION(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
				printf(getValueOrVarS(2, currentScriptData[currentScriptPosition + 6], 0x80));
				printf(", ");
				printf(getValueOrVarS(4, currentScriptData[currentScriptPosition + 6], 0x40));
				printf(", ");
				printf(getValueOrVarS(6, currentScriptData[currentScriptPosition + 6], 0x20));
				printf(")");
				currentScriptPosition += 9; // we are missing something here
			}
			else
			{
				printf("WALK_TO_POSITION(0x%02X)", currentScriptData[currentScriptPosition + 1]);
				currentScriptPosition += 2;
			}
			break;
		}
		case 0x43:
		{
			if(currentScriptData[currentScriptPosition + 1] != 1)
			{
                printf("WALK_TO_POSITION_2(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
                printf(getValueOrVarS(2, currentScriptData[currentScriptPosition + 6], 0x80));
                printf(", ");
                printf(getValueOrVarS(4, currentScriptData[currentScriptPosition + 6], 0x40));
                printf(", ");
                printf(getValueOrVarU(7));
                printf(")");
				currentScriptPosition += 11;
			}
			else
			{
				printf("WALK_TO_POSITION_2(0x%02X)", currentScriptData[currentScriptPosition + 1]);
				currentScriptPosition += 2;
			}
			break;
		}
		case 0x44:
		{
			if(currentScriptData[currentScriptPosition + 1] != 1)
			{
				printf("op44(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
				printf(readCharacter(2));
				printf(") //?");
				currentScriptPosition += 6;
			}
			else
			{
				printf("op44(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
				printf(readCharacter(2));
				printf(") //?");
				currentScriptPosition += 6;
			}
			break;
		}
		case 0x45:
		{
			if(currentScriptData[currentScriptPosition + 1] != 1)
			{
				printf("op45(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
				printf(readCharacter(2));
				printf(", ");
				printf(getValueOrVarU(3));
				printf(") //?");
				currentScriptPosition += 8;
			}
			else
			{
				printf("op45(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
				printf(readCharacter(2));
				printf(", ");
				printf(getValueOrVarU(3));
				printf(") //?");
				currentScriptPosition += 8;
			}
			break;
		}
		case 0x46:
		{
			if(currentScriptData[currentScriptPosition + 1] == 1)
			{
				printf("MOVE(0x%02X) // wait movement completion", currentScriptData[currentScriptPosition + 1]);
				currentScriptPosition += 2;
			}
			else
			{
				printf("MOVE(0x%02X", currentScriptData[currentScriptPosition + 1]);
				printf(", ");
				printf(getValueOrVarU(2)); // direction
				printf(", ");
				printf(getValueOrVarU(4)); // length in frame
				printf(")");
				currentScriptPosition += 6;
			}
			break;
		}
		case 0x47:
		{
			printf("op47(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x48:
		{
			// setup 3 variables
			printf("GET_POSITION(%s, 0x%04X, 0x%04X, 0x%04X)", readCharacter(1), read16u(2), read16u(4), read16u(6));
			currentScriptPosition += 8;
			break;
		}
		case 0x49:
		{
			printf("%s = TARGET_ROTATION & 0xFFF\n", getVarName(read16u(1)));
			currentScriptPosition += 3;
			break;
		}
		case 0x4A:
		{
			// setup 3 variables
			printf("op4A(%s, 0x%04X)", readCharacter(1), read16u(2));
			currentScriptPosition += 4;
			break;
		}
		case 0x4B:
		{
			printf("DEBUG(%s)", getVarName(read16u(1)));
			currentScriptPosition += 3;
			break;
		}
		case 0x4C:
		{
			printf("op4C(");
			printf("0x04X, ", read16u(1));
			printf(", ");
			printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 0xB], 0x40));
			printf(", ");
			printf(getValueOrVarS(5, currentScriptData[currentScriptPosition + 0xB], 0x20));
			printf(", ");
			printf(getValueOrVarS(7, currentScriptData[currentScriptPosition + 0xB], 0x10));
			printf(", ");
			printf(getValueOrVarS(9, currentScriptData[currentScriptPosition + 0xB], 0x08));
			printf(")");
			currentScriptPosition += 0xC;
			break;
		}
		case 0x4D:
		{
			printf("%s = op4D(", getVarName(read16u(1)));
			printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 0xF], 0x40));
			printf(", ");
			printf(getValueOrVarS(5, currentScriptData[currentScriptPosition + 0xF], 0x20));
			printf(", ");
			printf(getValueOrVarS(7, currentScriptData[currentScriptPosition + 0xF], 0x10));
			printf(", ");
			printf(getValueOrVarS(9, currentScriptData[currentScriptPosition + 0xF], 0x08));
			printf(", ");
			printf(getValueOrVarS(11, currentScriptData[currentScriptPosition + 0xF], 0x04));
			printf(", ");
			printf(getValueOrVarS(13, currentScriptData[currentScriptPosition + 0xF], 0x04));
			printf(")");
			currentScriptPosition += 0x10;
			break;
		}
		case 0x4E:
		{
			printf("%s = COMPUTE_CHARACTER_DISTANCE(", getVarName(read16u(1)));
			printf(readCharacter(3));
			printf(", ");
			printf(readCharacter(4));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0x4F:
		{
			printf("op4F(0x%02X, ", readCharacter(1));
			printf(readCharacter(3));
			printf(", ");
			printf(readCharacter(4));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0x50:
		{
			printf("if (scriptVar3 && 0x%02X) == 0 jump 0x%02X", read16u(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x51:
		{
			printf("if (scriptVar2 && 0x%02X) == 0 jump 0x%02X", read16u(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x54:
		{
			printf("scriptVar2 = 0");
			currentScriptPosition += 1;
			break;
		}
		case 0x55:
		{
			printf("op55(%d) jump 0x%02X", currentScriptData[currentScriptPosition + 1], read16u(2));
			currentScriptPosition += 4;
			break;
		}
		case 0x56:
		{
			printf("op56(%d) jump 0x%02X", currentScriptData[currentScriptPosition + 1], read16u(2));
			currentScriptPosition += 4;
			break;
		}
		case 0x59:
		{
			printf("op59()");
			currentScriptPosition += 1;
			break;
		}
		case 0x5A:
		{
			printf("SET_NEXT_ROOM(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0x5B:
		{
			if(currentScriptData[currentScriptPosition + 1] == 0)
			{
				printf("op5B(%d", currentScriptData[currentScriptPosition + 1]);
				printf(", ");
				printf(getValueOrVarS(2, currentScriptData[currentScriptPosition + 10], 0x80));
				printf(", ");
				printf(getValueOrVarS(4, currentScriptData[currentScriptPosition + 10], 0x40));
				printf(", ");
				printf(getValueOrVarS(6, currentScriptData[currentScriptPosition + 10], 0x20));
				printf(", ");
				printf(getValueOrVarS(8, currentScriptData[currentScriptPosition + 10], 0x10));
				printf(")");
				currentScriptPosition += 0xb;
			}
			else
			{
				printf("op5B(%d)", currentScriptData[currentScriptPosition + 1]);
				currentScriptPosition += 2;
			}
			break;
		}
		case 0x5C:
		{
			switch(currentScriptData[currentScriptPosition + 1] & 3)
			{
				case 0:
				case 1:
				case 2:
				{
					printf("JUMP_MOVE(%d, ", currentScriptData[currentScriptPosition + 1] & 3);
					printf(getValueOrVarS(2, currentScriptData[currentScriptPosition + 8], 0x80));
					printf(", ");
					printf(getValueOrVarS(4, currentScriptData[currentScriptPosition + 8], 0x40));
					printf(", ");
					printf(getValueOrVarS(6, currentScriptData[currentScriptPosition + 8], 0x20));
					printf(", ");
					printf(getValueOrVarU(9));
					printf(")");
					currentScriptPosition += 0xB;
					break;
				}
				case 3:
				{
					printf("JUMP_MOVE(%d)", currentScriptData[currentScriptPosition + 1] & 3);
					currentScriptPosition += 0x2;
					break;
				}
				default:
				{
					//assert(0);
					return 4;
					break;
				}
				
			}
			break;
		}
		case 0x5D:
		{
			printf("SET_CAMERA_POSITION(");
			printf("0x%02X", currentScriptData[currentScriptPosition + 1]);
			printf(", ");
			printf(getValueOrVarS(2, currentScriptData[currentScriptPosition + 8], 0x80));
			printf(", ");
			printf(getValueOrVarS(4, currentScriptData[currentScriptPosition + 8], 0x40));
			printf(", ");
			printf(getValueOrVarS(6, currentScriptData[currentScriptPosition + 8], 0x20));
			printf(")");
			currentScriptPosition += 0xb;
			break;
		}
		case 0x5E:
		{
			if (currentScriptData[currentScriptPosition + 1] == 0)
			{
				printf("SET_CAMERA_TO_ACTOR(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
				printf(getValueOrVarU(2));
				printf(")");
				currentScriptPosition += 4;
			}
			else
			{
				printf("SET_CAMERA_TO_ACTOR(0x%02X)", currentScriptData[currentScriptPosition + 1]);
				currentScriptPosition += 2;
			}
			break;
		}
		case 0x5F:
		{
			printf("op5F(%s)", readCharacter(1));
			currentScriptPosition += 2;
			break;
		}
		case 0x60:
		{
			printf("op60()");
			currentScriptPosition += 1;
			break;
		}
		case 0x61:
		{
			switch(currentScriptData[currentScriptPosition + 1])
			{
                case 0x00:
                {
                    /*
                    if(strcmp("PC0", readCharacter(2)) == 0)
                    {
                        //assert(0);
                        return 4;
                    }
                    else
                    {
                        */
                        printf("CAMERA_FOLLOW_CHARACTER(%d, ", currentScriptData[currentScriptPosition + 1]);
                        printf(readCharacter(2));
                        printf(", ");
                        printf(getValueOrVarU(3));
                        printf(") //? Check PC1 in CD0-373-Temporal Vortex - Confusion Complex");
                        currentScriptPosition += 5;
                    //}
                    break;
                }
			    case 0x01:
				{
					printf("CAMERA_FOLLOW_CHARACTER(%d) // wait for camera to reach destination", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
                default:
                {
                    //assert(0);
                    return 4;
                    break;
                }
			}
			break;
		}
		case 0x62:
		{
			printf("SETUP_DIALOG_WINDOW(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(", ");
			printf(getValueOrVarU(7));
			printf(", ");
			printf(getValueOrVarU(9));
			printf(")");
			currentScriptPosition += 11;
			break;
		}
		case 0x63:
		{
			printf("op63(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x64:
		{
			printf("op64(%s)", getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80));
			currentScriptPosition += 4;
			break;
		}
		case 0x65:
		{
			printf("op65(0x%04X)", read16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x66:
		{
			printf("op66(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarS(2, currentScriptData[currentScriptPosition + 4], 0x80));
			currentScriptPosition += 5;
			break;
		}
		case 0x67:
		{
			printf("SET_SHADOW_COLOR(0x%04X)", read16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x68:
		{
			printf("SET_SCREEN_RGB(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(", ");
			printf(getValueOrVarU(7));
			printf(", ");
			printf(getValueOrVarU(9));
			printf(")");
			currentScriptPosition += 11;
			break;
		}
		case 0x69:
		{
			unsigned short int jumpOffset = read16u(3);
			printf("if %s == SCENARIO_FLAG jump 0x%04X", getValueOrVarU(1), jumpOffset);
			currentScriptPosition += 5;
			break;
		}
		case 0x6A:
		{
			unsigned short int jumpOffset = read16u(3);
			printf("if %s < SCENARIO_FLAG jump 0x%04X", getValueOrVarU(1), jumpOffset);
			currentScriptPosition += 5;
			break;
		}
		case 0x6B:
		{
			unsigned short int jumpOffset = read16u(3);
			printf("if %s != SCENARIO_FLAG jump 0x%04X", getValueOrVarU(1), jumpOffset);
			currentScriptPosition += 5;
			break;
		}
		case 0x6C:
		{
			printf("SCENARIO_FLAG = %s", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x6D:
		{
			printf("%s = SCENARIO_FLAG", getVarName(read16u(1)));
			currentScriptPosition += 3;
			break;
		}
		case 0x6E:
		{
			printf("SWAP(var[0x%02X], var[0x%02X])", read16u(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x6F:
		{
			unsigned long int arrayOffset = read16u(1);
			printf("%s = array_0x%02X[%s]", getVarName(read16u(3)), arrayOffset, getValueOrVarU(5));

			//assert((decompileArray[arrayOffset]._type == TYPE_UNK) || (decompileArray[arrayOffset]._type == TYPE_ARRAY_8));
			//decompileArray[arrayOffset]._type = TYPE_ARRAY_8;

			currentScriptPosition += 7;
			break;
		}
		case 0x70:
		{
			unsigned long int arrayOffset = read16u(1);
			printf("%s = array_0x%02X[%s] %d", getVarName(read16u(3)), arrayOffset, getValueOrVarU(5), currentScriptData[currentScriptPosition + 7]);

			//assert((decompileArray[arrayOffset]._type == TYPE_UNK) || (decompileArray[arrayOffset]._type == TYPE_ARRAY_16));
			//decompileArray[arrayOffset]._type = TYPE_ARRAY_16;

			currentScriptPosition += 8;
			break;
		}
		case 0x71:
		{
			printf( "SET_VAR_BIT(%s)", getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80));
			currentScriptPosition += 4;
			break;
		}
		case 0x72:
		{
			const char* varBitNumAsString = getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80);
			printf("CLEAR_VAR_BIT(%s)", varBitNumAsString);
			currentScriptPosition += 4;
			break;
		}
		case 0x73:
		{
            const char* varBitNumAsString = getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80);
            printf("JUMP 0x%04X IF VAR_BIT[%s] CLEAR", read16u(4), varBitNumAsString, 0x80);
			currentScriptPosition += 6;
			break;
		}
		case 0x74:
		{
			printf("op74(");
			printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80));
			printf(", ");
			printf(readCharacter(4));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0x75:
		{
			printf("op75(0x%02X, 0x%02X, %s)", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], getValueOrVarU(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x76:
		{
			printf("op76(0x%02X, 0x%02X, %s)", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], getValueOrVarU(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x77:
		{
			printf("op77(0x%02X, 0x%02X, 0x%04X)", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x78:
		{
			printf("op78(0x%02X, 0x%02X, %s)", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], getValueOrVarU(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x79:
		{
			printf("if op79(%s", readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(") jump 0x%04X", read16u(4));
			currentScriptPosition += 6;
			break;
		}
		case 0x7A:
		{
			printf("op7A(");
			printf(readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(") jump 0x%04X", read16u(4));
			currentScriptPosition += 6;
			break;
		}
		case 0x7B:
		{
			printf("ADD_CHARATER_TO_PARTY(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x7C:
		{
			printf("EXECUTE_CHARACTER_SCRIPT()");
			currentScriptPosition += 1;
			break;
		}
		case 0x7D:
		{
			printf("REMOVE_FROM_PARTY(%s)", readCharacter(1));
			currentScriptPosition += 2;
			break;
		}
		case 0x7E:
		{
			printf("PLAY_MOVIE(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x7F:
		{
			printf("WAIT_MOVIE()");
			currentScriptPosition += 1;
			break;
		}
		case 0x80:
		{
			printf("ENABLE_BACKGROUND_LAYER(%s)", getValueOrVarU(1)); // max is 0x1F background layer
			currentScriptPosition += 3;
			break;
		}
		case 0x81:
		{
			printf("DISABLE_BACKGROUND_LAYER(%s)", getValueOrVarU(1)); // max is 0x1F background layer
			currentScriptPosition += 3;
			break;
		}
		case 0x82:
		{
			printf("AXIS_CHARACTER_SCALE(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
			printf(readCharacter(2));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0x83:
		{
			printf("op83(");
			printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 9], 0x80));
			printf(", ");
			printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 9], 0x40));
			printf(", ");
			printf(getValueOrVarS(5, currentScriptData[currentScriptPosition + 9], 0x20));
			printf(", ");
			printf(getValueOrVarS(7, currentScriptData[currentScriptPosition + 9], 0x10));
			printf(")");
			currentScriptPosition += 10;
			break;
		}
		case 0x84:
		{
			printf("op84(");
			printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 0xD], 0x80));
			printf(", ");
			printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 0xD], 0x40));
			printf(", ");
			printf(getValueOrVarS(5, currentScriptData[currentScriptPosition + 0xD], 0x20));
			printf(", ");
			printf(getValueOrVarS(7, currentScriptData[currentScriptPosition + 0xD], 0x10));
			printf(", ");
			printf(getValueOrVarS(9, currentScriptData[currentScriptPosition + 0xD], 0xF));
			printf(", ");
			printf(getValueOrVarS(0xB, currentScriptData[currentScriptPosition + 0xD], 0xE));
			printf(", ");
			printf(getValueOrVarU(0xE));
			printf(", ");
			printf(getValueOrVarU(0x10));
			printf(", ");
			printf(getValueOrVarU(0x12));
			printf(", ");
			printf("0x%02X", currentScriptData[currentScriptPosition + 0x14]);
			printf(", ");
			printf("0x%02X", currentScriptData[currentScriptPosition + 0x15]);
			printf(", ");
			printf("0x%02X", currentScriptData[currentScriptPosition + 0x16]);
			printf(")");
			currentScriptPosition += 0x17;
			break;
		}
		case 0x85:
		{
			printf("op85()");
			currentScriptPosition += 1;
			break;
		}
		case 0x86:
		{
			printf("op86(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x87:
		{
			printf("op87(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(")");
			currentScriptPosition += 7;
			break;
		}
		case 0x88:
		{
			printf("PLAY_SONG(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x89:
		{
			printf("op89(%s)", getValueOrVarU(1)); // not using all the bytes...
			currentScriptPosition += 5;
			break;
		}
		case 0x8A:
		{
			printf("op8A() // wait something");
			currentScriptPosition += 1;
			break;
		}
		case 0x8B:
		{
			printf("op8B(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0x8C:
		{
			printf("op8C(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0x8D:
		{
			printf("op8D(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x8E:
		{
			printf("op8E(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x8F:
		{
			printf("POLYGON(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(", ");
			printf(getValueOrVarU(7));
			printf(", ");
			printf(getValueOrVarU(9));
			printf(", ");
			printf(getValueOrVarU(0xB));
			printf(", ");
			printf(getValueOrVarU(0xD));
			printf(", ");
			printf(getValueOrVarU(0xF));
			printf(")");
			currentScriptPosition += 0x11;
			break;
		}
		case 0x90:
		{
			switch(currentScriptData[currentScriptPosition + 1])
			{
				case 0:
				{
					printf("BG_WAVE_OFF( 0x%02X, %s )", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
					currentScriptPosition += 4;
					break;
				}	
				case 1:
				case 2:
				case 3:

					printf("BG_WAVE_OFF( 0x%02X )", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				default:
					//assert(0);
					return 4;
			}
			break;
		}
		case 0x91:
		{
			unsigned char param = currentScriptData[currentScriptPosition + 1];

			switch(param)
			{
				case 0x01:
				{
					printf("op91(0x%02X)", param);
					currentScriptPosition += 2;
					break;
				}
				default:
				{
					printf("op91(0x%02X) UNKNOWN - CHECK", param);
					//assert(0);
					return 4;
				}
			}
			break;
		}
		case 0x92:
		{
			printf("op92(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
			currentScriptPosition += 4;
			break;
		}
		case 0x93:
		{
			printf("op93(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0x94:
		{
			printf("if Character(%s) not in party, jump 0x%04X", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x95:
		{
			printf("if UNKCHECK_Character_op95(%s) jump 0x%04X", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x96:
		{
			printf("ADD_CHARACTER_TO_ROSTER(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x97:
		{
			printf("REMOVE_CHARACTER_FROM_ROSTER(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x98:
		{
			printf("%s = FIND_CHARACTER_SLOT_IN_PARTY(", getVarName(read16u(1)));
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0x99:
		{
			printf("var[0x%04X] = PART_CHARACTER_ID(%s)", read16u(1), getValueOrVarU(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x9A:
		{
			printf("op9A(%s, 0x%04X)", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0x9B:
		{
			printf("op9B(\"%c%c%c%c\", %s)", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], currentScriptData[currentScriptPosition + 3], currentScriptData[currentScriptPosition + 4], readCharacter(5));
			currentScriptPosition += 6;
			break;
		}
		case 0x9C:
		{
			printf("op9C(\"%c%c%c%c\")", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], currentScriptData[currentScriptPosition + 3], currentScriptData[currentScriptPosition + 4]);
			currentScriptPosition += 5;
			break;
		}
		case 0x9D:
		{
			printf("op9D(%s)", readCharacter(1));
			currentScriptPosition += 2;
			break;
		}
		case 0x9E:
		{
			printf("SETUP_CURRENT_DIALOG_FACE(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0x9F:
		{
			printf("CLEAR_DIALOG_FACE()");
			currentScriptPosition += 1;
			break;
		}
		case 0xA0:
		{
			// untested
			printf("opA0(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0xA1:
		{
			printf("CHARACTER_RGB(");
			printf(readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(", ");
			printf(getValueOrVarU(4));
			printf(", ");
			printf(getValueOrVarU(6));
			printf(")");
			currentScriptPosition += 8;
			break;
		}
		case 0xA2:
		{
			printf("opA2(");
			printf(readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(")");
			currentScriptPosition += 4;
			break;
		}
		case 0xA3:
		{
			printf("BATTLE_opA3(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(")");
			currentScriptPosition += 7;
			break;
		}
		case 0xA4:
		{
			printf("opA4(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xA5:
		{
			// untested
			printf("opA5(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xA6:
		{
			printf("opA6(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0xA7:
		{
			printf("opA7(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(", ");
			printf(getValueOrVarU(7));
			printf(")");
			currentScriptPosition += 9;
			break;
		}
		case 0xA8:
		{
			printf("opA8(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
			currentScriptPosition += 4;
			break;
		}
		case 0xA9:
		{
			printf("opA9(");
			printf(readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf( ")");
			currentScriptPosition += 4;
			break;
		}
		case 0xAA:
		{
			printf("SET_AXIS_ROTATION(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
			printf(readCharacter(2));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xAB:
		{
			printf("opAB(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xAC:
		{
			printf("opAC(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
			currentScriptPosition += 4;
			break;
		}
		case 0xAD:
		{
			printf("opAD(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0xAE:
		{
			printf("opAE(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0xAF:
		{
			printf("opAF(");
			printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 5], 0x80));
			printf(", ");
			printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			printf(", ");
			printf(getValueOrVarU(6));
			printf(")\n");
			currentScriptPosition += 8;
			break;
		}
		case 0xB0:
		{
			printf("opB0(%s)", getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80));
			currentScriptPosition += 4;
			break;
		}
		case 0xB1:
		{
			printf("opB1(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0xB2:
		{
			switch(currentScriptData[currentScriptPosition + 1])
			{
                case 0:
                {
                    printf("OVERRIDE_FOLLOW?(0x%02X)\n", currentScriptData[currentScriptPosition + 1]);
				    currentScriptPosition += 2;
                    break;
                }
			    case 1:
				{
					printf("OVERRIDE_FOLLOW(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
					printf(getValueOrVarU(2));
					printf(")");
                    currentScriptPosition += 4;
                    break;
                }
                default:
                {
                    //assert(0);
                    return 4;
                }
			}
			break;
		}
		case 0xB3:
		{
			printf("opB3(%s) jump 0x%04X", readCharacter(1), read16u(2));
			currentScriptPosition += 4;
			break;
		}
		case 0xB4:
		{
			printf("IF_HAVE_ITEM(%s) ELSE JUMP 0x%04X", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xB5:
		{
			printf("opB5(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xB6:
		{
			printf("opB6(\"%c%c%c%c\")", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], currentScriptData[currentScriptPosition + 3], currentScriptData[currentScriptPosition + 4]);
			currentScriptPosition += 5;
			break;
		}
		case 0xB7:
		{
			printf("opB7(%s)", getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80));
			currentScriptPosition += 4;
			break;
		}
		case 0xB8:
		{
			printf("opB8(%s) //?", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xB9:
		{
			printf("opB9(%d, ", currentScriptData[currentScriptPosition + 1]);
			printf(readCharacter(2));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(") // missing something");
			currentScriptPosition += 8;
			break;
		}
		case 0xBA:
		{
			int subOpcode = currentScriptData[currentScriptPosition + 1];
			if (subOpcode == 0)
			{
				printf("opBA00(");
				printf(getValueOrVarU(2));
				printf(", ");
				printf(getValueOrVarU(4));
				printf(", ");
				printf(getValueOrVarU(6));
				printf(") // ??");
				currentScriptPosition += 8;
			}
			else if (subOpcode == 1)
			{
				printf("opBA01(");
				printf(")");
				currentScriptPosition += 2;
			}
			else {
				printf("FATAL: Unknown opcode 0xBA 0x%02X", subOpcode);
				currentScriptPosition += 2;
				return 4;
				break;
			}
			break;
		}
		case 0xBB:
		{
			printf("RECEIVE_ITEM(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
			currentScriptPosition += 4;
			break;
		}
		case 0xBC:
		{
			printf("if CHECK_MONEY(%d, %d) jump 0x%04X", read16u(1), read16u(3), read16u(5));
			currentScriptPosition += 7;
			break;
		}
		case 0xBD:
		{
			printf("RECEIVE_MONEY(%d, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
			currentScriptPosition += 4;
			break;
		}
		case 0xBE:
		{
			printf("opBE()");
			currentScriptPosition += 1;
			break;
		}
		case 0xBF:
		{
			if(currentScriptData[currentScriptPosition + 1] == 0)
			{
				printf("opBF(");
				printf(readCharacter(2));
				printf(") // missing stuff, can't decode properly yet");
				currentScriptPosition += 5;
			}
			else
			{
				printf("opBF(");
				printf(readCharacter(1));
				printf(")");
				currentScriptPosition += 2;
			}
			break;
		}
		case 0xC0:
		{
			printf("opC0(%s)", readCharacter(1));
			currentScriptPosition += 2;
			break;
		}
		case 0xC1:
		{
			if(currentScriptData[currentScriptPosition + 1] == 1)
			{
				printf("LOAD_ANIMATION(0x01)");
				currentScriptPosition += 2;
			}
			else
			{
				printf("LOAD_ANIMATION(0x%02X, %s", currentScriptData[currentScriptPosition + 1], readCharacter(2)); 
				printf(", %s)", getValueOrVarU(3));
				currentScriptPosition += 5;
			}
			break;
		}
		case 0xC2:
		{
			printf("opC2(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(", ");
			printf(getValueOrVarU(7));
			printf(")");
			currentScriptPosition += 9;
			break;
		}
		case 0xC3:
		{
			printf("GET_CAMERA_POSITION(VAR[0x%02X], VAR[0x%02X])", read16u(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xC4:
		{
			printf("WAIT_ANIMATION_COMPLETED?(%s)", readCharacter(1));
			currentScriptPosition += 2;
			break;
		}
		case 0xC5:
		{
			printf("opC5(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(")");
			currentScriptPosition += 7;
			break;
		}
		case 0xC6:
		{
			printf("PLAY_SOUNDFX(");

			if(currentScriptData[currentScriptPosition + 1] == 0)
			{
				printf(getValueOrVarU(2));
				printf(", ");
				printf(getValueOrVarU(4));
				currentScriptPosition += 6;
			}
			else
			{
				printf(getValueOrVarU(2));
				printf(", ");
				printf(getValueOrVarU(4));
				printf(", ");
				printf(getValueOrVarU(6));
				printf(", ");
				printf(getValueOrVarU(8));
				currentScriptPosition += 10;
			}
			printf(")");
			break;
		}
		case 0xC7:
		{
			printf("opC7(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xC8:
		{
			printf("opC8(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 9], 0x40));
			printf(", ");
			printf(getValueOrVarS(5, currentScriptData[currentScriptPosition + 9], 0x20));
			printf(", ");
			printf(getValueOrVarS(7, currentScriptData[currentScriptPosition + 9], 0x10));
			printf(", ");
			printf(getValueOrVarU(10));
			printf(", ");
			printf(getValueOrVarU(12));
			printf(")");
			currentScriptPosition += 14;
			break;
		}
		case 0xC9:
		{
			printf("BATTLE_opC9(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(", ");
			printf(getValueOrVarU(5));
			printf(")");
			currentScriptPosition += 7;
			break;
		}
		case 0xCA:
		{
			printf("opCA()");
			currentScriptPosition += 1;
			break;
		}
		case 0xCB:
		{
			printf("WAIT_ROTATION()");
			currentScriptPosition += 1;
			break;
		}
		case 0xCC:
		{
			printf("opCC(%s)", readCharacter(1));
			currentScriptPosition += 2;
			break;
		}
		case 0xCD:
		{
			printf("opCD(%s) jump 0x%04X", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xCE:
		{
			printf("opCE(%s, 0x%02X)", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xCF:
		{
			printf("opCF(");
			printf(readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(")");
			currentScriptPosition += 4;
			break;
		}
		case 0xD0:
		{
			printf("opD0() jump 0x%04X", read16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xD1:
		{
			printf("opD1(%s)", getValueOrVarU(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xD2:
		{
			printf("opD2()");
			currentScriptPosition += 1;
			break;
		}
		case 0xD3:
		{
			// effect related ?
			printf("opD3(\"%c%c%c%c\")", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], currentScriptData[currentScriptPosition + 3], currentScriptData[currentScriptPosition + 4]);
			currentScriptPosition += 5;
			break;
		}
		case 0xD4:
		{
			printf("opD4(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xD5:
		{
			printf("var[0x%04X] = NUM_OF_ITEM(%s)", read16u(1), getValueOrVarU(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xD6:
		{
			printf("opD6(");
			printf(readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(")");
			currentScriptPosition += 4;
			break;
		}
		case 0xD7:
		{
			printf("opD7(\"%c%c%c%c\")", currentScriptData[currentScriptPosition + 1], currentScriptData[currentScriptPosition + 2], currentScriptData[currentScriptPosition + 3], currentScriptData[currentScriptPosition + 4]);
			currentScriptPosition += 5;
			break;
		}
		case 0xD8:
		{
			printf("opD8( ");
			printf(readCharacter(1));
			printf(", ");
			printf(getValueOrVarU(2));
			printf(", ");
			printf(getValueOrVarU(4));
			printf(") jump 0x%04X", read16u(6));
			currentScriptPosition += 8;
			break;
		}
		case 0xD9:
		{
			printf("SETUP_CHARACTER_MODEL(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xDA:
		{
			printf("WAIT_SETUP_CHARACTER_MODE()");
			currentScriptPosition += 1;
			break;
		}
		case 0xDB:
		{
			printf("%s = GET_CHARACTER_ID(%s)", getVarName(read16u(3)), getValueOrVarU(1));
			currentScriptPosition += 5;
			break;
		}
		case 0xDC:
		{
			printf("opDC(");
			printf(getValueOrVarU(1));
			printf(", ");
			printf(getValueOrVarU(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xDD:
		{
			switch(currentScriptData[currentScriptPosition + 1])
			{
				case 0:
				{
					printf("DATA_READ(%d, ", currentScriptData[currentScriptPosition + 1]);
					printf(getValueOrVarU(2));
					printf(")");
					currentScriptPosition += 4;
					break;
				}
			default:
				{
					printf("DATA_READ(%d) // wait completion", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}	
			}
			break;
		}
		case 0xDF:
		{
			printf("opDF(");
			printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 5], 0x80));
			printf(", ");
			printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
			printf(", ");
			printf(getValueOrVarU(6));
			printf(") // missing a byte at the end");
			currentScriptPosition += 9;
			break;
		}
		case 0xE0:
		{
			printf("DIALOG(0x%04X, 0x%02X)", read16u(1), currentScriptData[currentScriptPosition + 3]);
			//getDialog(readU16FromScript(1), fOut);
			currentScriptPosition += 4;
			break;
		}
		case 0xE1:
		{
			printf("opE1(0x%02X)", currentScriptData[currentScriptPosition + 1]);
			currentScriptPosition += 2;
			break;
		}
		case 0xE2:
		{
			printf("WAIT_DIALOG()");
			currentScriptPosition += 1;
			break;
		}
		case 0xE3:
		{
			printf("CHARACTER_DIALOG(");
			printf("Character(%d)", currentScriptData[currentScriptPosition + 1]&0x7f);
			printf(", ");
			printf("0x%04X", read16u(2));
			printf(", ");
			printf("0x%02X", currentScriptData[currentScriptPosition + 3]);
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xE4:
		{
			printf("opE4(");
			printf(readCharacter(1));
			printf(", 0x%04X, 0x%02X)", read16u(2), currentScriptData[currentScriptPosition + 4]);
			currentScriptPosition += 5;
			break;
		}
		case 0xE5:
		{
			printf("if opE5(%s), jump 0x%04X", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xE6:
		{
			printf("if Character(%s) not recruited, jump 0x%04X", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
        }
        case 0xE7:
		{
			printf("opE7() //? Related to op9B op9C");
			currentScriptPosition += 5;
			break;
		}
		case 0xE9:
		{
			printf("if CHECK_FRAME(%s) jump 0x%04X", getValueOrVarU(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xFE:
		{
			currentScriptPosition++;
			unsigned char subOpcode = currentScriptData[currentScriptPosition];
			switch(subOpcode)
			{
                case 0x00:
				{
					printf("opFE00(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x01:
				{
					printf("opFE01(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x02:
				{
					printf("opFE02(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x03:
				{
					printf("opFE03(");
					printf(getValueOrVarS(2, currentScriptData[currentScriptPosition + 0x4], 0x80));
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x04:
				{
					printf("opFE04(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x05:
				{
					printf("scriptVar1 = %s", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x06:
				{
					printf("opFE06(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x07:
				{
					printf("opFE07(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x09:
				{
					printf("opFE09(");
					printf(readCharacter(1));
					printf(", ");
					printf(getValueOrVarU(2));
					printf(")");
					currentScriptPosition += 4;
					break;
				}
				case 0x0A:
				{
					printf("opFE0A(");
					printf(readCharacter(1));
					printf(", ");
					printf(getValueOrVarU(2));
					printf(")");
					currentScriptPosition += 4;
					break;
				}
				case 0x0C:
				{
					printf("FORCE_CHARACTER_DIRECTION(%d, %s)", currentScriptData[currentScriptPosition + 1], readCharacter(2));
					currentScriptPosition += 3;
					break;
				}
				case 0x0D:
				{
					printf("opFE0D(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x0E:
				{
					printf("opFE0E(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x0F:
				{
					printf("opFE0F(%s, %s)", readCharacter(1), getValueOrVarU(2));
					currentScriptPosition += 4;
					break;
				}
				case 0x10:
				{
					printf("opFE10()");
					currentScriptPosition += 1;
					break;
				}
				case 0x11:
				{
					printf("opFE11(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x12:
				{
					printf("opFE12(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(", ");
					printf(getValueOrVarU(5));
					printf(")");
					currentScriptPosition += 7;
					break;
                }
                case 0x13:
				{
					printf("opFE13(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(", ");
					printf(getValueOrVarU(5));
					printf(")");
					currentScriptPosition += 7;
					break;
				}
				case 0x14:
				{
					printf("opFE14(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x15:
				{
					printf("opFE15(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x17:
				{
					printf("opFE17(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x18:
				{
					printf("opFE18(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x19:
				{
					printf("opFE19(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x1A:
				{
					printf("opFE1A()");
					currentScriptPosition += 1;
					break;
				}
				case 0x1B:
				{
					printf("SET_CURRENT_FRAME(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x1C:
				{
					printf("UPDATE_CURRENT_FRAME()");
					currentScriptPosition += 1;
					break;
				}
				case 0x1D:
				{
					printf("opFE1D(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x21:
				{
					printf("op21(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(", ");
					printf(getValueOrVarU(5));
					printf(")");
					currentScriptPosition += 7;
					break;
				}
				case 0x22:
				{
					printf("CHANGE_FX_VOLUME( ");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(", ");
					printf(getValueOrVarU(5));
					printf(")");
					currentScriptPosition += 7;
					break;
                }
                case 0x23:
				{
					printf("opFE23( ");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(", ");
					printf(getValueOrVarU(5));
					printf(")");
					currentScriptPosition += 7;
					break;
				}
				case 0x25:
				{
					printf("opFE25(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x29:
				{
					printf("op29(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x2A:
				{
					printf("opFE2A(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x2B:
				{
					printf("opFE2B(%d, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
					currentScriptPosition += 4;
					break;
				}
				case 0x2C:
				{
					printf("opFE2C(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x2D:
				{
					printf("opFE2D(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
                case 0x2E:
				{
					printf("opFE2E()");
					currentScriptPosition += 1;
					break;
				}
                case 0x2F:
				{
					printf("opFE2F()");
					currentScriptPosition += 1;
					break;
				}
				case 0x30:
				{
					printf("opFE30(UNKNOWN)");
					currentScriptPosition += 8;
					break;
				}
				case 0x31:
				{
					printf("opFE31(Character(%d))", currentScriptData[currentScriptPosition + 1]&0x7f);
					currentScriptPosition += 2;
					break;
				}
				case 0x32:
				{
					printf("opFE32()");
					currentScriptPosition += 1;
					break;
				}
				case 0x33:
				{
					printf("opFE33(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x34:
				{
					printf("opFE34(");
					printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 0xB], 0x80));
					printf(", ");
					printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 0xB], 0x40));
					printf(", ");
					printf(getValueOrVarS(5, currentScriptData[currentScriptPosition + 0xB], 0x20));
					printf(", ");
					printf(getValueOrVarS(7, currentScriptData[currentScriptPosition + 0xB], 0x10));
					printf(", ");
					printf(getValueOrVarS(9, currentScriptData[currentScriptPosition + 0xB], 0x08));
					printf(")");
					currentScriptPosition += 0xC;
					break;
				}
				case 0x35:
				{
					printf("opFE35(%s)", getValueOrVarS(1, currentScriptData[currentScriptPosition + 3], 0x80));
					currentScriptPosition += 4;
					break;
				}
				case 0x36:
				{
					printf("HEAL_GROUP(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x37:
				{
					printf("opFE37(");
					printf("0x%02X", read16u(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x38:
				{
					printf("OPTIONAL_LIFE(");
					printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 5], 0x80));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(")");
					currentScriptPosition += 6;
					break;
				}
				case 0x3A:
				{
					printf("opFE3A(");
					printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 5], 0x80));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(")");
					currentScriptPosition += 6;
					break;
                }
                case 0x3B:
				{
					printf("opFE3B(");
					printf(getValueOrVarU(1));
					printf(", Character(");
					printf(getValueOrVarU(3));
					printf("))");
					currentScriptPosition += 5;
					break;
                }
                case 0x3C:
				{
					printf("opFE3C()");
					currentScriptPosition += 6;
					break;
				}
				case 0x3D:
				{
					printf("CURE_STATUS_AILEMENT(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(")");
					// 1 = POISON
					// 8 = BURNS
					// 16 = SPRAIN
					// 32 = FLU
					currentScriptPosition += 0x5;
					break;
				}
				case 0x3E:
				{
					printf("CURE_ALL_STATUS_AILEMENT(%s", getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x3F:
				{
					printf("opFE3F()");
					currentScriptPosition += 1;
					break;
				}
				case 0x40:
				{
					printf("opFE40(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
                }
                case 0x41:
				{
					printf("opFE41(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
                case 0x42:
				{
					// save parameter to external array
					printf("opFE42(0x%02X)", currentScriptData[currentScriptPosition+1]);
					currentScriptPosition += 2;
					break;
                }
                case 0x43:
				{
					printf("opFE43(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(")");
					currentScriptPosition += 5;
					break;
				}
                case 0x44:
				{
					printf("DRAW_SYNC()");
					currentScriptPosition += 1;
					break;
				}
				case 0x45:
				{
					if(currentScriptData[currentScriptPosition + 1])
					{
						printf("opFE45(");
						printf("0x%02X, ", currentScriptData[currentScriptPosition + 1]);
						printf("%s, ", readCharacter(2));
						printf("0x%02X, ", currentScriptData[currentScriptPosition + 3]);
						printf("0x%02X", currentScriptData[currentScriptPosition + 4]);
						printf(")");
						currentScriptPosition += 5;
					}
					else
					{
						printf("opFE45(");
						printf("0x%02X, ", currentScriptData[currentScriptPosition + 1]);
						printf("%s", readCharacter(2));
						printf(")");
						currentScriptPosition += 3;
					}
					break;
				}
				case 0x46:
				{
					printf("opFE46(");
					printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 7], 0x80));
					printf(", ");
					printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 7], 0x40));
					printf(", ");
					printf(getValueOrVarS(5, currentScriptData[currentScriptPosition + 7], 0x20));
					printf(")");
					currentScriptPosition += 8;
					break;
				}
				case 0x47:
				{
					printf("opFE47(");
					printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 7], 0x80));
					printf(", ");
					printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 7], 0x40));
					printf(", ");
					printf(getValueOrVarS(5, currentScriptData[currentScriptPosition + 7], 0x20));
					printf(")");
					currentScriptPosition += 8;
					break;
				}
				case 0x48:
				{
					printf("opFE48(");
					printf(getValueOrVarU(2));
					printf(", ");
					printf("0x%02X", read16u(4));
					printf(", ");
					printf("0x%02X", read16u(6));
					printf(", ");
					printf("0x%02X", read16u(8));
					printf(")");
					currentScriptPosition += 0xA;
					break;
				}
				case 0x49:
				{
					printf("op49(");
					printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 5], 0x80));
					printf(", ");
					printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
					printf(")");
					currentScriptPosition += 6;
					break;
				}
				case 0x4A:
				{
					printf("RENAME_CHARACTER(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x4B:
				{
					printf("opFE4B(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x4C:
				{
					printf("opFE4C(%s)", readCharacter(1));
					currentScriptPosition += 2;
					break;
				}
				case 0x4E:
				{
					printf("opFE4E(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x4F:
				{
					printf("opFE4F(%d, %s)", currentScriptData[currentScriptPosition + 1], getValueOrVarU(2));
					currentScriptPosition += 4;
					break;
				}
				case 0x50:
				{
					printf("opFE50()");
					currentScriptPosition += 1;
					break;
				}
				case 0x52:
				{
					printf("opFE52(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x53:
				{
					printf("SET_CHARACTER_HEAD_ANGLE(");
					printf(getValueOrVarS(1, currentScriptData[currentScriptPosition + 5], 0x80));
					printf(", ");
					printf(getValueOrVarS(3, currentScriptData[currentScriptPosition + 5], 0x40));
					printf(", ");
					printf(readCharacter(6));
					printf(")");
					currentScriptPosition += 7;
					break;
				}
				case 0x55:
				{
					printf("CHARACTER_LOOK_AT_CHARACTER(");
					printf(readCharacter(1));
					printf(", ");
					printf(readCharacter(2));
					printf(")");
					currentScriptPosition += 3;
					break;
				}
				case 0x56:
				{
					printf("SET_CHARACTER_HEAD_DEFAULT(%s)", readCharacter(1));
					currentScriptPosition += 2;
					break;
				}
				case 0x57:
				{
					printf("opFE57(0x%02X", currentScriptData[currentScriptPosition + 1]);
					printf(", ");
					printf(getValueOrVarU(2));
					printf(", ");
					printf("0x%02X", currentScriptData[currentScriptPosition + 4]);
					printf(") //?");
					currentScriptPosition += 5;
					break;
				}
				case 0x59:
				{
					printf("opFE59()");
					currentScriptPosition += 1;
					break;
				}
				case 0x5A:
				{
					printf("opFE5A(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x5B:
				{
					printf("SETUP_PARTY_SCREEN()");
					currentScriptPosition += 1;
					break;
				}
				case 0x5C:
				{
					printf("opFE5C(");
					printf("0x%02X, ", currentScriptData[currentScriptPosition + 1]);
					printf(readCharacter(2));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(")");
					currentScriptPosition += 5;
					break;
                }
                case 0x5D:
				{
					printf("opFE5D(");
					printf(readCharacter(1));
					printf(", ");
					printf(getValueOrVarU(2));
					printf(")");
					currentScriptPosition += 4;
					break;
				}
				case 0x5E:
				{
					printf("SETUP_ELEMENT_SHOP(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
                }
                case 0x60:
				{
					printf("opFE60(");
					printf(readCharacter(1));
					printf(", ");
					printf(getValueOrVarU(2));
					printf(")");
					currentScriptPosition += 4;
					break;
				}
				case 0x61:
				{
					printf("opFE61()");
					currentScriptPosition += 1;
					break;
				}
				case 0x62:
				{
					printf("SET_CHARACTER_HEAD_SPEED(");
					printf(readCharacter(1));
					printf(", ");
					printf(getValueOrVarU(2));
					printf(")");
					currentScriptPosition += 4;
					break;
				}
				case 0x63:
				{
					printf("opFE63() // stop script if some variable has been set");
					currentScriptPosition += 1;
					break;
				}
				case 0x64:
				{
					printf("opFE64(%d, %s)", currentScriptData[currentScriptPosition + 1], readCharacter(2));
					currentScriptPosition += 3;
					break;
				}
				case 0x65:
				{
					if(currentScriptData[currentScriptPosition + 1] == 0)
					{
						printf("CHANGE_DISC(%d, ", currentScriptData[currentScriptPosition + 1]);
						printf(getValueOrVarU(2));
						printf(")");
						currentScriptPosition += 4;
					}
					else
					{
						printf("CHANGE_DISC(%d", currentScriptData[currentScriptPosition + 1]);
						printf(")");
						currentScriptPosition += 2;
					}
					break;
				}
				case 0x66:
				{
					printf("opFE66()");
					currentScriptPosition += 1;
					break;
				}
				case 0x67:
				{
					printf("opFE67(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x68:
				{
					printf("SETUP_WEAPON_SHOP(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
                }
                case 0x69:
				{
					printf("opFE69(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x6B:
				{
					printf("opFE6B(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
                }
                case 0x6C:
				{
					printf("opFE6C(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x6D:
				{
					printf("opFE6D()");
					currentScriptPosition += 1;
					break;
				}
				case 0x6E:
				{
					printf("%s = opFE6E()", getVarName(read16u(1)));
					currentScriptPosition += 3;
					break;
				}
				case 0x6F:
				{
					printf("opFE6F(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x70:
				{
					printf("opFE70(%s)", readCharacter(1));
					currentScriptPosition += 2;
					break;
				}
				case 0x71:
				{
					printf("USE_ALTERNATE_PORTRAIT(%d)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x72:
				{
					printf("opFE72()");
					currentScriptPosition += 1;
					break;
				}
				case 0x73:
				{
					const char* limitIdAsString = getValueOrVarU(3);

					printf("LEARN_CHARACTER_LIMIT_2(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(limitIdAsString);
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x74:
				{
					const char* limitIdAsString = getValueOrVarU(3);

					printf("LEARN_CHARACTER_LIMIT_1(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(limitIdAsString);
					printf(")");
					currentScriptPosition += 5;
					break;
                }
                case 0x75:  
				{
					printf("opFE75(%s) //? SETUP_GRANDSLAM_SCREEN", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x76:
				{
					printf("%s = opFE76()", getVarName(read16u(1)));
					currentScriptPosition += 3;
					break;
                }
                case 0x77:
				{
					printf("opFE77() //?");
					currentScriptPosition += 3;
					break;
				}
				case 0x78:
				{
					printf("opFE78(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(") // this will stall the script until some condition is met");
					currentScriptPosition += 5;
					break;
				}
				case 0x79:
				{
					const char* limitIdAsString = getValueOrVarU(3);

					printf("LEARN_CHARACTER_LIMIT_0(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(limitIdAsString);
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x7A:
				{
					printf("ADD_FRAME_TO_COLLECTION(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x7B:
				{
					printf("TOGGLE_CHARACTER_LIMIT_MODE(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
                }
                case 0x7C:
                {
                    printf("opFE7C(");
					printf(getValueOrVarU(1));
					printf(", ");
					printf(getValueOrVarU(3));
					printf(") // Operation with two variables");
					currentScriptPosition += 5;
					break;
                }
                case 0x7D:
				{
					printf("opFE7D(%s)", getValueOrVarU(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x7E:
				{
					printf("opFE7E()");
					currentScriptPosition += 1;
					break;
                }
                case 0x80:
				{
					printf("opFE80(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x81:
				{
					printf("opFE81(0x%02X)", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
                }
                case 0x82:
				{
					printf("opFE82(0x%02X) //?", currentScriptData[currentScriptPosition + 1]);
					currentScriptPosition += 2;
					break;
				}
				case 0x84:
				{
					printf("SETUP_RENAME_SCREEN()");
					currentScriptPosition += 1;
					break;
				}
				case 0x85:
				{
					printf("SAVE()");
					currentScriptPosition += 1;
					break;
				}
				default:
				{
					printf("FATAL: Unknown opcode 0xFE 0x%02X\n", subOpcode);
					currentScriptPosition += 1;
					return 4;
					break;
				}
			}
			break;
		}
		default: {
			printf("FATAL: Unknown opcode 0x%02X\n", code);
			currentScriptPosition += 1;
			return 4;
			break;
		}
	}
	int code_length = pos - pre_pos;
	fflush(stdout);
	printf("\n{");
	for (int i = 0; i < code_length; i++) {
		printf("%02X ", script[pre_pos + i]);
	}
	printf("}\n");
	fflush(stdout);
	return ret_code;
}
