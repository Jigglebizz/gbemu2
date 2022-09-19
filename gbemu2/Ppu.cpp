#include "pch.h"
#include "ppu.h"

DISABLE_OPTS

//---------------------------------------------------------------------------------
Ppu g_Ppu;

//---------------------------------------------------------------------------------
struct Tile
{
  u8 m_Data[16];
};

//---------------------------------------------------------------------------------
struct TileMap
{
  Tile m_Tiles[ 255 ];
};

//---------------------------------------------------------------------------------
struct BackgroundMap
{
  u8 m_Map[ 32 * 32 ];
};

//---------------------------------------------------------------------------------
struct OamEntry
{
  using OamFlagsType = u8;
  enum OamFlags : OamFlagsType
  {
    kPalette  = 0x10,
    kXFlip    = 0x20,
    kYFlip    = 0x40,
    kPriority = 0x80
  };

  u8           m_PosY;
  u8           m_PosX;
  u8           m_TileNumber;
  OamFlagsType m_Flags;
};

//---------------------------------------------------------------------------------
struct Oam
{
  OamEntry m_Entries[ 40 ];
};

//---------------------------------------------------------------------------------
void Ppu::Init( AddressBus* bus, Display* display )
{
  m_Bus = bus;
  m_Display = display;
}

//---------------------------------------------------------------------------------
void Ppu::Tick()
{
  // check if lcd is enabled
  u8* lcd_control = (*m_Bus)[ 0xff40 ];
  u8* lcd_stat    = (*m_Bus)[ 0xff41 ];

  if ( ( ( *lcd_control ) & kLcdEnable ) == 0 )
  {
    return;
  }

  // now process some pixels
  const u8 scx  = *(*m_Bus)[ 0xff42 ];
  const u8 scy  = *(*m_Bus)[ 0xff43 ];
  const u8 bgp  = *(*m_Bus)[ 0xff47 ];
  const u8 obp0 = *(*m_Bus)[ 0xff48 ];
  const u8 obp1 = *(*m_Bus)[ 0xff49 ];
  const u8 wy   = *(*m_Bus)[ 0xff4a ];
  const u8 wx   = *(*m_Bus)[ 0xff4b ];

  Oam*           sprites  = (Oam*)          (*m_Bus)[ 0xfe00 ];
  TileMap*       tile_map = (TileMap*)      (*m_Bus)[ 0x8000 ];
  BackgroundMap* bg_map   = (BackgroundMap*)(*m_Bus)[ ( (*lcd_control) & kBgTileMapSelect ) ? 0x9c00 : 0x9800 ];

  // BG fetch


  // set vblank period
  u8* yc = (*m_Bus)[ 0xff44 ];
  *yc = 0x90;
}