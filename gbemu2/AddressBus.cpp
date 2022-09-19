#include "pch.h"
#include "AddressBus.h"
#include "Debug.h"

//---------------------------------------------------------------------------------
void AddressBus::Init( Cart* cart, void* dmg_rom )
{
  m_Cart   = cart;
  m_DmgRom = (u8*)dmg_rom;
}

//---------------------------------------------------------------------------------
u8* AddressBus::operator[]( u16 addr )
{
  // Check memory breakpoints
  for ( u8 i_mb = 0; i_mb < g_Debug.m_MemBreakCount; ++i_mb )
  {
    MemBreakpoint* mb = &g_Debug.m_MemBreaks[ i_mb ];
    if ( mb->m_Address == addr )
    {
      __debugbreak();
    }
  }

  // Only overlay bootrom if $ff50 hasn't been set yet
  if ( addr < 0x100 && *(*this)[0xff50] == 0 )
  {
    return &m_DmgRom[ addr ];
  }

  if ( addr < 0x8000 )
  {
    return (*m_Cart)[ addr ];
  }

  if ( addr < 0xa000 )
  {
    return &m_Vram[ addr - 0x8000 ];
  }

  if ( addr < 0xc000 )
  {
    printf( "Switchable RAM not implemented!\n" );
    return nullptr;
  }

  if ( addr < 0xe000 )
  {
    return &m_Ram[ addr - 0xc000 ];
  }

  if ( addr < 0xfe00 )
  {
    return &m_Ram[ addr - 0xc000 ]; // echo of ram
  }

  if ( addr < 0xfea0 )
  {
    return &m_SpriteAttr[ addr - 0xfe00 ];
  }

  if ( addr < 0xff00 )
  {
    printf( "Attempted to access unusable memory!\n" );
    return nullptr;
  }

  if ( addr < 0xff80 )
  {
    return &m_Io[ addr - 0xff00 ];
  }

  if ( addr < 0xffff )
  {
    return &m_Ram2[ addr - 0xff80 ];
  }

  if ( addr == 0xffff )
  {
    return &m_InterruptReg;
  }

  return nullptr;
}