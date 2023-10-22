#pragma once

#include <stdio.h>
#include "8086_decode.h"

enum class PrevInstructionContextType {
    None,
    Rep,
    Lock,
    Segment,
};

struct PrevInstructionContext {
    PrevInstructionContextType type;
    union {
        SegmentRegisterPayload segment;
    };
};

PrevInstructionContext print_instruction(Instruction instruction, PrevInstructionContext prev_context, FILE *dest_file);
