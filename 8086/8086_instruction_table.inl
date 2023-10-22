#ifndef INST
#define INST(Mnemonic, Encoding, ...) InstructionFormat {OperationType::Mnemonic, Encoding, __VA_ARGS__},
#endif

#ifndef INSTALT
#define INSTALT INST
#endif

#define B_(Bits) {SegmentUsage::Literal, sizeof(#Bits) - 1, 0b##Bits}
#define D_ {SegmentUsage::D, 1, 0}
#define S_ {SegmentUsage::S, 1, 0}
#define W_ {SegmentUsage::W, 1, 0}
#define V_ {SegmentUsage::V, 1, 0}
#define Z_ {SegmentUsage::V, 1, 0}
#define RM_ {SegmentUsage::RM, 3, 0}
#define MOD_ {SegmentUsage::MOD, 2, 0}
#define REG_ {SegmentUsage::REG, 3, 0}
#define SR_ {SegmentUsage::SR, 2, 0}
#define DISP_LO_ {SegmentUsage::DispLo, 8, 0}
#define DISP_HI_ {SegmentUsage::DispHi, 8, 0}
#define IP_CS_LO_ {SegmentUsage::IpCsLo, 8, 0}
#define IP_CS_HI_ {SegmentUsage::IpCsHi, 8, 0}
#define IP_INC_LO_ {SegmentUsage::IpIncLo, 8, 0}
#define IP_INC_HI_ {SegmentUsage::IpIncHi, 8, 0}
#define DATA_ {SegmentUsage::Data, 8, 0}
#define DATA_IF_W_ {SegmentUsage::DataIfW, 8, 0}
#define ADDR_LO_ {SegmentUsage::AddrLo, 8, 0}
#define ADDR_HI_ {SegmentUsage::AddrHi, 8, 0}
#define IP_INC8_ {SegmentUsage::IP_INC8, 8, 0}
#define XXX_ {SegmentUsage::XXX, 3, 0}
#define YYY_ {SegmentUsage::YYY, 3, 0}

#define Imp_W_(Value) {SegmentUsage::W, 0, Value}
#define Imp_S_(Value) {SegmentUsage::S, 0, Value}
#define Imp_REG_(Value) {SegmentUsage::REG, 0, Value}
#define Imp_MOD_(Value) {SegmentUsage::MOD, 0, Value}
#define Imp_RM_(Value) {SegmentUsage::RM, 0, Value}
#define Imp_D_(Value) {SegmentUsage::D, 0, Value}
#define RM_ALWAYS_W_ {SegmentUsage::RM_Always_W, 0, 0}

