#include "constants.h"

unsigned int opCodeLength[] = {
    0x01,
    0x03
};

unsigned int getOpcodeLength(unsigned int idx)
{
    if(idx < (sizeof(opCodeLength) / sizeof(unsigned int)))
        return opCodeLength[idx];
    return 0;
}