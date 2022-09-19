#pragma once

struct CmdLineOptions;

constexpr u16 kMaxLineBreaks = 32;
constexpr u16 kMaxMemBreaks  = 32;

//---------------------------------------------------------------------------------
struct LineBreakpoint
{
  u16 m_Line;
};

//---------------------------------------------------------------------------------
struct MemBreakpoint
{
  u16 m_Address;
};

//---------------------------------------------------------------------------------
using DebugFlags_t = u8;
enum DebugFlags : DebugFlags_t
{
  kDebugFlagsLogInstructions      = 0x01,
  kDebugFlagsDrawInstructionPanel = 0x02,
  kDebugFlagsDrawMemoryPanel      = 0x04,
};

//---------------------------------------------------------------------------------
class Debug
{
public:
  void Init( const CmdLineOptions* cmdline );
  void Draw();

  void DrawInstructionPanel();
  void DrawMemoryPanel();

  DebugFlags_t   m_Flags;
  u8             m_LineBreakCount;
  u8             m_MemBreakCount;
  LineBreakpoint m_LineBreaks[ kMaxLineBreaks ];
  MemBreakpoint  m_MemBreaks [ kMaxMemBreaks  ];

  // panel vars
  static constexpr size_t kMemAddressTextSize = 5;
  char m_MemoryAddressText[ kMemAddressTextSize ];
  int  m_ColumnSelIdx;
  bool m_JumpWithExecution;
};

extern Debug g_Debug;