// TODO: Macros for the imp patterns
INST(MOV,    {B_(100010), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(MOV, {B_(1100011), W_, MOD_, B_(000), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(MOV, {B_(1011), W_, REG_, DATA_, DATA_IF_W_}, {Imp_D_(1)})
INSTALT(MOV, {B_(1010000), W_, ADDR_LO_, ADDR_HI_}, {Imp_D_(1), Imp_REG_(0b000)})
INSTALT(MOV, {B_(1010001), W_, ADDR_LO_, ADDR_HI_}, {Imp_D_(0), Imp_REG_(0b000)})
INSTALT(MOV, {B_(10001110), MOD_, B_(0), SR_, RM_, DISP_LO_, DISP_HI_}, {Imp_D_(1)})
INSTALT(MOV, {B_(10001100), MOD_, B_(0), SR_, RM_, DISP_LO_, DISP_HI_}, {Imp_D_(0)})

INST(PUSH,    {B_(11111111), MOD_, B_(110), RM_, DISP_LO_, DISP_HI_}, {Imp_W_(1)})
INSTALT(PUSH, {B_(01010), REG_}, {Imp_W_(1)})
INSTALT(PUSH, {B_(000), SR_, B_(110)}, {Imp_W_(1)})

INST(POP,    {B_(10001111), MOD_, B_(000), RM_, DISP_LO_, DISP_HI_}, {Imp_W_(1)})
INSTALT(POP, {B_(01011), REG_}, {Imp_W_(1)})
INSTALT(POP, {B_(000), SR_, B_(111)}, {Imp_W_(1)})

INST(XCHG,    {B_(1000011), W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {Imp_D_(1)})
INSTALT(XCHG, {B_(10010), REG_}, {Imp_W_(1), Imp_MOD_(0b11), Imp_RM_(0b00)})

INST(IN,    {B_(1110010), W_, DATA_}, {Imp_REG_(0), Imp_S_(1)})
INSTALT(IN, {B_(1110110), W_}, {Imp_REG_(0), Imp_MOD_(0b11), Imp_RM_(0b10), RM_ALWAYS_W_})

INST(OUT,    {B_(1110011), W_, DATA_}, {Imp_D_(0), Imp_REG_(0), Imp_S_(1)})
INSTALT(OUT, {B_(1110111), W_}, {Imp_D_(0), Imp_REG_(0), Imp_MOD_(0b11), Imp_RM_(0b10), RM_ALWAYS_W_})

INST(XLAT,  {B_(11010111)}, {})
INST(LEA,   {B_(10001101), MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {Imp_D_(1), Imp_W_(1)})
INST(LDS,   {B_(11000101), MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {Imp_D_(1), Imp_W_(1)})
INST(LES,   {B_(11000100), MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {Imp_D_(1), Imp_W_(1)})
INST(LAHF,  {B_(10011111)}, {})
INST(SAHF,  {B_(10011110)}, {})
INST(PUSHF, {B_(10011100)}, {})
INST(POPF,  {B_(10011101)}, {})

INST(ADD,    {B_(000000), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(ADD, {B_(100000), S_, W_, MOD_, B_(000), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(ADD, {B_(0000010), W_, DATA_, DATA_IF_W_}, {Imp_D_(1), Imp_REG_(0b000)})

INST(ADC,    {B_(000100), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(ADC, {B_(100000), S_, W_, MOD_, B_(010), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(ADC, {B_(0001010), W_, DATA_, DATA_IF_W_}, {Imp_D_(1), Imp_REG_(0b000)})

INST(INC,    {B_(1111111), W_, MOD_, B_(000), RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(INC, {B_(01000), REG_}, {Imp_W_(1)})

INST(AAA,    {B_(00110111)}, {})
INST(DAA,    {B_(00100111)}, {})

INST(SUB,    {B_(001010), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(SUB, {B_(100000), S_, W_, MOD_, B_(101), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(SUB, {B_(0010110), W_, DATA_, DATA_IF_W_}, {Imp_D_(1), Imp_REG_(0b000)})

INST(SBB,    {B_(000110), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(SBB, {B_(100000), S_, W_, MOD_, B_(011), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(SBB, {B_(0001110), W_, DATA_, DATA_IF_W_}, {Imp_D_(1), Imp_REG_(0b000)})

INST(DEC,    {B_(1111111), W_, MOD_, B_(001), RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(DEC, {B_(01001), REG_}, {Imp_W_(1)})

INST(NEG,    {B_(1111011), W_, MOD_, B_(011), RM_, DISP_LO_, DISP_HI_}, {})

INST(CMP,    {B_(001110), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(CMP, {B_(100000), S_, W_, MOD_, B_(111), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(CMP, {B_(0011110), W_, DATA_, DATA_IF_W_}, {Imp_D_(1), Imp_REG_(0b000)})

INST(AAS,    {B_(00111111)}, {})
INST(DAS,    {B_(00101111)}, {})
INST(MUL,    {B_(1111011), W_, MOD_, B_(100), RM_, DISP_LO_, DISP_HI_}, {})
INST(IMUL,   {B_(1111011), W_, MOD_, B_(101), RM_, DISP_LO_, DISP_HI_}, {})
INST(AAM,    {B_(11010100), B_(00001010)}, {})
INST(DIV,    {B_(1111011), W_, MOD_, B_(110), RM_, DISP_LO_, DISP_HI_}, {})
INST(IDIV,   {B_(1111011), W_, MOD_, B_(111), RM_, DISP_LO_, DISP_HI_}, {})
INST(AAD,    {B_(11010101), B_(00001010)}, {})
INST(CBW,    {B_(10011000)}, {})
INST(CWD,    {B_(10011001)}, {})

INST(NOT, {B_(1111011), W_, MOD_, B_(010), RM_, DISP_LO_, DISP_HI_}, {})
INST(SHL, {B_(110100), V_, W_, MOD_, B_(100), RM_, DISP_LO_, DISP_HI_}, {Imp_D_(0)})
INST(SHR, {B_(110100), V_, W_, MOD_, B_(101), RM_, DISP_LO_, DISP_HI_}, {Imp_D_(0)})
INST(SAR, {B_(110100), V_, W_, MOD_, B_(111), RM_, DISP_LO_, DISP_HI_}, {Imp_D_(0)})
INST(ROL, {B_(110100), V_, W_, MOD_, B_(000), RM_, DISP_LO_, DISP_HI_}, {Imp_D_(0)})
INST(ROR, {B_(110100), V_, W_, MOD_, B_(001), RM_, DISP_LO_, DISP_HI_}, {Imp_D_(0)})
INST(RCL, {B_(110100), V_, W_, MOD_, B_(010), RM_, DISP_LO_, DISP_HI_}, {Imp_D_(0)})
INST(RCR, {B_(110100), V_, W_, MOD_, B_(011), RM_, DISP_LO_, DISP_HI_}, {Imp_D_(0)})

INST(AND,    {B_(001000), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(AND, {B_(1000000), W_, MOD_, B_(100), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(AND, {B_(0010010), W_, DATA_, DATA_IF_W_}, {Imp_REG_(0)})

INST(TEST,    {B_(100001), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(TEST, {B_(1111011), W_, MOD_, B_(000), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(TEST, {B_(1010100), W_, DATA_, DATA_IF_W_}, {Imp_REG_(0)})

INST(OR,    {B_(000010), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(OR, {B_(1000000), W_, MOD_, B_(001), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(OR, {B_(0000110), W_, DATA_, DATA_IF_W_}, {Imp_REG_(0)})

INST(XOR,    {B_(001100), D_, W_, MOD_, REG_, RM_, DISP_LO_, DISP_HI_}, {})
INSTALT(XOR, {B_(1000000), W_, MOD_, B_(110), RM_, DISP_LO_, DISP_HI_, DATA_, DATA_IF_W_}, {Imp_D_(0)})
INSTALT(XOR, {B_(0011010), W_, DATA_, DATA_IF_W_}, {Imp_REG_(0)})

INST(REP,  {B_(1111001), Z_}, {})
INST(MOVS, {B_(1010010), W_}, {})
INST(CMPS, {B_(1010011), W_}, {})
INST(SCAS, {B_(1010111), W_}, {})
INST(LODS, {B_(1010110), W_}, {})
INST(STOS, {B_(1010101), W_}, {})

INST(CALL,    {B_(11101000), IP_INC_LO_, IP_INC_HI_}, {Imp_W_(1)})
INSTALT(CALL, {B_(11111111), MOD_, B_(010), RM_, DISP_LO_, DISP_HI_}, {Imp_W_(1)})
INSTALT(CALL, {B_(10011010), IP_CS_LO_, IP_CS_HI_}, {Imp_W_(1)})
INSTALT(CALL, {B_(11111111), MOD_, B_(011), RM_, DISP_LO_, DISP_HI_}, {Imp_W_(1)})

INST(JMP,    {B_(11101001), IP_INC_LO_, IP_INC_HI_}, {Imp_W_(1)})
INSTALT(JMP, {B_(11101011), IP_INC8_}, {})
INSTALT(JMP, {B_(11111111), MOD_, B_(100), RM_, DISP_LO_, DISP_HI_}, {Imp_W_(1)})
INSTALT(JMP, {B_(11101010), IP_CS_LO_, IP_CS_HI_}, {Imp_W_(1)})
INSTALT(JMP, {B_(11111111), MOD_, B_(101), DISP_LO_, DISP_HI_}, {Imp_W_(1)})

INST(RET,    {B_(11000011)}, {})
INSTALT(RET, {B_(11000010), DATA_, DATA_IF_W_}, {Imp_W_(0b1)})
INSTALT(RET, {B_(11001011)}, {})
INSTALT(RET, {B_(11001010), DATA_, DATA_IF_W_}, {Imp_W_(0b1)})

INST(JE,  {B_(01110100), IP_INC8_}, {})
INST(JL,  {B_(01111100), IP_INC8_}, {})
INST(JLE, {B_(01111110), IP_INC8_}, {})
INST(JB,  {B_(01110010), IP_INC8_}, {})
INST(JBE, {B_(01110110), IP_INC8_}, {})
INST(JP,  {B_(01111010), IP_INC8_}, {})
INST(JO,  {B_(01110000), IP_INC8_}, {})
INST(JS,  {B_(01111000), IP_INC8_}, {})
INST(JNE, {B_(01110101), IP_INC8_}, {})
INST(JNL, {B_(01111101), IP_INC8_}, {})
INST(JG,  {B_(01111111), IP_INC8_}, {})
INST(JNB, {B_(01110011), IP_INC8_}, {})
INST(JA,  {B_(01110111), IP_INC8_}, {})
INST(JNP, {B_(01111011), IP_INC8_}, {})
INST(JNO, {B_(01110001), IP_INC8_}, {})
INST(JNS, {B_(01111001), IP_INC8_}, {})

INST(LOOP,   {B_(11100010), IP_INC8_}, {})
INST(LOOPZ,  {B_(11100001), IP_INC8_}, {})
INST(LOOPNZ, {B_(11100000), IP_INC8_}, {})
INST(JCXZ,   {B_(11100011), IP_INC8_}, {})

INST(INT,    {B_(11001101), DATA_}, {})
INSTALT(INT, {B_(11001100)}, {})
INST(INTO,   {B_(11001110)}, {})
INST(IRET,   {B_(11001111)}, {})

INST(CLC,     {B_(11111000)}, {})
INST(CMC,     {B_(11110101)}, {})
INST(STC,     {B_(11111001)}, {})
INST(CLD,     {B_(11111100)}, {})
INST(STD,     {B_(11111101)}, {})
INST(CLI,     {B_(11111010)}, {})
INST(STI,     {B_(11111011)}, {})
INST(HLT,     {B_(11110100)}, {})
INST(WAIT,    {B_(10011011)}, {})
INST(ESC,     {B_(11011), XXX_, MOD_, YYY_, DISP_LO_, DISP_HI_}, {})
INST(LOCK,    {B_(11110000)}, {})
INST(SEGMENT, {B_(001), SR_, B_(110)}, {})

#undef INST
#undef INSTALT

#undef B_
#undef D_
#undef S_
#undef W_
#undef V_
#undef RM_
#undef MOD_
#undef REG_
#undef SR_
#undef DISP_LO
#undef DISP_HI
#undef DATA_
#undef DATA_IF_W_
#undef ADDR_LO_
#undef ADDR_HI_
#undef XXX_
#undef YYY_

#undef Imp_W_
#undef Imp_REG_
#undef Imp_MOD_
#undef Imp_RM_
#undef Imp_D_
#undef RM_ALWAYS_W_
