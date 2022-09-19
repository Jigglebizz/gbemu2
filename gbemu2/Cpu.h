#include "AddressBus.h"
#include "Cart.h"
#include "CpuInstructions.h"

//---------------------------------------------------------------------------------
class Cpu
{
friend class Debug;
public:
         void        Init( void* cart, void* dmg_rom );
         void        Tick();

         AddressBus* GetBus();
  inline bool        ShouldRun() const;
  inline void        SetShouldRun( bool should );
private:
  //---------------------------------------------------------------------------------
  using flags_t = u8;
  enum flags : flags_t
  {
    kFlagZ = 1 << 7,
    kFlagN = 1 << 6,
    kFlagH = 1 << 5,
    kFlagC = 1 << 4
  };

  //---------------------------------------------------------------------------------
  struct Registers
  {
    u8 f; u8 a;
    u8 c; u8 b;
    u8 e; u8 d;
    u8 l; u8 h;
    u16 sp;
    u16 pc;

    u16* af() { return (u16*)&f; }
    u16* bc() { return (u16*)&c; }
    u16* de() { return (u16*)&e; }
    u16* hl() { return (u16*)&l; }
  };
  
  //---------------------------------------------------------------------------------
  Registers   m_Reg;
  bool        m_Running;
  Cart        m_Cart;
  AddressBus  m_Bus;

  Instruction m_Instr;

  //---------------------------------------------------------------------------------
  inline u8*  GetRegForParam( InstrParam pt );
  inline u16* GetRegForParam16( InstrParam pt );

  //---------------------------------------------------------------------------------
  void RL( Instruction* instr );
};

extern Cpu g_Cpu;

//---------------------------------------------------------------------------------
bool Cpu::ShouldRun() const
{
  return m_Running;
}

//---------------------------------------------------------------------------------
void Cpu::SetShouldRun( bool should ) 
{
  m_Running = should;
}

//---------------------------------------------------------------------------------
inline u8* Cpu::GetRegForParam( InstrParam pt )
{
  switch ( pt )
  {
    case kA:
      return &m_Reg.a;
    case kB:
      return &m_Reg.b;
    case kC:
      return &m_Reg.c;
    case kD:
      return &m_Reg.d;
    case kE:
      return &m_Reg.e;
    case kH:
      return &m_Reg.h;
    case kL:
      return &m_Reg.l;
  }

  return nullptr;
}

//---------------------------------------------------------------------------------
inline u16* Cpu::GetRegForParam16( InstrParam pt )
{
  switch ( pt )
  {
    case kAF:
      return m_Reg.af();
    case kBC:
      return m_Reg.bc();
    case kDE:
      return m_Reg.de();
    case kHL:
      return m_Reg.hl();
    case kSP:
      return &m_Reg.sp;
  }
  return nullptr;
}
