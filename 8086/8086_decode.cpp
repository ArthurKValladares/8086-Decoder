#include "8086_decode.h"

#include <iostream>

#define HAS_SEGMENT(usage) has_segment[usage_index(usage)]
#define SEGMENT_BITS(usage) segment_bits[usage_index(usage)]

#define USE_SIGN_EXTENSION (HAS_SEGMENT(SegmentUsage::S) && (SEGMENT_BITS(SegmentUsage::S) == 0b1))

InstructionFormat instruction_formats[] =
{
    #include "8086_instruction_table.inl"
};

u64 byte_offset_for_bit_offset(u64 bit_offset) {
    return bit_offset / 8;
}

u8 offset_inside_byte(u64 bit_offset) {
    return bit_offset % 8;
}

u32 round_up(int num, int multiple)
{
    return ((num + multiple - 1) / multiple) * multiple;
}

u8 n_sized_mask(u8 n) {
    return (1 << n) - 1;
}

u8 get_bits_for_instruction_field(const u8 *bytes, u64 starting_address, u64 bits_processed, SegmentBits instruction_bits) {
    const u64 byte_offset = byte_offset_for_bit_offset(bits_processed);
    const u8 offset_in_byte = offset_inside_byte(bits_processed);
    const u8 shift_right_amount = 8 - (instruction_bits.bit_count + offset_in_byte);
    const u8 byte_to_test = bytes[starting_address + byte_offset];
    return (byte_to_test >> shift_right_amount) & n_sized_mask(instruction_bits.bit_count);
}

constexpr u8 usage_index(SegmentUsage usage) {
    return static_cast<u8>(usage) - 1;
}

RegisterEncoding get_register(u8 reg, u8 w) {
    const bool wide = (w == 0b1);
    switch (reg) {
        case 0b000: {
            return !wide ? RegisterEncoding::AL : RegisterEncoding::AX;
        }
        case 0b001: {
            return !wide ? RegisterEncoding::CL : RegisterEncoding::CX;
        }
        case 0b010: {
            return !wide ? RegisterEncoding::DL : RegisterEncoding::DX;
        }
        case 0b011: {
            return !wide ? RegisterEncoding::BL : RegisterEncoding::BX;
        }
        case 0b100: {
            return !wide ? RegisterEncoding::AH : RegisterEncoding::SP;
        }
        case 0b101: {
            return !wide ? RegisterEncoding::CH : RegisterEncoding::BP;
        }
        case 0b110: {
            return !wide ? RegisterEncoding::DH : RegisterEncoding::SI;
        }
        case 0b111: {
            return !wide ? RegisterEncoding::BH : RegisterEncoding::DI;
        }
        default: {
            return RegisterEncoding::None;
        }
    }
}
 
SegmentRegisterEncoding get_segment_register(u8 sr) {
    switch (sr)
    {
        case 0b00: {
            return SegmentRegisterEncoding::ES;
        }
        case 0b01: {
            return SegmentRegisterEncoding::CS;
        }
        case 0b10: {
            return SegmentRegisterEncoding::SS;
        }
        case 0b11: {
            return SegmentRegisterEncoding::DS;
        }
        default: {
            return SegmentRegisterEncoding::None;
        }
            
    }
}

EffectiveAddress get_rm_encoding(u8 rm, u8 mod) {
    switch (rm) {
        case 0b000: {
            return EffectiveAddress::BX_SI;
        }
        case 0b001: {
            return EffectiveAddress::BX_DI;
        }
        case 0b010: {
            return EffectiveAddress::BP_SI;
        }
        case 0b011: {
            return EffectiveAddress::BP_DI;
        }
        case 0b100: {
            return EffectiveAddress::SI;
        }
        case 0b101: {
            return EffectiveAddress::DI;
        }
        case 0b110: {
            if (mod == 0b00) {
                return EffectiveAddress::Direct;
            } else { 
                return EffectiveAddress::DirectBP;
            }
        }
        case 0b111: {
            return EffectiveAddress::BX;
        }
        default: {
            return EffectiveAddress::None;
        }
    }
}


