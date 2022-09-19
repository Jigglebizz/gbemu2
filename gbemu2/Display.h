#pragma once

#include <SDL.h>

class Display
{
public:
  void Init();
  void Destroy();
  void BeginFrame();
  void DrawScreen();
  void EndFrame();
  void SetPixel( u8 x, u8 y, gb_col_t color );
  void ResizeWindow( int w, int h );
  void SetPalette( u32 col0, u32 col1, u32 col2, u32 col3 );

  void DebugDrawText( const char* text, u32 x, u32 y );
private:
  // 2 bytes per pixel
  // width * height / 4
  static constexpr size_t  kPixelArrSize = (GB_SCREEN_W * GB_SCREEN_H) >> 2;

  SDL_Window*   m_Window;
  SDL_Renderer* m_Renderer;
  SDL_Texture*  m_ScreenTex;
  uint32_t      m_WinW;
  uint32_t      m_WinH;
  u8            m_Pixels [ kPixelArrSize ];
  u32           m_Palette[ kGbColCount ];
};

extern Display g_Display;