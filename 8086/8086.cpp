#include "8086.hpp"
#include "8086_text.h"

#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

void dis_asm_8086(const u8 *bytes, u64 num_bytes, FILE *dest_file)
{
    u64 byte_at = 0;
    PrevInstructionContext prev_context = PrevInstructionContext{PrevInstructionContextType::None, {}};
    while (byte_at < num_bytes)
    {
        Instruction instruction = decode_instruction(bytes, byte_at);
        if (instruction.op != OperationType::None)
        {
            prev_context = print_instruction(instruction, prev_context, dest_file);
            byte_at += instruction.num_bytes;
        }
        else
        {
            printf("ERROR: Unrecognized binary in instruction stream: %c%c%c%c%c%c%c%c", BYTE_TO_BINARY(bytes[byte_at]));
            break;
        }
    }
}