bool validate_instruction_format(const u8 *bytes, u64 starting_address, const InstructionFormat &inst_format,
                          bool* has_segment, u8* segment_bits, u32& instruction_size) {
    // NOTE: process implicit values first, as some instructions depend on fields set by them
    for (u32 bits_index = 0; bits_index < ArrayCount(inst_format.implicit_bits); ++bits_index) {
        const SegmentBits bits_to_process = inst_format.implicit_bits[bits_index];
        if (bits_to_process.usage == SegmentUsage::End) {
            break;
        } else {
            HAS_SEGMENT(bits_to_process.usage) = true;
            const u8 u_index = usage_index(bits_to_process.usage);
            has_segment[u_index] = true;
            segment_bits[u_index] = bits_to_process.value;
        }
    }
    u32 bits_processed = 0;
    for (u32 bits_index = 0; bits_index < ArrayCount(inst_format.bits); ++bits_index) {
        const SegmentBits bits_to_process = inst_format.bits[bits_index];
        if (bits_to_process.usage == SegmentUsage::End) {
            break;
        }
        HAS_SEGMENT(bits_to_process.usage) = true;
        
        const u8 bits_to_test = get_bits_for_instruction_field(bytes, starting_address, bits_processed, bits_to_process);
        if (bits_to_process.usage == SegmentUsage::Literal) {
            const u8 value_to_test_against = bits_to_process.value;
            if (bits_to_test != value_to_test_against) {
                return false;
            }
        }
        SEGMENT_BITS(bits_to_process.usage) = bits_to_test;
        
        bool instruction_should_be_used = true;
        if (bits_to_process.usage == SegmentUsage::DispLo) {
            assert(HAS_SEGMENT(SegmentUsage::MOD) && HAS_SEGMENT(SegmentUsage::RM));
            const u8 mod  = SEGMENT_BITS(SegmentUsage::MOD);
            const u8 rm   = SEGMENT_BITS(SegmentUsage::RM);
            const bool has_8_bit_disp = (mod == 0b01);
            const bool has_16_bit_disp = ((mod == 0b10) || ((mod == 0b00) && (rm == 0b110)));
            instruction_should_be_used = has_8_bit_disp | has_16_bit_disp;
        }
        if (bits_to_process.usage == SegmentUsage::DispHi) {
            assert(HAS_SEGMENT(SegmentUsage::MOD) && HAS_SEGMENT(SegmentUsage::RM));
            assert(HAS_SEGMENT(SegmentUsage::DispLo));
            const u8 mod  = SEGMENT_BITS(SegmentUsage::MOD);
            const u8 rm   = SEGMENT_BITS(SegmentUsage::RM);
            const bool has_16_bit_disp = (mod == 0b10) || ((mod == 0b00) && (rm == 0b110));
            instruction_should_be_used = has_16_bit_disp;
        }
        if (bits_to_process.usage == SegmentUsage::AddrLo) {
            instruction_should_be_used = true;
        }
        if (bits_to_process.usage == SegmentUsage::AddrHi) {
            assert(HAS_SEGMENT(SegmentUsage::W));
            instruction_should_be_used = (SEGMENT_BITS(SegmentUsage::W) == 0b1);
        }
        if (bits_to_process.usage == SegmentUsage::Data) {
            instruction_should_be_used = true;
        }
        if (bits_to_process.usage == SegmentUsage::DataIfW) {
            assert(HAS_SEGMENT(SegmentUsage::W));
            instruction_should_be_used = (SEGMENT_BITS(SegmentUsage::W) == 0b1) && !USE_SIGN_EXTENSION;
        }
        if (instruction_should_be_used) {
            bits_processed += bits_to_process.bit_count;
        }
    }
    instruction_size = round_up(bits_processed, 8) / 8;
    return true;
}

