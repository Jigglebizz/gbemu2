#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imgui.h>

using u8  = uint8_t;
using i8  =  int8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using gb_col_t = uint8_t;
enum gb_col : gb_col_t
{
  kGbCol0,
  kGbCol1,
  kGbCol2,
  kGbCol3,

  kGbColCount,
  kGbColMask = kGbCol3,
};

#define PACK_RGB( r, g, b ) ( ( (r & 0xff) << 16 ) | ( (g & 0xff) << 8 ) | ( b & 0xff ) )
#define RGB_R( rgb ) ( ( rgb & 0xff0000 ) >> 16 )
#define RGB_G( rgb ) ( ( rgb & 0x00ff00 ) >> 8  )
#define RGB_B( rgb ) ( rgb & 0x0000ff )

#define GB_COL_PACK0( col ) ( ( col & kGbColMask ) << 6 )
#define GB_COL_PACK1( col ) ( ( col & kGbColMask ) << 4 )
#define GB_COL_PACK2( col ) ( ( col & kGbColMask ) << 2 )
#define GB_COL_PACK3( col ) ( col & kGbColMask )

#define GB_COL_UPDATE0( pixels, col ) ( pixels & 0x3f ) | GB_COL_PACK0( col )
#define GB_COL_UPDATE1( pixels, col ) ( pixels & 0xcf ) | GB_COL_PACK1( col )
#define GB_COL_UPDATE2( pixels, col ) ( pixels & 0xf3 ) | GB_COL_PACK2( col )
#define GB_COL_UPDATE3( pixels, col ) ( pixels & 0xfc ) | GB_COL_PACK3( col )

#define GB_COL_PACK( a, b, c, d ) GB_COL_PACK0( a ) | GB_COL_PACK1( b ) | GB_COL_PACK2( c ) | GB_COL_PACK3( d )

#define GB_COL_UNPACK0( packed_cols ) ( ( packed_cols >> 6 ) & kGbColMask )
#define GB_COL_UNPACK1( packed_cols ) ( ( packed_cols >> 4 ) & kGbColMask )
#define GB_COL_UNPACK2( packed_cols ) ( ( packed_cols >> 2 ) & kGbColMask )
#define GB_COL_UNPACK3( packed_cols ) ( packed_cols & kGbColMask )

#define GB_SCREEN_W 160
#define GB_SCREEN_H 144

#define DISABLE_OPTS __pragma(optimize("", off))

inline u8 SetFlag( u8 flags, u8 flag, bool set )
{
  return set ? ( flags | flag ) : ( flags & ~flag );
}