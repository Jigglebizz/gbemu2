#include "pch.h"

#include "Display.h"
#include "Cpu.h"
#include "Ppu.h"
#include "Debug.h"
#include "Cmdline.h"
#include "imgui/imgui_impl_sdl.h"

bool g_ShouldRun = true;

//---------------------------------------------------------------------------------
void PumpEngine()
{
  SDL_Event sdl_evt;
  ImGuiIO& io = ImGui::GetIO();
  while ( SDL_PollEvent( &sdl_evt ) )
  {
    ImGui_ImplSDL2_ProcessEvent( &sdl_evt );
    switch ( sdl_evt.type )
    {
      case SDL_KEYDOWN:
      break;
      case SDL_KEYUP:
      {
        printf( "key pressed %c\n", sdl_evt.key.keysym.sym );

        if ( sdl_evt.key.keysym.sym == SDLK_i )
        {
          g_Debug.m_Flags ^= kDebugFlagsDrawInstructionPanel;
        }
        if ( sdl_evt.key.keysym.sym == SDLK_m )
        {
          g_Debug.m_Flags ^= kDebugFlagsDrawMemoryPanel;
        }
      }
      break;
      case SDL_WINDOWEVENT:
      {
        if ( sdl_evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
        {
          g_Display.ResizeWindow( sdl_evt.window.data1, sdl_evt.window.data2 );
        }
      }
      break;
      case SDL_MOUSEWHEEL:
      {
        io.MouseWheel = static_cast<float>(sdl_evt.wheel.y);
      }
      break;
      case SDL_QUIT:
        g_ShouldRun = false;
        break;
    }
  }

  int mouse_x, mouse_y;
  const int buttons = SDL_GetMouseState( &mouse_x, &mouse_y );
  io.MousePos = ImVec2( (float)mouse_x, (float)mouse_y );
  io.MouseDown[ 0 ] = buttons & SDL_BUTTON( SDL_BUTTON_LEFT );
  io.MouseDown[ 1 ] = buttons & SDL_BUTTON( SDL_BUTTON_RIGHT );
}

//---------------------------------------------------------------------------------
int main( int argc, char** argv )
{
  g_Display.Init();

  CmdLineOptions cmdline;
  int ok = GetCommandLine( argc, argv, &cmdline );
  if ( ok != 0 )
  {
    return 1;
  }

  g_Debug.Init( &cmdline );

  if ( cmdline.m_CartContents && cmdline.m_DmgContents )
  {
    g_Cpu.Init( cmdline.m_CartContents, cmdline.m_DmgContents );
    g_Ppu.Init( g_Cpu.GetBus(), &g_Display  );
  }

  while ( g_ShouldRun )
  {
    g_Display.BeginFrame();
    PumpEngine();
    
    if ( g_Cpu.ShouldRun() )
    {
      g_Cpu.Tick();
      g_Ppu.Tick();
    }

    if ( cmdline.m_Flags & kCmdlineFlagsRunTestPattern )
    {
      for ( u8 y = 0; y < GB_SCREEN_H; ++y )
      {
        for ( u8 x = 0; x < GB_SCREEN_W; ++x )
        {
          g_Display.SetPixel( x, y, rand() % kGbColCount );
        }
      }

      g_Display.DrawScreen();
    }

    g_Debug.Draw();
    g_Display.EndFrame();
  }

  return 0;
}