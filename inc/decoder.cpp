#include "decoder.h"

#define MAKESWITCH( f ) switch_map[f] = &Decoder::op##f

Decoder::Decoder(unsigned char * script_data, int * offset) {
	script = script_data;
	position = offset;
    script_begin = * position;
    output = false;
    ret_code = 0;
    lib = dlopen("./libDic.so", RTLD_NOW);
    if (lib == NULL) {
        cerr << dlerror() << endl;
        exit(-1);
    }
    typedef void (*get)(unsigned char * script_data, int * offset);
    get dinit = (get)dlsym(lib, "init");
    dinit(script, position);
    initMap();
}

void Decoder::initMap() {
    for (int index = 0; index <= 0x5B; ++index) {
        if (index == 0x52) continue;
        if (index == 0x53) continue;
        if (index == 0x57) continue;
        if (index == 0x58) continue;
        char funcName[10];
        sprintf(funcName, "op0x%02X", index);
        switch_map[index] = (intFunc)dlsym(lib, funcName);
    }
    getBuffer = (charFunc)dlsym(lib, "getBuffer");
    buffer = getBuffer();
    return;
}

vector<unsigned int> * Decoder::getJumps() {
    return &jumps;
}

int Decoder::decode() {
	int & pos = * position;
	
	printf("0x%04x: ", pos - script_begin);
	int code = script[pos];	
    int pre_pos = * position;
    int code_length = 0;
    ret_code = 0;

    //code = 0x11;
    //memset(&descBuffer[0], 0, sizeof(descBuffer));
    auto search = switch_map.find(code);
    if (search != switch_map.end()) {
        code_length = search->second();
    }
    else {
        sprintf(buffer, "FATAL: unknown opcode %02X", code);
        code_length = 1;
    }

    printf(buffer);
    //memset(&descBuffer[0], 0, sizeof(descBuffer));

    pos += code_length;
	printf("\n{");
	for (int i = 0; i < code_length; i++) {
		printf("%02X ", script[pre_pos + i]);
	}
	printf("}\n");
	fflush(stdout);
	return ret_code;

    /*
	switch(code)
	{
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
			printf(getVar16u(2));
			printf(", ");
			printf(getVar16u(4));
			printf(", ");
			printf(getVar16u(6));
			printf(")");
			currentScriptPosition += 8;
			break;
		}
		case 0xA2:
		{
			printf("opA2(");
			printf(readCharacter(1));
			printf(", ");
			printf(getVar16u(2));
			printf(")");
			currentScriptPosition += 4;
			break;
		}
		case 0xA3:
		{
			printf("BATTLE_opA3(");
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
			printf(", ");
			printf(getVar16u(5));
			printf(")");
			currentScriptPosition += 7;
			break;
		}
		case 0xA4:
		{
			printf("opA4(%s)", getVar16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xA5:
		{
			// untested
			printf("opA5(%s)", getVar16u(1));
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
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
			printf(", ");
			printf(getVar16u(5));
			printf(", ");
			printf(getVar16u(7));
			printf(")");
			currentScriptPosition += 9;
			break;
		}
		case 0xA8:
		{
			printf("opA8(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getVar16u(2));
			currentScriptPosition += 4;
			break;
		}
		case 0xA9:
		{
			printf("opA9(");
			printf(readCharacter(1));
			printf(", ");
			printf(getVar16u(2));
			printf( ")");
			currentScriptPosition += 4;
			break;
		}
		case 0xAA:
		{
			printf("SET_AXIS_ROTATION(0x%02X, ", currentScriptData[currentScriptPosition + 1]);
			printf(readCharacter(2));
			printf(", ");
			printf(getVar16u(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xAB:
		{
			printf("opAB(");
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xAC:
		{
			printf("opAC(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getVar16u(2));
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
			printf(getVar16s(1, currentScriptData[currentScriptPosition + 5], 0x80));
			printf(", ");
			printf(getVar16s(3, currentScriptData[currentScriptPosition + 5], 0x40));
			printf(", ");
			printf(getVar16u(6));
			printf(")\n");
			currentScriptPosition += 8;
			break;
		}
		case 0xB0:
		{
			printf("opB0(%s)", getVar16s(1, currentScriptData[currentScriptPosition + 3], 0x80));
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
					printf(getVar16u(2));
					printf(")");
                    currentScriptPosition += 4;
                    break;
                }
                default:
                {
                    //assert(0);
                    printf("OVERRIDE_FOLLOW() //? Unrecognized argument, unable to determine length");
                    currentScriptPosition += 2;
                    ret_code = 4;
                    break;
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
			printf("IF_HAVE_ITEM(%s) ELSE JUMP 0x%04X", getVar16u(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xB5:
		{
			printf("opB5(%s)", getVar16u(1));
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
			printf("opB7(%s)", getVar16s(1, currentScriptData[currentScriptPosition + 3], 0x80));
			currentScriptPosition += 4;
			break;
		}
		case 0xB8:
		{
			printf("opB8(%s) //?", getVar16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xB9:
		{
			printf("opB9(%d, ", currentScriptData[currentScriptPosition + 1]);
			printf(readCharacter(2));
			printf(", ");
			printf(getVar16u(3));
			printf(") // missing something");
			currentScriptPosition += 8;
			break;
		}
		case 0xBA:
		{
            int subOpcode = currentScriptData[currentScriptPosition + 1];
            switch(subOpcode) {
                case 0x00: {
                    printf("opBA%02X(", subOpcode);
                    printf(getVar16u(2));
                    printf(", ");
                    printf(getVar16u(4));
                    printf(", ");
                    printf(getVar16u(6));
                    printf(") //? Setup 3 variables");
                    currentScriptPosition += 8;
                    break;
                }
                case 0x01: {
                    printf("opBA%02X(", subOpcode);
                    printf(")");
                    currentScriptPosition += 2;
                    break;
                }
                default: {
                    printf("opBA%02X() //? Unrecognized argument, unable to determine length", subOpcode);
                    currentScriptPosition += 2;
                    ret_code = 4;
                    break;
                }
            }
			break;
		}
		case 0xBB:
		{
			printf("RECEIVE_ITEM(0x%02X, %s)", currentScriptData[currentScriptPosition + 1], getVar16u(2));
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
			printf("RECEIVE_MONEY(%d, %s)", currentScriptData[currentScriptPosition + 1], getVar16u(2));
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
				printf(", %s)", getVar16u(3));
				currentScriptPosition += 5;
			}
			break;
		}
		case 0xC2:
		{
			printf("opC2(");
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
			printf(", ");
			printf(getVar16u(5));
			printf(", ");
			printf(getVar16u(7));
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
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
			printf(", ");
			printf(getVar16u(5));
			printf(")");
			currentScriptPosition += 7;
			break;
		}
		case 0xC6:
		{
			printf("PLAY_SOUNDFX(");

			if(currentScriptData[currentScriptPosition + 1] == 0)
			{
				printf(getVar16u(2));
				printf(", ");
				printf(getVar16u(4));
				currentScriptPosition += 6;
			}
			else
			{
				printf(getVar16u(2));
				printf(", ");
				printf(getVar16u(4));
				printf(", ");
				printf(getVar16u(6));
				printf(", ");
				printf(getVar16u(8));
				currentScriptPosition += 10;
			}
			printf(")");
			break;
		}
		case 0xC7:
		{
			printf("opC7(%s)", getVar16u(1));
			currentScriptPosition += 3;
			break;
		}
		case 0xC8:
		{
			printf("opC8(");
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16s(3, currentScriptData[currentScriptPosition + 9], 0x40));
			printf(", ");
			printf(getVar16s(5, currentScriptData[currentScriptPosition + 9], 0x20));
			printf(", ");
			printf(getVar16s(7, currentScriptData[currentScriptPosition + 9], 0x10));
			printf(", ");
			printf(getVar16u(10));
			printf(", ");
			printf(getVar16u(12));
			printf(")");
			currentScriptPosition += 14;
			break;
		}
		case 0xC9:
		{
			printf("BATTLE_opC9(");
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
			printf(", ");
			printf(getVar16u(5));
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
			printf("opCD(%s) jump 0x%04X", getVar16u(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xCE:
		{
			printf("opCE(%s, 0x%02X)", getVar16u(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xCF:
		{
			printf("opCF(");
			printf(readCharacter(1));
			printf(", ");
			printf(getVar16u(2));
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
			printf("opD1(%s)", getVar16u(1));
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
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
			printf(")");
			currentScriptPosition += 5;
			break;
		}
		case 0xD5:
		{
			printf("var[0x%04X] = NUM_OF_ITEM(%s)", read16u(1), getVar16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xD6:
		{
			printf("opD6(");
			printf(readCharacter(1));
			printf(", ");
			printf(getVar16u(2));
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
			printf(getVar16u(2));
			printf(", ");
			printf(getVar16u(4));
			printf(") jump 0x%04X", read16u(6));
			currentScriptPosition += 8;
			break;
		}
		case 0xD9:
		{
			printf("SETUP_CHARACTER_MODEL(");
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
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
			printf("%s = GET_CHARACTER_ID(%s)", getVarName(read16u(3)), getVar16u(1));
			currentScriptPosition += 5;
			break;
		}
		case 0xDC:
		{
			printf("opDC(");
			printf(getVar16u(1));
			printf(", ");
			printf(getVar16u(3));
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
					printf(getVar16u(2));
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
			printf(getVar16s(1, currentScriptData[currentScriptPosition + 5], 0x80));
			printf(", ");
			printf(getVar16s(3, currentScriptData[currentScriptPosition + 5], 0x40));
			printf(", ");
			printf(getVar16u(6));
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
			printf("if opE5(%s), jump 0x%04X", getVar16u(1), read16u(3));
			currentScriptPosition += 5;
			break;
		}
		case 0xE6:
		{
			printf("if Character(%s) not recruited, jump 0x%04X", getVar16u(1), read16u(3));
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
			printf("if CHECK_FRAME(%s) jump 0x%04X", getVar16u(1), read16u(3));
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
					printf("opFE02(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x03:
				{
					printf("opFE03(");
					printf(getVar16s(2, currentScriptData[currentScriptPosition + 0x4], 0x80));
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
					printf("scriptVar1 = %s", getVar16u(1));
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
					printf(getVar16u(2));
					printf(")");
					currentScriptPosition += 4;
					break;
				}
				case 0x0A:
				{
					printf("opFE0A(");
					printf(readCharacter(1));
					printf(", ");
					printf(getVar16u(2));
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
					printf("opFE0D(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x0E:
				{
					printf("opFE0E(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x0F:
				{
					printf("opFE0F(%s, %s)", readCharacter(1), getVar16u(2));
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
					printf("opFE11(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x12:
				{
					printf("opFE12(");
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(", ");
					printf(getVar16u(5));
					printf(")");
					currentScriptPosition += 7;
					break;
                }
                case 0x13:
				{
					printf("opFE13(");
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(", ");
					printf(getVar16u(5));
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
					printf("opFE15(%s)", getVar16u(1));
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
					printf("SET_CURRENT_FRAME(%s)", getVar16u(1));
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
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x21:
				{
					printf("op21(");
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(", ");
					printf(getVar16u(5));
					printf(")");
					currentScriptPosition += 7;
					break;
				}
				case 0x22:
				{
					printf("CHANGE_FX_VOLUME( ");
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(", ");
					printf(getVar16u(5));
					printf(")");
					currentScriptPosition += 7;
					break;
                }
                case 0x23:
				{
					printf("opFE23( ");
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(", ");
					printf(getVar16u(5));
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
					printf("op29(%s)", getVar16u(1));
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
					printf("opFE2B(%d, %s)", currentScriptData[currentScriptPosition + 1], getVar16u(2));
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
					printf("opFE2D(%s)", getVar16u(1));
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
					printf(getVar16s(1, currentScriptData[currentScriptPosition + 0xB], 0x80));
					printf(", ");
					printf(getVar16s(3, currentScriptData[currentScriptPosition + 0xB], 0x40));
					printf(", ");
					printf(getVar16s(5, currentScriptData[currentScriptPosition + 0xB], 0x20));
					printf(", ");
					printf(getVar16s(7, currentScriptData[currentScriptPosition + 0xB], 0x10));
					printf(", ");
					printf(getVar16s(9, currentScriptData[currentScriptPosition + 0xB], 0x08));
					printf(")");
					currentScriptPosition += 0xC;
					break;
				}
				case 0x35:
				{
					printf("opFE35(%s)", getVar16s(1, currentScriptData[currentScriptPosition + 3], 0x80));
					currentScriptPosition += 4;
					break;
				}
				case 0x36:
				{
					printf("HEAL_GROUP(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x37:
				{
					printf("opFE37(");
					printf("0x%02X", read16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x38:
				{
					printf("OPTIONAL_LIFE(");
					printf(getVar16s(1, currentScriptData[currentScriptPosition + 5], 0x80));
					printf(", ");
					printf(getVar16u(3));
					printf(")");
					currentScriptPosition += 6;
					break;
				}
				case 0x3A:
				{
					printf("opFE3A(");
					printf(getVar16s(1, currentScriptData[currentScriptPosition + 5], 0x80));
					printf(", ");
					printf(getVar16u(3));
					printf(")");
					currentScriptPosition += 6;
					break;
                }
                case 0x3B:
				{
					printf("opFE3B(");
					printf(getVar16u(1));
					printf(", Character(");
					printf(getVar16u(3));
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
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
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
					printf("CURE_ALL_STATUS_AILEMENT(%s", getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
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
					printf("opFE40(%s)", getVar16u(1));
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
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
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
					printf(getVar16s(1, currentScriptData[currentScriptPosition + 7], 0x80));
					printf(", ");
					printf(getVar16s(3, currentScriptData[currentScriptPosition + 7], 0x40));
					printf(", ");
					printf(getVar16s(5, currentScriptData[currentScriptPosition + 7], 0x20));
					printf(")");
					currentScriptPosition += 8;
					break;
				}
				case 0x47:
				{
					printf("opFE47(");
					printf(getVar16s(1, currentScriptData[currentScriptPosition + 7], 0x80));
					printf(", ");
					printf(getVar16s(3, currentScriptData[currentScriptPosition + 7], 0x40));
					printf(", ");
					printf(getVar16s(5, currentScriptData[currentScriptPosition + 7], 0x20));
					printf(")");
					currentScriptPosition += 8;
					break;
				}
				case 0x48:
				{
					printf("opFE48(");
					printf(getVar16u(2));
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
					printf(getVar16s(1, currentScriptData[currentScriptPosition + 5], 0x80));
					printf(", ");
					printf(getVar16s(3, currentScriptData[currentScriptPosition + 5], 0x40));
					printf(")");
					currentScriptPosition += 6;
					break;
				}
				case 0x4A:
				{
					printf("RENAME_CHARACTER(%s)", getVar16u(1));
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
					printf("opFE4F(%d, %s)", currentScriptData[currentScriptPosition + 1], getVar16u(2));
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
					printf("opFE52(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x53:
				{
					printf("SET_CHARACTER_HEAD_ANGLE(");
					printf(getVar16s(1, currentScriptData[currentScriptPosition + 5], 0x80));
					printf(", ");
					printf(getVar16s(3, currentScriptData[currentScriptPosition + 5], 0x40));
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
					printf(getVar16u(2));
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
					printf(getVar16u(3));
					printf(")");
					currentScriptPosition += 5;
					break;
                }
                case 0x5D:
				{
					printf("opFE5D(");
					printf(readCharacter(1));
					printf(", ");
					printf(getVar16u(2));
					printf(")");
					currentScriptPosition += 4;
					break;
				}
				case 0x5E:
				{
					printf("SETUP_ELEMENT_SHOP(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
                }
                case 0x60:
				{
					printf("opFE60(");
					printf(readCharacter(1));
					printf(", ");
					printf(getVar16u(2));
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
					printf(getVar16u(2));
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
						printf(getVar16u(2));
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
					printf("opFE67(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x68:
				{
					printf("SETUP_WEAPON_SHOP(%s)", getVar16u(1));
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
					const char* limitIdAsString = getVar16u(3);

					printf("LEARN_CHARACTER_LIMIT_2(");
					printf(getVar16u(1));
					printf(", ");
					printf(limitIdAsString);
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x74:
				{
					const char* limitIdAsString = getVar16u(3);

					printf("LEARN_CHARACTER_LIMIT_1(");
					printf(getVar16u(1));
					printf(", ");
					printf(limitIdAsString);
					printf(")");
					currentScriptPosition += 5;
					break;
                }
                case 0x75:  
				{
					printf("opFE75(%s) //? SETUP_GRANDSLAM_SCREEN", getVar16u(1));
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
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(") // this will stall the script until some condition is met");
					currentScriptPosition += 5;
					break;
				}
				case 0x79:
				{
					const char* limitIdAsString = getVar16u(3);

					printf("LEARN_CHARACTER_LIMIT_0(");
					printf(getVar16u(1));
					printf(", ");
					printf(limitIdAsString);
					printf(")");
					currentScriptPosition += 5;
					break;
				}
				case 0x7A:
				{
					printf("ADD_FRAME_TO_COLLECTION(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
				}
				case 0x7B:
				{
					printf("TOGGLE_CHARACTER_LIMIT_MODE(%s)", getVar16u(1));
					currentScriptPosition += 3;
					break;
                }
                case 0x7C:
                {
                    printf("opFE7C(");
					printf(getVar16u(1));
					printf(", ");
					printf(getVar16u(3));
					printf(") // Operation with two variables");
					currentScriptPosition += 5;
					break;
                }
                case 0x7D:
				{
					printf("opFE7D(%s)", getVar16u(1));
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
					printf("FATAL: Unknown opcode 0xFE0x%02X\n", subOpcode);
                    currentScriptPosition += 1;
                    ret_code = 4;
					break;
				}
			}
			break;
		}
		default: {
			printf("FATAL: Unknown opcode 0x%02X\n", code);
			currentScriptPosition += 1;
			ret_code = 4;
			break;
		}
	}
    */
}
