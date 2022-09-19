#pragma once

#include "Cart.h"

class AddressBus
{
friend class Debug;
public:
  void Init( Cart* cart, void* dmg_rom );
  u8*  operator[]( u16 addr );

private:
  Cart* m_Cart;
  u8*   m_DmgRom;
  u8    m_Vram      [ 0x2000 ]; // 8kb
  u8    m_Ram       [ 0x2000 ]; // 8kb
  u8    m_SpriteAttr[ 0xa0 ];   // 160b
  u8    m_Io        [ 0x80 ];   // 128b
  u8    m_Ram2      [ 0x80 ];   // 128b
  u8    m_InterruptReg;
};