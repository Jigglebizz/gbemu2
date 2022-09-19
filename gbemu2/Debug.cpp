#include "pch.h"
#include "Debug.h"

#include "Cmdline.h"
#include "Cpu.h"
#include "Ppu.h"
#include "AddressBus.h"
#include "CpuInstructions.h"

Debug g_Debug;

//---------------------------------------------------------------------------------
void Debug::Init( const CmdLineOptions* cmdline )
{
  m_Flags          = cmdline->m_DebugFlags;
  m_LineBreakCount = cmdline->m_LineBreakCount;
  m_MemBreakCount  = cmdline->m_MemBreakCount;

  memcpy_s( m_LineBreaks, sizeof( m_LineBreaks ), cmdline->m_LineBreaks, sizeof( cmdline->m_LineBreaks ) );
  memcpy_s( m_MemBreaks,  sizeof( m_MemBreaks  ), cmdline->m_MemBreaks,  sizeof( cmdline->m_MemBreaks  ) );

  *(uint32_t*)m_MemoryAddressText = 0;
  m_MemoryAddressText[4] = '\0';

  m_ColumnSelIdx = 2;
  m_JumpWithExecution = false;
}

//---------------------------------------------------------------------------------
void Debug::Draw()
{
  if ( m_Flags & kDebugFlagsDrawInstructionPanel )
  {
    DrawInstructionPanel();
  }

  if (m_Flags & kDebugFlagsDrawMemoryPanel)
  {
    DrawMemoryPanel();
  }
}

//---------------------------------------------------------------------------------
void Debug::DrawInstructionPanel()
{
  u16         pc     = g_Cpu.m_Reg.pc;
  u8          opcode = *g_Cpu.m_Bus[ pc ];
  Instruction instr  = g_Cpu.m_Instr;
  char instr_text[ 32 ];

  FullInstName( instr_text, 32, &instr );
  
  bool open = true;
  ImGui::Begin("Instruction Info", &open );
  ImGui::Text("0x%04x : 0x%02x %s", pc, opcode, instr_text );
  if ( ImGui::Button( g_Cpu.ShouldRun() ? "Pause CPU" : "Play CPU"))
  {
    g_Cpu.SetShouldRun( !g_Cpu.ShouldRun() );
  }

  if ( g_Cpu.ShouldRun() == false )
  {
    ImGui::SameLine();
    if ( ImGui::Button( "Step" ) )
    {
      g_Cpu.Tick();
      g_Ppu.Tick();
    }
  }
  ImGui::End();

  m_Flags &= open ? (DebugFlags_t)-1 : ~kDebugFlagsDrawInstructionPanel;
}

static int FilterHexCharacters( ImGuiInputTextCallbackData* data )
{
  bool is_hex_char =
       (data->EventChar >= '0' && data->EventChar <= '9')
    || (data->EventChar >= 'a' && data->EventChar <= 'f')
    || (data->EventChar >= 'A' && data->EventChar <= 'F');

  return !is_hex_char;
}

//---------------------------------------------------------------------------------
void Debug::DrawMemoryPanel()
{
  bool open = true;
  ImGui::Begin( "Memory", &open );

  ImGui::InputText("Address", m_MemoryAddressText, kMemAddressTextSize, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_CallbackCharFilter, &FilterHexCharacters );
  ImGui::Checkbox( "Jump with execution", &m_JumpWithExecution );

  const char* column_count_strs[] = { "1", "2", "4", "8", "16" };
  const int   column_counts[]     = {  1,   2,   4,   8,   16  };
  ImGui::SetNextItemWidth( 55 );
  if ( ImGui::BeginCombo( "Columns", column_count_strs[ m_ColumnSelIdx ], ImGuiComboFlags_HeightRegular ) )
  {
    for ( int n = 0; n < IM_ARRAYSIZE(column_count_strs); ++n )
    {
      const bool is_selected = ( m_ColumnSelIdx == n );
      if ( ImGui::Selectable( column_count_strs[ n ], is_selected, 0 ))
      {
        m_ColumnSelIdx = n;
      }

      if ( is_selected )
      {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  // Draw the mem table
  uint16_t mem_address = 0;
  if ( *(uint32_t*)m_MemoryAddressText )
  {
    char* end_ptr = m_MemoryAddressText;
    mem_address = strtoul( m_MemoryAddressText, &end_ptr, 16 );
  }

  uint16_t bytes_per_col = column_counts[ m_ColumnSelIdx ];
  int total_columns = bytes_per_col + 2;
  if ( ImGui::BeginTable( "MemTable", total_columns ))
  {
    ImGui::TableSetupColumn( "Addr", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed );

    for ( int row = 0; row < 32; ++row )
    {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex( 0 );
      ImGui::Text( "0x%04x", mem_address + ( (uint32_t)bytes_per_col * row ) );

      for ( uint16_t col = 2; col < (uint16_t)total_columns; ++col )
      {
        ImGui::TableSetColumnIndex( col );
        ImGui::Text( "%02x", *(*g_Cpu.GetBus())[ mem_address + ( col - 2 ) + ( bytes_per_col * (uint16_t)row ) ] );
      }
    }
    ImGui::EndTable();
  }

  m_Flags &= open ? (DebugFlags_t)-1 : ~kDebugFlagsDrawMemoryPanel;
}