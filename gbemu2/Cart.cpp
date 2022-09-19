#include "pch.h"
#include "Cart.h"

//---------------------------------------------------------------------------------
void Cart::Init( void* cart_contents )
{
  m_Contents = (u8*)cart_contents;
}

//---------------------------------------------------------------------------------
u8* Cart::operator[]( u16 addr )
{
  return &m_Contents[ addr ];
}