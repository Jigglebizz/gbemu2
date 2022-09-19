#pragma once

enum InstrOpType
{
  kOpNone,
  kNop,
  kLd,
  kLdInc,
  kLdDec,
  kInc,
  kDec,
  kAdd,
  kAddC,
  kSub,
  kSubC,
  kAnd,
  kXor,
  kOr,
  kCompare,
  kComplementA,
  kRl,
  kRlc,
  kRr,
  kRrc,
  kJump,
  kJumpRel,
  kJumpCond,
  kCall,
  kRet,
  kRetInterrupt,
  kPop,
  kPush,
  kReset,
  kDecAdj,
  kSetCarryFlag,
  kClearCarryFlag,
  kComplementFlag,
  kStop,
  kHalt,
  kDisableInterrupts,
  kEnableInterrupts,
  kCbCommand,
  kBit,
};

enum InstrParamType
{
  kTypeNone,
  kReg,
  kReg16,
  kAddr,
  kImm,
  kImm16,
  kImmToReg,
  kImmToReg16,
  kRegToAddr,
  kRegToReg,
  kRegToReg16,
  kRegToImmAddr,
  kRegToImmAddr16,
  kAddrToReg,
  kRegToFF00PlusReg,
  kRegToFF00PlusImm,
  kFF00PlusImmToReg,
  kFF00PlusRegToReg
};

using InstrParam = u16;
enum InstrParamEnum : InstrParam
{
  kParamNone,
  kA,
  kB,
  kC,
  kD,
  kE,
  kH,
  kL,
  kAF,
  kBC,
  kDE,
  kHL,
  kSP,
};

enum InstrCondition
{
  kCondNone,
  kCondZ,
  kCondNZ,
  kCondC,
  kCondNC,
};

//---------------------------------------------------------------------------------
struct Instruction
{
  InstrOpType    m_OpType;
  InstrParamType m_ParamType;
  InstrParam     m_Param1;
  InstrParam     m_Param2;
  InstrCondition m_Cond;
};

