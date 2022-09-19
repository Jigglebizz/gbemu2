#include "pch.h"
#include "Display.h"

#include <imgui_sdl.h>

Display g_Display;

//---------------------------------------------------------------------------------
void Display::Init()
{
  if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER ) )
  {
    printf("Failed to init SDL! %s\n", SDL_GetError() );
    return;
  }

  static constexpr int win_x = 100;
  static constexpr int win_y = 100;
  static constexpr int win_w = GB_SCREEN_W * 4;
  static constexpr int win_h = GB_SCREEN_H * 4;

  m_Window = SDL_CreateWindow( "Mike's GbEmu", win_x, win_y, win_w, win_h, SDL_WINDOW_SHOWN );
  if ( m_Window == nullptr )
  {
    printf( "SDL_CreateWindow Error: %s\n", SDL_GetError() );
    Destroy();
    return;
  }

  m_WinW = win_w;
  m_WinH = win_h;

  SDL_SetWindowResizable( m_Window, SDL_TRUE );

  m_Renderer = SDL_CreateRenderer( m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
  if ( m_Renderer == nullptr )
  {
    printf( "SDL_CreateRenderer Error: %s\n", SDL_GetError() );
    Destroy();
    return;
  }

  SDL_Rect clip_rect = {0, 0, (int)m_WinW, (int)m_WinH };
  SDL_RenderSetClipRect( m_Renderer, &clip_rect );

  ImGui::CreateContext();
  ImGuiSDL::Initialize( m_Renderer, m_WinW, m_WinH );

  for ( u16 i_pix = 0; i_pix < kPixelArrSize; ++i_pix )
  {
    m_Pixels[ i_pix ] = GB_COL_PACK( kGbCol0, kGbCol0, kGbCol0, kGbCol0 );
  }

  m_Palette[ 0 ] = PACK_RGB( 0, 0, 0 );
  m_Palette[ 1 ] = PACK_RGB( 85, 85, 85 );
  m_Palette[ 2 ] = PACK_RGB( 170, 170, 170 );
  m_Palette[ 3 ] = PACK_RGB( 255, 255, 255 );
}

//---------------------------------------------------------------------------------
void Display::Destroy()
{
  if ( m_Window )
  {
    SDL_DestroyWindow( m_Window );
  }
  SDL_Quit();
}

static_assert( GB_SCREEN_W % 4 == 0, "screen width must be divisible by 4 - we draw 4 pix at a time horizontally" );

//---------------------------------------------------------------------------------
void Display::BeginFrame()
{
  ImGui::NewFrame();
  SDL_RenderClear(m_Renderer);
  SDL_SetRenderDrawColor(m_Renderer, (u8)50, (u8)50, (u8)50, (u8)256);
  SDL_RenderFillRect(m_Renderer, nullptr);
}

//---------------------------------------------------------------------------------
void Display::DrawScreen()
{
  const float pix_w = (float)m_WinW / (float)GB_SCREEN_W;
  const float pix_h = (float)m_WinH / (float)GB_SCREEN_H;

  for (u8 h = 0; h < GB_SCREEN_H; ++h)
  {
    for ( u8 w = 0; w < GB_SCREEN_W >> 2; ++w )
    {
      u8 pixels = m_Pixels[ h * (GB_SCREEN_W >> 2 ) + w ];

      u8 inter_col0 = GB_COL_UNPACK0( pixels );

      u32 col0 = m_Palette[ GB_COL_UNPACK0( pixels ) ];
      u32 col1 = m_Palette[ GB_COL_UNPACK1( pixels ) ];
      u32 col2 = m_Palette[ GB_COL_UNPACK2( pixels ) ];
      u32 col3 = m_Palette[ GB_COL_UNPACK3( pixels ) ];

      SDL_Rect sdl_pixel{ (int)((w << 2) * pix_w), (int)(h * pix_h), (int)pix_w, (int)pix_h };
      SDL_SetRenderDrawColor( m_Renderer, RGB_R( col0 ), RGB_G( col0 ), RGB_B( col0 ), 255 );
      SDL_RenderFillRect( m_Renderer, &sdl_pixel );

      sdl_pixel.x += (int)pix_w;
      SDL_SetRenderDrawColor( m_Renderer, RGB_R( col1 ), RGB_G( col1 ), RGB_B( col1 ), 255 );
      SDL_RenderFillRect( m_Renderer, &sdl_pixel );

      sdl_pixel.x += (int)pix_w;
      SDL_SetRenderDrawColor( m_Renderer, RGB_R( col2 ), RGB_G( col2 ), RGB_B( col2 ), 255 );
      SDL_RenderFillRect( m_Renderer, &sdl_pixel );

      sdl_pixel.x += (int)pix_w;
      SDL_SetRenderDrawColor( m_Renderer, RGB_R( col3 ), RGB_G( col3 ), RGB_B( col3 ), 255 );
      SDL_RenderFillRect( m_Renderer, &sdl_pixel );
    }
  }
}

//---------------------------------------------------------------------------------
void Display::EndFrame()
{
  ImGui::Render();
  ImGuiSDL::Render( ImGui::GetDrawData() );
  SDL_RenderPresent( m_Renderer );
}

//---------------------------------------------------------------------------------
void Display::SetPixel( u8 x, u8 y, gb_col_t col )
{
  u32 pix_idx = y * (GB_SCREEN_W >> 2) + ( x >> 2 );
  switch ( x % 4 )
  {
    case 0:
      m_Pixels[ pix_idx ] = GB_COL_UPDATE0( m_Pixels[ pix_idx ], col );
      break;
    case 1:
      m_Pixels[ pix_idx ] = GB_COL_UPDATE1( m_Pixels[ pix_idx ], col );
      break;
    case 2:
      m_Pixels[ pix_idx ] = GB_COL_UPDATE2( m_Pixels[ pix_idx ], col );
      break;
    case 3:
      m_Pixels[ pix_idx ] = GB_COL_UPDATE3( m_Pixels[ pix_idx ], col );
      break;
  }
}

//---------------------------------------------------------------------------------
void Display::ResizeWindow( int w, int h )
{
  m_WinW = w;
  m_WinH = h;
  SDL_Rect clip_rect = { 0, 0, (int)m_WinW, (int)m_WinH };
  SDL_RenderSetClipRect(m_Renderer, &clip_rect);

  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize.x = static_cast<float>( w );
  io.DisplaySize.y = static_cast<float>( h );
}

//---------------------------------------------------------------------------------
void Display::SetPalette( u32 col0, u32 col1, u32 col2, u32 col3 )
{
  m_Palette[ 0 ] = col0;
  m_Palette[ 1 ] = col1;
  m_Palette[ 2 ] = col2;
  m_Palette[ 3 ] = col3;
}

//---------------------------------------------------------------------------------
void Display::DebugDrawText( const char* text, u32 x, u32 y )
{
  
}