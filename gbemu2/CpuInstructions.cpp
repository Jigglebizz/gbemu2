#include "pch.h"
#include "CpuInstructions.h"

const char* kOpStrings[] =
{
  "None",
  "Nop",
  "Ld",
  "LdInc",
  "LdDec",
  "Inc",
  "Dec",
  "Add",
  "AddC",
  "Sub",
  "SubC",
  "And",
  "Xor",
  "Or",
  "Compare",
  "ComplementA",
  "Rl",
  "Rlc",
  "Rr",
  "Rrc",
  "Jump",
  "JumpRel",
  "JumpCond",
  "Call",
  "Ret",
  "RetInterrupt",
  "Pop",
  "Push",
  "Reset",
  "DecAdj",
  "SetCarryFlag",
  "ClearCarryFlag",
  "ComplementFlag",
  "Stop",
  "Halt",
  "DisableInterrupts",
  "EnableInterrupts",
  "CbCommand",
  "Bit",
};

const char* kParamTypeStrings[] =
{
  "None",
  "Reg",
  "Reg16",
  "Addr",
  "Imm",
  "Imm16",
  "ImmToReg",
  "ImmToReg16",
  "RegToAddr",
  "RegToReg",
  "RegToReg16",
  "RegToImmAddr",
  "RegToImmAddr16",
  "AddrToReg",
  "RegToFF00PlusReg",
  "RegToFF00PlusImm",
  "FF00PlusImmToReg",
  "FF00PlusRegToReg"
};

const char* kParamStrings[] = 
{
  "None",
  "A",
  "B",
  "C",
  "D",
  "E",
  "H",
  "L",
  "AF",
  "BC",
  "DE",
  "HL",
  "SP",
};

const char* kCondStrings[] =
{
  "None",
  "Z",
  "NZ",
  "C",
  "NC",
};

void FullInstName( char* out, size_t len, Instruction* instr )
{
  switch ( instr->m_ParamType )
  {
  case kTypeNone:
    snprintf( out, len, "%s", kOpStrings[ instr->m_OpType] );
    return;
  case kReg:
  case kReg16:
    snprintf( out, len, "%s %s", kOpStrings[ instr->m_OpType ], kParamStrings[ instr->m_Param1 ] );
    if ( instr->m_OpType == kBit )
    {
      snprintf( out, len, "%s %d", out, instr->m_Param2 );
    }
    return;
  case kAddr:
    snprintf( out, len, "%s (%s)", kOpStrings[ instr->m_OpType ], kParamStrings[ instr->m_Param1 ] );
    return;
  case kImm:
  case kImm16:
    snprintf( out, len, "%s %s", kOpStrings[instr->m_OpType], instr->m_OpType == kJumpRel ? kCondStrings[ instr->m_Cond ] : "$imm" );
    return;
  case kImmToReg:
  case kImmToReg16:
    snprintf( out, len, "%s %s, $imm", kOpStrings[instr->m_OpType], kParamStrings[ instr->m_Param1 ] );
    return;
  case kRegToAddr:
    snprintf( out, len, "%s (%s), %s", kOpStrings[instr->m_OpType], kParamStrings[ instr->m_Param2 ], kParamStrings[ instr->m_Param1 ] );
    return;
  case kRegToReg:
  case kRegToReg16:
    snprintf( out, len, "%s %s, %s", kOpStrings[instr->m_OpType], kParamStrings[ instr->m_Param2 ], kParamStrings[ instr->m_Param1 ] );
    return;
  case kRegToImmAddr:
  case kRegToImmAddr16:
    snprintf( out, len, "%s ($imm), %s", kOpStrings[instr->m_OpType], kParamStrings[ instr->m_Param1 ] );
    return;
  case kAddrToReg:
    snprintf( out, len, "%s %s, (%s)", kOpStrings[instr->m_OpType], kParamStrings[ instr->m_Param2 ], kParamStrings[ instr->m_Param1 ] );
    return;
  case kRegToFF00PlusReg:
    snprintf( out, len, "%s ($FF00+%s), %s", kOpStrings[instr->m_OpType], kParamStrings[ instr->m_Param2 ], kParamStrings[ instr->m_Param1 ] );
    return;
  case kRegToFF00PlusImm:
    snprintf( out, len, "%s ($FF00+$imm), %s", kOpStrings[instr->m_OpType], kParamStrings[ instr->m_Param1 ] );
    return;
  case kFF00PlusImmToReg:
    snprintf( out, len, "%s %s, ($FF00+$imm)", kOpStrings[instr->m_OpType], kParamStrings[instr->m_Param1] );
    return;
  case kFF00PlusRegToReg:
    snprintf( out, len, "%s %s, ($FF00+%s)", kOpStrings[instr->m_OpType], kParamStrings[ instr->m_Param2 ], kParamStrings[ instr->m_Param1 ] );
    return;
  }
}