Instruction try_decode(const u8 *bytes, u64 starting_address, const InstructionFormat &inst_format) {
    Instruction dest_instruction = {};
    
    bool has_segment[usage_index(SegmentUsage::Count)] = {};
    u8 segment_bits[usage_index(SegmentUsage::Count)] = {};
    
    u32 instruction_size = 0;
    const bool is_valid_format = validate_instruction_format(bytes, starting_address, inst_format,
                                                             has_segment, segment_bits, instruction_size);
    if (!is_valid_format) return dest_instruction;
    
    const u8 mod     = SEGMENT_BITS(SegmentUsage::MOD);
    const u8 rm      = SEGMENT_BITS(SegmentUsage::RM);
    const u8 sr      = SEGMENT_BITS(SegmentUsage::SR);
    const u8 reg     = SEGMENT_BITS(SegmentUsage::REG);
    const u8 data    = SEGMENT_BITS(SegmentUsage::Data);
    const u8 data_w  = SEGMENT_BITS(SegmentUsage::DataIfW);
    const u8 disp_lo = SEGMENT_BITS(SegmentUsage::DispLo);
    const u8 disp_hi = SEGMENT_BITS(SegmentUsage::DispHi);
    const u8 addr_lo = SEGMENT_BITS(SegmentUsage::AddrLo);
    const u8 addr_hi = SEGMENT_BITS(SegmentUsage::AddrHi);
    const u8 ip_incr = SEGMENT_BITS(SegmentUsage::IP_INC8);

    const bool word_data           = HAS_SEGMENT(SegmentUsage::W) && (SEGMENT_BITS(SegmentUsage::W) == 0b1);
    const bool has_8_bit_disp      = HAS_SEGMENT(SegmentUsage::DispLo) && (mod == 0b01);
    const bool has_16_bit_disp     = HAS_SEGMENT(SegmentUsage::DispHi) && ((mod == 0b10) || ((mod == 0b00) && (rm == 0b110)));
    const bool source_in_reg_field = HAS_SEGMENT(SegmentUsage::D) && (SEGMENT_BITS(SegmentUsage::D) == 0b0);
    
    dest_instruction.op = inst_format.op;
    dest_instruction.num_bytes = instruction_size;
    if (word_data) {
        dest_instruction.flags |= static_cast<u32>(InstructionFlag::Wide);
    }

    InstructionOperand& inst_reg_op = source_in_reg_field
        ? dest_instruction.second
        : dest_instruction.first;
    InstructionOperand& inst_rm_op = source_in_reg_field
        ? dest_instruction.first
        : dest_instruction.second;
    if (HAS_SEGMENT(SegmentUsage::REG)) {
        inst_reg_op.type = OperandType::Register;
        inst_reg_op.reg.encoding = get_register(reg, word_data);
    }
    if (HAS_SEGMENT(SegmentUsage::SR)) {
        assert(!HAS_SEGMENT(SegmentUsage::REG));
        inst_reg_op.type = OperandType::SegmentRegister;
        inst_reg_op.segment_register.encoding = get_segment_register(sr);
    }
    if (HAS_SEGMENT(SegmentUsage::MOD)) {
        if (mod == 0b11) {
            const bool rm_w = word_data || HAS_SEGMENT(SegmentUsage::RM_Always_W);
            inst_rm_op.type = OperandType::Register;
            inst_rm_op.reg.encoding = get_register(rm, rm_w);
        } else {
            inst_rm_op.type = OperandType::Memory;
            inst_rm_op.address.base = get_rm_encoding(rm, mod);
            if (has_8_bit_disp) {
                inst_rm_op.address.displacement = (i8) disp_lo;
            } else if (has_16_bit_disp) {
                inst_rm_op.address.displacement = (i16) (disp_hi << 8) | disp_lo;
            }
        }
    }
    if (HAS_SEGMENT(SegmentUsage::IP_INC8)) {
        inst_rm_op.type = OperandType::IpIncrement;
        inst_rm_op.ip_increment.inst_size = instruction_size;
        inst_rm_op.ip_increment.diplacement = (i8) ip_incr;
    }
    
    InstructionOperand& unset_operand = (dest_instruction.first.type == OperandType::None)
        ? dest_instruction.first
        : dest_instruction.second;
    if (HAS_SEGMENT(SegmentUsage::Data)) {
        assert(unset_operand.type == OperandType::None);
        unset_operand.type = OperandType::Immediate;
        if (!word_data || USE_SIGN_EXTENSION) {
            unset_operand.immediate.val = (u8) data;
        } else if (HAS_SEGMENT(SegmentUsage::DataIfW) && word_data) {
            unset_operand.immediate.val = (u16) ((data_w << 8) | data);
        }
    }
    if (HAS_SEGMENT(SegmentUsage::AddrLo) || HAS_SEGMENT(SegmentUsage::AddrHi)) {
        assert(unset_operand.type == OperandType::None);
        unset_operand.type = OperandType::DirectAddress;
        if (word_data) {
            unset_operand.address.direct_address = (u16) (addr_hi << 8) | addr_lo;
        } else {
            unset_operand.address.direct_address = (u8) addr_lo;
        }
    }
    if (HAS_SEGMENT(SegmentUsage::V)) {
        unset_operand.type = OperandType::Rotate;
        unset_operand.rotate.count = SEGMENT_BITS(SegmentUsage::V);
    }
    return dest_instruction;
}

Instruction decode_instruction(const u8 *bytes, u64 at_byte)
{
    Instruction result = {};
    for(u32 index = 0; index < ArrayCount(instruction_formats); ++index)
    {
        InstructionFormat inst_format = instruction_formats[index];
        result = try_decode(bytes, at_byte, inst_format);
        if(result.op != OperationType::None)
        {
            break;
        }
    }
    
    return result;
}
