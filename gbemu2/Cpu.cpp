#include "pch.h"
#include "Cpu.h"

#include "Debug.h"

Cpu g_Cpu;

//---------------------------------------------------------------------------------
void Cpu::Init( void* cart, void* dmg_rom )
{
  memset( &m_Reg, 0, sizeof( m_Reg ) );
  m_Cart.Init( cart );
  m_Bus.Init ( &m_Cart, dmg_rom );
  m_Running = true;

  // If we have no dmg rom, let's skip the bootup check
  if ( dmg_rom == nullptr )
  {
    m_Reg.pc = 0x100;
  }

  m_Reg.h = 1;
}

//---------------------------------------------------------------------------------
void Cpu::Tick()
{
  for ( u8 i_lb = 0; i_lb < g_Debug.m_LineBreakCount; ++i_lb )
  {
    LineBreakpoint* lb = &g_Debug.m_LineBreaks[ i_lb ];
    if ( lb->m_Line == m_Reg.pc )
    {
      __debugbreak();
    }
  }

  u8* instr_ptr = m_Bus[ m_Reg.pc++ ];
  Instruction  instr = kInstructions[ *instr_ptr ];
  m_Instr = instr;

  if ( g_Debug.m_Flags & kDebugFlagsLogInstructions )
  {
    constexpr size_t kMaxInstrNameLen = 16;
    char instr_name[ kMaxInstrNameLen ];
    FullInstName( instr_name, kMaxInstrNameLen, &instr );
    printf( "$%04X : 0x%02X:%s\n", m_Reg.pc - 1, *instr_ptr, instr_name );
  }

  switch ( instr.m_OpType )
  {
  case kNop:
  break;
  case kLd:
  {
    switch ( instr.m_ParamType )
    {
    case kImmToReg:
    {
      const u8 imm_val = *m_Bus[ m_Reg.pc++ ];
      u8*      reg     = GetRegForParam( instr.m_Param1 );
      *reg = imm_val;
    }
    break;
    case kImmToReg16:
    {
      const u16 imm_val = *(u16*)m_Bus[ m_Reg.pc ];
      u16*      reg     = GetRegForParam16( instr.m_Param1 );

      *reg      = imm_val;
      m_Reg.pc += 2;
    }
    break;
    case kRegToAddr:
    {
      const u8 reg_val = *GetRegForParam( instr.m_Param1 );
      u8*      addr    = m_Bus[ *GetRegForParam16( instr.m_Param2 ) ];

      *addr = reg_val;
    }
    break;
    case kRegToReg:
    {
      const u8 reg_val  = *GetRegForParam( instr.m_Param1 );
      u8*      dest_reg =  GetRegForParam( instr.m_Param2 );

      *dest_reg = reg_val;
    }
    break;
    case kRegToReg16:
    {
      const u16 reg_val  = *GetRegForParam16( instr.m_Param1 );
      u16*      dest_reg =  GetRegForParam16( instr.m_Param2 );

      *dest_reg = reg_val;
    }
    break;
    case kRegToImmAddr:
    {
      const u8   reg_val   = *GetRegForParam( instr.m_Param1 );
      const u16  imm_addr  = *(u16*)m_Bus[ m_Reg.pc ];
            u8*  dest_addr =  m_Bus[ imm_addr ];

      *dest_addr = reg_val;
      m_Reg.pc += 2;
    }
    break;
    case kRegToImmAddr16:
    {
      const u16  reg_val   = *GetRegForParam16( instr.m_Param1 );
      const u16  imm_addr  = *(u16*)m_Bus[ m_Reg.pc ];
            u16* dest_addr =  (u16*)m_Bus[ imm_addr ];

      *dest_addr = reg_val;
      m_Reg.pc += 2;
    }
    break;
    case kAddrToReg:
    {
      const u16 addr     = *GetRegForParam16( instr.m_Param1 );
            u8  addr_val = *m_Bus[ addr ];
            u8* dest_reg = GetRegForParam( instr.m_Param2 );

      *dest_reg = addr_val;
    }
    break;
    case kRegToFF00PlusReg:
    {
      const u8  reg_val   = *GetRegForParam( instr.m_Param1 );
      const u8  offset    = *GetRegForParam( instr.m_Param2 );
      const u16 addr      = 0xff00 + offset;
            u8* dest_addr = m_Bus[ addr ];
        
      *dest_addr = reg_val;
    }
    break;
    case kFF00PlusRegToReg:
    {
      const u8  offset   = *GetRegForParam( instr.m_Param1 );
      const u16 addr     = 0xff00 + offset;
            u8* dest_reg =  GetRegForParam( instr.m_Param2 );

      *dest_reg = *m_Bus[ addr ];
    }
    break;
    case kRegToFF00PlusImm:
    {
      const u8  reg_val   = *GetRegForParam( instr.m_Param1 );
      const u8  offset    = *m_Bus[ m_Reg.pc++ ];
      const u16 addr      = 0xff00 + offset;
            u8* dest_addr = m_Bus[ addr ];

      *dest_addr = reg_val;
    }
    break;
    case kFF00PlusImmToReg:
    {
      const u8  offset   = *m_Bus[ m_Reg.pc++ ];
      const u16 addr     = 0xff00 + offset;
            u8* dest_reg = GetRegForParam( instr.m_Param1 );

      *dest_reg = *m_Bus[ addr ];
    }
    break;
    default:
      printf( "LD param type not implemented! %s\n", kParamTypeStrings[ instr.m_ParamType ] );
      m_Running = false;
    }
  }
  break;
  case kInc:
  {
    switch ( instr.m_ParamType )
    {
    case kReg:
    {
      u8* reg  = GetRegForParam( instr.m_Param1 );
      u8  prev = *reg;
      (*reg)++;

      m_Reg.f &= ~kFlagN;
      m_Reg.f = SetFlag( m_Reg.f, kFlagZ, *reg == 0 );
      m_Reg.f = SetFlag( m_Reg.f, kFlagH, ( ( prev ^ (*reg) ) ) & 0x04 );
    }
    break;
    case kReg16:
      (*GetRegForParam16( instr.m_Param1 ))++;
    break;
    case kAddr:
    {
      const u16 addr_ptr = *GetRegForParam16( instr.m_Param1 );
            u8* addr     = m_Bus[ addr_ptr ];
        
      u8 prev = *addr;
      (*addr)++;

      m_Reg.f &= ~kFlagN;

      if ( *addr == 0 )
      {
        m_Reg.f |= kFlagZ;
      }
      else
      {
        m_Reg.f &= ~kFlagZ;
      }

      if ( ( ( prev ^ (*addr) ) ) & 0x04 )
      {
        m_Reg.f |= kFlagH;
      }
      else
      {
        m_Reg.f &= kFlagH;
      }
    }
    break;
    }
  }
  break;
  case kDec:
  {
    switch ( instr.m_OpType )
    {
    case kReg:
    {
      u8* reg  = GetRegForParam( instr.m_Param1 );
      u8  prev = *reg;
      (*reg)--;

      m_Reg.f &= ~kFlagN;

      m_Reg.f = SetFlag( m_Reg.f, kFlagZ, *reg == 0 );
      m_Reg.f = SetFlag( m_Reg.f, kFlagH, ( ( prev ^ (*reg) ) ) & 0x04 );
    }
    break;
    case kReg16:
      (*GetRegForParam16( instr.m_Param1 ))--;
    break;
    case kAddr:
    {
      const u16 addr_ptr = *GetRegForParam16( instr.m_Param1 );
            u8* addr     = m_Bus[ addr_ptr ];
        
      u8 prev = *addr;
      (*addr)--;

      m_Reg.f &= ~kFlagN;

      if ( *addr == 0 )
      {
        m_Reg.f |= kFlagZ;
      }
      else
      {
        m_Reg.f &= ~kFlagZ;
      }

      if ( ( ( prev ^ (*addr) ) ) & 0x04 )
      {
        m_Reg.f |= kFlagH;
      }
      else
      {
        m_Reg.f &= kFlagH;
      }
    }
    break;
    }
  }
  break;
  case kLdInc:
  {
    switch ( instr.m_ParamType )
    {
      case kRegToAddr:
      {
        const u8   reg_val  = *GetRegForParam  ( instr.m_Param1 );
              u16* addr_ptr =  GetRegForParam16( instr.m_Param2 );
              u8*  addr     = m_Bus[ *addr_ptr ];

        *addr = reg_val;
        (*addr_ptr)++;
      }
      break;
      default:
        printf( "Param Type %s not implemented!", kParamTypeStrings[ instr.m_ParamType ] );
        m_Running = false;
    }
    break;
  }
  break;
  case kLdDec:
  {
    switch ( instr.m_ParamType )
    {
      case kRegToAddr:
      {
        const u8   reg_val  = *GetRegForParam  ( instr.m_Param1 );
              u16* addr_ptr =  GetRegForParam16( instr.m_Param2 );
              u8*  addr     = m_Bus[ *addr_ptr ];

        *addr = reg_val;
        (*addr_ptr)--;
      }
      break;
      default:
        printf( "Param Type %s not implemented!", kParamTypeStrings[ instr.m_ParamType ] );
        m_Running = false;
    }
    break;
  }
  case kXor:
  {
    switch ( instr.m_ParamType )
    {
      case kReg:
      {
        m_Reg.a = m_Reg.a ^ *GetRegForParam( instr.m_Param1 );
      }
      break;
      default:
        printf( "XOR param type unimplemented: %s\n", kParamTypeStrings[ instr.m_ParamType ] );
        m_Running = false;
    }
  }
  break;
  case kCompare:
  {
    if ( instr.m_ParamType == kImmToReg )
    {
      const u8 comp = *m_Bus[ m_Reg.pc++ ];
      const u8 prev = m_Reg.a;
      const u8 result = m_Reg.a - comp;

      m_Reg.f = kFlagN;

      m_Reg.f = SetFlag( m_Reg.f, kFlagZ, result == 0 );
      m_Reg.f = SetFlag( m_Reg.f, kFlagH, ( ( prev ^ (result) ) ) & 0x04 );
      m_Reg.f = SetFlag( m_Reg.f, kFlagC, comp > prev );
    }
    else
    {
      printf("Unimplemented comparison parameter type %s", kParamTypeStrings[ instr.m_ParamType ] );
      m_Running = false;
    }
  }
  break;
  case kRl:
  {
    RL( &instr );
  }
  break;
  case kJumpRel:
  {
    i8 rel_imm = *(i8*)m_Bus[ m_Reg.pc++ ];
    bool cond_met = true;
    switch ( instr.m_Cond )
    {
    case kCondC:
      cond_met = ( m_Reg.f & kFlagC );
    break;
    case kCondNC:
      cond_met = ( m_Reg.f & kFlagC ) == 0;
    break;
    case kCondZ:
      cond_met = ( m_Reg.f & kFlagZ );
    break;
    case kCondNZ:
      cond_met = ( m_Reg.f & kFlagZ ) == 0;
    break;
    }

    if ( cond_met )
    {
      m_Reg.pc += rel_imm;
    }
  }
  break;
  case kCall:
  {
    if ( instr.m_Cond == kCondNone )
    {
      const u16 call_to = *(u16*)m_Bus[ m_Reg.pc ];
      m_Reg.pc += 2;
      
      u16* stack_ptr = (u16*)m_Bus[ m_Reg.sp ];
      *stack_ptr = m_Reg.pc;
      m_Reg.sp -= 2;

      m_Reg.pc = call_to;
    }
    else
    {
      printf( "Unsupported call condition %s!\n", kCondStrings[ instr.m_Cond ] );
      m_Running = false;
    }
  }
  break;
  case kRet:
  {
    m_Reg.sp += 2;
    const u16 ret_addr = *(u16*)m_Bus[ m_Reg.sp ];
    m_Reg.pc = ret_addr;
  }
  break;
  case kPop:
  {
    u16* dest_reg = GetRegForParam16( instr.m_Param1 );
    *dest_reg     = *(u16*)m_Bus[ m_Reg.sp ];
    m_Reg.sp += 2;
  }
  break;
  case kPush:
  {
    const u16  reg_val   = *GetRegForParam16( instr.m_Param1 );
          u16* stack_ptr = (u16*)m_Bus[ m_Reg.sp ];

    *stack_ptr = reg_val;
    m_Reg.sp -= 2;
  }
  break;
  case kCbCommand:
  {
    u8* cb_instr_ptr      = m_Bus[ m_Reg.pc++ ];
    Instruction cb_instr = kCbInstructions[ *cb_instr_ptr ];


    if ( g_Debug.m_Flags & kDebugFlagsLogInstructions )
    {
      constexpr size_t kMaxInstrNameLen = 16;
      char instr_name[ kMaxInstrNameLen ];
      FullInstName( instr_name, kMaxInstrNameLen, &cb_instr );
      printf( "$%04X : 0x%02X:%s\n", m_Reg.pc - 1, *instr_ptr, instr_name );
    }

    switch ( cb_instr.m_OpType )
    {
      case kBit:
      {
        if ( cb_instr.m_ParamType == kReg )
        {
          const u8 reg_val = *GetRegForParam( cb_instr.m_Param1 );
          const u8 mask    = 1 << cb_instr.m_Param2;

          m_Reg.f = ( m_Reg.f & kFlagC ) | kFlagH | ( (reg_val & mask) == 0 ? kFlagZ : 0);
        }
        else
        {
          printf("Unimplemented bit operation %s!", kParamTypeStrings[ cb_instr.m_ParamType ] );
        }
      }
      break;
      case kRl:
      {
        RL( &cb_instr );
      }
      break;
      default:
        printf("CB opcode is not implemented\n" );
        m_Running = false;
    }
  }
  break;
  default:
    printf( "Opcode not implemented! %s:0x%02X\n", kOpStrings[ instr.m_OpType ], *instr_ptr );
    m_Running = false;
  }
}

//---------------------------------------------------------------------------------
void Cpu::RL( Instruction* instr )
{
  if ( instr->m_ParamType == kReg )
  {
    u8* reg = GetRegForParam( instr->m_Param1 );

    m_Reg.f  = 0;
    m_Reg.f |= ( (*reg) & 0x80 ) ? kFlagC : 0;
    *reg = *reg << 1;

    m_Reg.f |= (*reg) == 0 ? kFlagZ : 0;
  }
  else
  {
    printf("Unsupported RL param type %s!\n", kParamTypeStrings[ instr->m_ParamType ] );
    m_Running = false;
  }
}

//---------------------------------------------------------------------------------
AddressBus* Cpu::GetBus()
{
  return &m_Bus;
}