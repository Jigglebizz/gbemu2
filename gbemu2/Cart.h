#pragma once

class Cart
{
public:
  void Init( void* cart_contents );
  u8* operator[]( u16 addr );

private:
  u8* m_Contents;
};