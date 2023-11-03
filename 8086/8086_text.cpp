#include "8086_decode.h"
#include "8086_text.h"

#include <assert.h>
#include <iostream>
#include <optional>

 char const *mnemonics_table[] =
{
    "",
#define INST(Mnemonic, ...) #Mnemonic,
#define INSTALT(...)
#include "8086_instruction_table.inl"
};

static char const *get_mnemonics(OperationType op_type) {
    char const *result = "";
    if(op_type < OperationType::Count)
    {
        result = mnemonics_table[static_cast<u32>(op_type)];
    }
    return result;
}

void print_register(RegisterEncoding reg_encoding, FILE *dest_file) {
    switch (reg_encoding)
    {
        case RegisterEncoding::AL: {
            fprintf(dest_file, "al");
            break;
        }
        case RegisterEncoding::AX: {
            fprintf(dest_file, "ax");
            break;
        }
        case RegisterEncoding::CL: {
            fprintf(dest_file, "cl");
            break;
        }
        case RegisterEncoding::CX: {
            fprintf(dest_file, "cx");
            break;
        }
        case RegisterEncoding::DL: {
            fprintf(dest_file, "dl");
            break;
        }
        case RegisterEncoding::DX: {
            fprintf(dest_file, "dx");
            break;
        }
        case RegisterEncoding::BL: {
            fprintf(dest_file, "bl");
            break;
        }
        case RegisterEncoding::BX: {
            fprintf(dest_file, "bx");
            break;
        }
        case RegisterEncoding::AH: {
            fprintf(dest_file, "ah");
            break;
        }
        case RegisterEncoding::SP: {
            fprintf(dest_file, "sp");
            break;
        }
        case RegisterEncoding::CH: {
            fprintf(dest_file, "ch");
            break;
        }
        case RegisterEncoding::BP: {
            fprintf(dest_file, "bp");
            break;
        }
        case RegisterEncoding::DH: {
            fprintf(dest_file, "dh");
            break;
        }
        case RegisterEncoding::SI: {
            fprintf(dest_file, "si");
            break;
        }
        case RegisterEncoding::BH: {
            fprintf(dest_file, "bh");
            break;
        }
        case RegisterEncoding::DI: {
            fprintf(dest_file, "di");
            break;
        }
        default:  {
            break;
        }
    }
}

void print_segment_register(SegmentRegisterEncoding encoding, FILE *dest_file) {
    switch (encoding)
    {
    case SegmentRegisterEncoding::ES: {
        fprintf(dest_file, "es");
        break;
    }
    case SegmentRegisterEncoding::CS: {
        fprintf(dest_file, "cs");
        break;
    }
    case SegmentRegisterEncoding::SS: {
        fprintf(dest_file, "ss");
        break;
    }
    case SegmentRegisterEncoding::DS: {
        fprintf(dest_file, "ds");
        break;
    }
    default:
        break;
    }
}
char const* get_effective_address_str(MemoryAddressPayload address) {
    switch (address.base)
    {
        case EffectiveAddress::Direct: {
            return "";
        }
        case EffectiveAddress::DirectBP: {
            return "bp";
        }
        case EffectiveAddress::BX_SI: {
            return "bx + si";
        }
        case EffectiveAddress::BX_DI: {
            return "bx + di";
        }
        case EffectiveAddress::BP_SI: {
            return "bp + si";
        }
        case EffectiveAddress::BP_DI: {
            return "bp + di";
        }
        case EffectiveAddress::SI: {
            return "si";
        }
        case EffectiveAddress::DI: {
            return "di";
        }
        case EffectiveAddress::BX: {
            return "bx";
        }
        default: {
            return "";
        }
    }
}

void print_address(MemoryAddressPayload address, FILE *dest_file) {
    char const* effective_addr = get_effective_address_str(address);
    fprintf(dest_file, "[%s", effective_addr);
    if (address.displacement != 0) {
        assert(address.base != EffectiveAddress::None);
        assert(address.direct_address == 0);
        if (effective_addr != nullptr && effective_addr[0] == '\0') {
            fprintf(dest_file, "%d", address.displacement);
        } else {
            char op = '+';
            if (address.displacement < 0) {
                op = '-';
            }
            fprintf(dest_file, " %c %d", op, abs(address.displacement));
        }
    }
    if (address.direct_address != 0) {
        assert(address.displacement == 0);
        fprintf(dest_file, "%d", address.direct_address);
    }
    fprintf(dest_file, "]");
}

PrevInstructionContext print_instruction(Instruction instruction, PrevInstructionContext prev_context, FILE *dest_file) {
    if (instruction.op == OperationType::SEGMENT) {
        return PrevInstructionContext{PrevInstructionContextType::Segment, {instruction.first.segment_register}};
    }

    InstructionOperand* first = &instruction.first;
    InstructionOperand* second = &instruction.second;
    if (prev_context.type == PrevInstructionContextType::Lock && instruction.op == OperationType::XCHG) {
        first = &instruction.second;
        second = &instruction.first;
    }
    const bool wide = (instruction.flags & static_cast<u32>(InstructionFlag::Wide)) != 0;
    char const* mnemonic = get_mnemonics(instruction.op);
    auto print_operand = [&](const InstructionOperand* op, bool is_first) {
        switch (op->type)
        {
            case OperandType::None: {
                break;
            }
            case OperandType::Register: {
                print_register(op->reg.encoding, dest_file);
                break;
            }
            case OperandType::SegmentRegister: {
                print_segment_register(op->segment_register.encoding, dest_file);
                break;
            }
            case OperandType::Memory: {
                const InstructionOperand* other = is_first ? second : first;
                if (other->type != OperandType::Register) {
                    fprintf(dest_file, "%s ", wide ? "word" : "byte");
                }
                if (prev_context.type == PrevInstructionContextType::Segment) {
                    print_segment_register(prev_context.segment.encoding, dest_file);
                    fprintf(dest_file, ":");
                }
                print_address(op->address, dest_file);
                break;
            }
            case OperandType::DirectAddress: {
                print_address(op->address, dest_file);
                break;
            }
            case OperandType::Immediate: {
                fprintf(dest_file, "%d", op->immediate.val);
                break;
            }
            case OperandType::IpIncrement: {
                // NOTE: Using $ syntax to avoid having to deal with labels
                fprintf(dest_file, "$%+d%+d", op->ip_increment.inst_size, op->ip_increment.diplacement);
                break;
            }
            case OperandType::Rotate: {
                const char* rotate = op->rotate.count == 0 
                    ? "1"
                    : "cl";
                fprintf(dest_file, "%s", rotate);
                break;
            }
            default:  {
                break;
            }
        }
    };
    
    char const* suffix = "";
    if (prev_context.type == PrevInstructionContextType::Rep) {
        suffix = wide ? "W" : "B";
    }
    fprintf(dest_file, "%s%s ", mnemonic, suffix);
    
    if (instruction.op == OperationType::REP) {
        return PrevInstructionContext{PrevInstructionContextType::Rep, {}};
    }
    if (instruction.op == OperationType::LOCK) {
        return PrevInstructionContext{PrevInstructionContextType::Lock, {}};
    }
    
    print_operand(first, true);
    if (first->type != OperandType::None && second->type != OperandType::None) {
        fprintf(dest_file, ", ");
    }
    print_operand(second, false);
    fprintf(dest_file, "\n");
    return {};
}
