#pragma once

#include "AddressBus.h"
#include "Display.h"

//---------------------------------------------------------------------------------
class Ppu
{
public:
  void Init( AddressBus* bus, Display* display );
  void Tick();
private:
  enum ControlFlags : u8
  {
    kBgEnable           = 0x01,
    kObjEnable          = 0x02,
    kObjSize            = 0x04,
    kBgTileMapSelect    = 0x08,
    kBgTileDataSelect   = 0x10,
    kWindowEnable       = 0x20,
    kWinTileMapSelect   = 0x40,
    kLcdEnable          = 0x80
  };

  AddressBus* m_Bus;
  Display*    m_Display;
};

extern Ppu g_Ppu;