#pragma once

#include "defs.h"

enum class OperationType
{
    None = 0,
#define INST(Mnemonic, ...) Mnemonic,
#define INSTALT(...)
#include "8086_instruction_table.inl"
    Count,
};

enum class SegmentUsage : u8
{
    End = 0,
    MOD,
    REG,
    RM,
    SR,
    Disp,
    DispLo,
    DispHi,
    Data,
    DataIfW,
    AddrLo,
    AddrHi,
    IpCsLo,
    IpCsHi,
    IpIncLo,
    IpIncHi,
    XXX,
    YYY,
    D,
    S,
    W,
    V,
    Z,
    IP_INC8,
    Literal,
    RM_Always_W,
    Count,
};

struct SegmentBits
{
    SegmentUsage usage;
    u8 bit_count;
    u8 value;
};

enum class RegisterEncoding: u32
{
    None = 0,
    AL,
    AX,
    CL,
    CX,
    DL,
    DX,
    BL,
    BX,
    AH,
    SP,
    CH,
    BP,
    DH,
    SI,
    BH,
    DI,
    Count,
};

enum class SegmentRegisterEncoding: u8 {
    None = 0,
    ES,
    CS,
    SS,
    DS,
    Count,
};

enum class EffectiveAddress: u32
{
    None = 0,
    Direct,
    DirectBP,
    BX_SI,
    BX_DI,
    BP_SI,
    BP_DI,
    SI,
    DI,
    BX,
    Count,
};

enum class InstructionFlag: u32
{
    Wide = (1 << 0)
};

enum class OperandType
{
    None,
    Register,
    SegmentRegister,
    Memory,
    DirectAddress,
    Immediate,
    IpIncrement,
    Rotate,
    Count,
};

struct MemoryAddressPayload
{
    EffectiveAddress base;
    i32 displacement;
    u32 direct_address;
};

struct RegisterPayload
{
    RegisterEncoding encoding;
    u8 offset;
    u8 count;
};

struct IpIncrementPayload {
    u32 inst_size;
    i32 diplacement;
};

struct RotatePayload {
    u32 count;
};

struct SegmentRegisterPayload {
    SegmentRegisterEncoding encoding;
};

struct ImmediatePayload {
    u32 val;
};

struct InstructionOperand
{
    OperandType type;
    union
    {
        MemoryAddressPayload address;
        RegisterPayload reg;
        ImmediatePayload immediate;
        IpIncrementPayload ip_increment;
        RotatePayload rotate;
        SegmentRegisterPayload segment_register;
    };
};

struct Instruction
{
    OperationType op;
    u8 num_bytes;
    InstructionOperand first;
    InstructionOperand second;
    u32 flags;
};

struct InstructionFormat
{
    OperationType op;
    SegmentBits bits[16];
    SegmentBits implicit_bits[16];
};

Instruction decode_instruction(const u8 *bytes, u64 at_byte);
