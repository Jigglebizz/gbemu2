#pragma once

#include "Debug.h"

using CmdLineFlags_t = u8;
enum CmdLineFlags : CmdLineFlags_t
{
  kCmdlineFlagsRunTestPattern  = 0x01
};

//---------------------------------------------------------------------------------
struct CmdLineOptions
{
  CmdLineFlags_t m_Flags;
  DebugFlags_t   m_DebugFlags;
  char*          m_CartPath;
  char*          m_DmgPath;
  void*          m_CartContents;
  void*          m_DmgContents;

  u8             m_LineBreakCount;
  u8             m_MemBreakCount;
  LineBreakpoint m_LineBreaks[ kMaxLineBreaks ];
  MemBreakpoint  m_MemBreaks [ kMaxMemBreaks  ];
};

int GetCommandLine( int argc, char** argv, CmdLineOptions* out_cmdline );