//---------------------------------------------------------------------------------
constexpr Instruction kInstructions[0x100] = 
{
  // 0x00
  { kNop },
  { kLd,  kImmToReg16,     kBC },
  { kLd,  kRegToAddr,      kA, kBC },
  { kInc, kReg16,          kBC },
  { kInc, kReg,            kB  },
  { kDec, kReg,            kB  },
  { kLd,  kImmToReg,       kB  },
  { kRlc, kReg,            kA  },
  { kLd,  kRegToImmAddr16, kSP },
  { kAdd, kRegToReg16,     kBC, kHL },
  { kLd,  kAddrToReg,      kBC, kA },
  { kDec, kReg16,          kBC },
  { kInc, kReg,            kC  },
  { kDec, kReg,            kC  },
  { kLd,  kImmToReg,       kC  },
  { kRrc, kReg,            kA  },

  // 0x10
  { kStop },
  { kLd,      kImmToReg16, kDE },
  { kLd,      kRegToAddr,  kA,  kDE },
  { kInc,     kReg16,      kDE },
  { kInc,     kReg,        kD  },
  { kDec,     kReg,        kD  },
  { kLd,      kImmToReg,   kD  },
  { kRl,      kReg,        kA  },
  { kJumpRel, kImm },
  { kAdd,     kRegToReg16, kDE, kHL },
  { kLd,      kAddrToReg,  kDE, kA },
  { kDec,     kReg16,      kDE },
  { kInc,     kReg,        kE  },
  { kDec,     kReg,        kE  },
  { kLd,      kImmToReg,   kE  },
  { kRr,      kReg,        kA  },
  
  // 0x20
  { kJumpRel, kImm, kParamNone, kParamNone, kCondNZ },
  { kLd,      kImmToReg16, kHL  },
  { kLdInc,   kRegToAddr,  kA, kHL },
  { kInc,     kReg16,      kHL  },
  { kInc,     kReg,        kH },
  { kDec,     kReg,        kH },
  { kLd,      kImmToReg,   kH },
  { kDecAdj,  kReg,        kA },
  { kJumpRel, kImm, kParamNone, kParamNone, kCondZ },
  { kAdd,     kRegToReg16, kHL, kHL },
  { kLdInc,   kAddrToReg,  kHL, kA  },
  { kDec,     kReg16,      kHL },
  { kInc,     kReg,        kL  },
  { kDec,     kReg,        kL  },
  { kLd,      kImmToReg,   kL  },
  { kComplementA },

  // 0x30
  { kJumpRel, kImm, kParamNone, kParamNone, kCondNC },
  { kLd,      kImmToReg16, kSP  },
  { kLdDec,   kRegToAddr,  kA, kHL },
  { kInc,     kReg16,      kSP  },
  { kInc,     kAddr,       kHL  },
  { kDec,     kAddr,       kHL  },
  { kLd,      kImmToReg16, kHL  },
  { kSetCarryFlag },
  { kJumpRel, kImm,      kParamNone, kParamNone, kCondC },
  { kAdd,     kRegToReg16, kSP, kHL },
  { kLdDec,   kAddrToReg,  kHL, kA  },
  { kDec,     kReg16,      kSP },
  { kInc,     kReg,        kA  },
  { kDec,     kReg,        kA  },
  { kLd,      kImmToReg,   kA  },
  { kClearCarryFlag },

  // 0x40
  { kLd, kRegToReg,  kB,  kB },
  { kLd, kRegToReg,  kC,  kB },
  { kLd, kRegToReg,  kD,  kB },
  { kLd, kRegToReg,  kE,  kB },
  { kLd, kRegToReg,  kH,  kB },
  { kLd, kRegToReg,  kL,  kB },
  { kLd, kAddrToReg, kHL, kB },
  { kLd, kRegToReg,  kA,  kB },
  { kLd, kRegToReg,  kB,  kC },
  { kLd, kRegToReg,  kC,  kC },
  { kLd, kRegToReg,  kD,  kC },
  { kLd, kRegToReg,  kE,  kC },
  { kLd, kRegToReg,  kH,  kC },
  { kLd, kRegToReg,  kL,  kC },
  { kLd, kAddrToReg, kHL, kC },
  { kLd, kRegToReg,  kA,  kC },
  
  // 0x50
  { kLd, kRegToReg,  kB,  kD },
  { kLd, kRegToReg,  kC,  kD },
  { kLd, kRegToReg,  kD,  kD },
  { kLd, kRegToReg,  kE,  kD },
  { kLd, kRegToReg,  kH,  kD },
  { kLd, kRegToReg,  kL,  kD },
  { kLd, kAddrToReg, kHL, kD },
  { kLd, kRegToReg,  kA,  kD },
  { kLd, kRegToReg,  kB,  kE },
  { kLd, kRegToReg,  kC,  kE },
  { kLd, kRegToReg,  kD,  kE },
  { kLd, kRegToReg,  kE,  kE },
  { kLd, kRegToReg,  kH,  kE },
  { kLd, kRegToReg,  kL,  kE },
  { kLd, kAddrToReg, kHL, kE },
  { kLd, kRegToReg,  kA,  kE },

  // 0x60
  { kLd, kRegToReg,  kB,  kH },
  { kLd, kRegToReg,  kC,  kH },
  { kLd, kRegToReg,  kD,  kH },
  { kLd, kRegToReg,  kE,  kH },
  { kLd, kRegToReg,  kH,  kH },
  { kLd, kRegToReg,  kL,  kH },
  { kLd, kAddrToReg, kHL, kH },
  { kLd, kRegToReg,  kA,  kH },
  { kLd, kRegToReg,  kB,  kL },
  { kLd, kRegToReg,  kC,  kL },
  { kLd, kRegToReg,  kD,  kL },
  { kLd, kRegToReg,  kE,  kL },
  { kLd, kRegToReg,  kH,  kL },
  { kLd, kRegToReg,  kL,  kL },
  { kLd, kAddrToReg, kHL, kL },
  { kLd, kRegToReg,  kA,  kL },

  // 0x70
  { kLd, kRegToAddr, kB,  kHL },
  { kLd, kRegToAddr, kC,  kHL },
  { kLd, kRegToAddr, kD,  kHL },
  { kLd, kRegToAddr, kE,  kHL },
  { kLd, kRegToAddr, kH,  kHL },
  { kLd, kRegToAddr, kL,  kHL },
  { kHalt },
  { kLd, kRegToAddr, kA,  kHL },
  { kLd, kRegToReg,  kB,  kA  },
  { kLd, kRegToReg,  kC,  kA  },
  { kLd, kRegToReg,  kD,  kA  },
  { kLd, kRegToReg,  kE,  kA  },
  { kLd, kRegToReg,  kH,  kA  },
  { kLd, kRegToReg,  kL,  kA  },
  { kLd, kAddrToReg, kHL, kA  },
  { kLd, kRegToReg,  kA,  kA  },

  // 0x80
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x90
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0xa0
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  { kXor, kReg, kA },

  // 0xb0
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0xc0
  {},
  { kPop, kReg16, kBC },
  {},
  {},
  {},
  { kPush, kReg16, kBC },
  {},
  {},
  {},
  { kRet },
  {},
  { kCbCommand },
  {},
  { kCall, kImm16 },
  {},
  {},

  // 0xd0
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0xe0
  { kLd, kRegToFF00PlusImm, kA },
  {},
  { kLd, kRegToFF00PlusReg, kA, kC },
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  { kLd, kRegToImmAddr, kA },
  {},
  {},
  {},
  {},
  {},

  // 0xf0
  { kLd, kFF00PlusImmToReg, kA },
  {},
  { kLd, kFF00PlusRegToReg, kC, kA },
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  { kCompare, kImmToReg, kA },
  {},
};

constexpr Instruction kCbInstructions[] = 
{
  // 0x00
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x10
  {},
  { kRl, kReg, kC },
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x20
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x30
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x40
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x50
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x60
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x70
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  { kBit, kReg, kH, 7 },
  {},
  {},
  {},

  // 0x80
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0x90
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0xa0
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},

  // 0xb0
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
  {},
};

extern const char* kOpStrings[];
extern const char* kParamTypeStrings[];
extern const char* kParamStrings[];
extern const char* kCondStrings[];

void FullInstName( char* out, size_t len, Instruction* inst );