#include "pch.h"
#include "Cmdline.h"

//---------------------------------------------------------------------------------
int GetCommandLine( int argc, char** argv, CmdLineOptions* out_cmdline )
{
  memset( out_cmdline, 0, sizeof( *out_cmdline ) );

  // parse cmdline
  for (int i_arg = 0; i_arg < argc; ++i_arg)
  {
    if ( strcmp( argv[ i_arg ], "-test_pattern" ) == 0 )
    {
      out_cmdline->m_Flags |= kCmdlineFlagsRunTestPattern;
    }

    if ( strcmp( argv[ i_arg ], "-log_instructions" ) == 0 )
    {
      out_cmdline->m_DebugFlags |= kDebugFlagsLogInstructions;
    }

    if ( strcmp( argv[ i_arg ], "-cart" ) == 0 && i_arg < argc - 1 )
    {
      i_arg++;
      out_cmdline->m_CartPath = argv[ i_arg ];
    }

    if ( strcmp( argv[ i_arg ], "-dmg" ) == 0 && i_arg < argc - 1 )
    {
      i_arg++;
      out_cmdline->m_DmgPath = argv[ i_arg ];
    }

    if ( ( strcmp( argv[ i_arg ], "-lb" ) == 0 || strcmp( argv[ i_arg ], "-b" ) == 0 ) && i_arg < argc - 1 )
    {
      i_arg++;
      const u16 at = (u16)strtoul( argv[ i_arg ], nullptr, 16 );

      if ( out_cmdline->m_LineBreakCount < kMaxLineBreaks )
      {
        LineBreakpoint* lb = &out_cmdline->m_LineBreaks[ out_cmdline->m_LineBreakCount++ ];
        lb->m_Line = at;
      }
      else
      {
        printf( "Line break supplied at $%04x but max number of line breaks reached!\n", at );
      }
    }

    if ( strcmp( argv[ i_arg ], "-mb" ) == 0 && i_arg < argc - 1 )
    {
      i_arg++;
      const u16 at = (u16)strtoul( argv[ i_arg ], nullptr, 16 );

      if ( out_cmdline->m_MemBreakCount < kMaxMemBreaks )
      {
        MemBreakpoint* mb = &out_cmdline->m_MemBreaks[ out_cmdline->m_MemBreakCount++ ];
        mb->m_Address = at;
      }
      else
      {
        printf( "Line break supplied at $%04x but max number of line breaks reached!\n", at );
      }
    }
  }

  // read cart
  if ( out_cmdline->m_CartPath != nullptr )
  {
    FILE* cart_file;
    errno_t err = fopen_s( &cart_file, out_cmdline->m_CartPath, "rb" );
    if ( cart_file == nullptr )
    {
      printf( "Could not open cart file %s\n", out_cmdline->m_CartPath );
      return 1;
    }

    fseek( cart_file, 0, SEEK_END);
    long cart_size = ftell( cart_file );

    out_cmdline->m_CartContents = malloc( cart_size );
    if ( out_cmdline->m_CartContents == nullptr )
    {
      printf( "Could not allocate memory for cart. Your computer is probably broken.\n" );
      return 1;
    }
    rewind( cart_file );

    fread( out_cmdline->m_CartContents, cart_size, 1, cart_file );
    printf( "Read cart %s!\n", out_cmdline->m_CartPath );
  }

  // read bootrom
  if ( out_cmdline->m_DmgPath != nullptr )
  {
    FILE* dmg_file;
    errno_t err = fopen_s( &dmg_file, out_cmdline->m_DmgPath, "rb" );
    if ( dmg_file == nullptr )
    {
      printf( "Could not open dmg rom %s\n", out_cmdline->m_DmgPath);
      return 1;
    }

    fseek( dmg_file, 0, SEEK_END );
    long dmg_size = ftell( dmg_file );

    out_cmdline->m_DmgContents = malloc( dmg_size );
    if ( out_cmdline->m_DmgContents == nullptr )
    {
      printf( "Could not allocate memory for dmg. Your computer is probably broken.\n" );
      return 1;
    }
    rewind( dmg_file );

    fread( out_cmdline->m_DmgContents, dmg_size, 1, dmg_file );
    printf( "Read DMG %s!\n", out_cmdline->m_DmgPath);
  }

  return 0;
}