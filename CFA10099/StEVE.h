/****************************************************************************
StEVE.h

The StEVE class (Static EVE) implements the FT81X interface as a C++ class,
in such a way that various display types can all be handled with the same
code, without resorting too much to preprocessor dependencies. So for
example, this doesn't require you to use #define to set parameters such as
display size.

****************************************************************************/

#pragma once

#include <Arduino.h>
#include <SPI.h>

// Uncomment this to show detailed data about what the code does
#define DBG_TRAFFIC(...) //DBG_GEEK(__VA_ARGS__)

#ifndef STRINGIZE
#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)
#endif

class StEVE
{
public:
    //-----------------------------------------------------------------------
    // Memory locations
    const uint32_t CHIP_ID_ADDRESS              = 0x000C0000UL; // Datasheet 5.2 p.45

    // Memory map, see datasheet section 5 p.40
    const uint32_t RAM_CMD                      = 0x00308000UL;
    const uint32_t RAM_CMD_SIZE                 = 4*1024L;
    const uint32_t RAM_DL                       = 0x00300000UL;
    const uint32_t RAM_DL_SIZE                  = 8*1024L;
    const uint32_t RAM_G                        = 0x00000000UL;
    const uint32_t RAM_G_SIZE                   = 1024*1024L;
    const uint32_t RAM_REG                      = 0x00302000UL;
    const uint32_t RAM_ROMSUB                   = 0x0030A000UL;
    const uint32_t ROM_FONT                     = 0x001E0000UL;
    const uint32_t ROMFONT_TABLEADDRESS         = 0x002FFFFCUL;

    // Registers, see datasheet section 5.1 p.40
    const uint32_t REG_ANA_COMP                 = 0x00302184UL;
    const uint32_t REG_ANALOG                   = 0x0030216CUL;
    const uint32_t REG_BIST_EN                  = 0x00302174UL;
    const uint32_t REG_BUSYBITS                 = 0x003020E8UL;
    const uint32_t REG_CLOCK                    = 0x00302008UL;
    const uint32_t REG_CMD_DL                   = 0x00302100UL;
    const uint32_t REG_CMD_READ                 = 0x003020F8UL;
    const uint32_t REG_CMD_WRITE                = 0x003020FCUL;
    const uint32_t REG_CMDB_SPACE               = 0x00302574UL;
    const uint32_t REG_CMDB_WRITE               = 0x00302578UL;
    const uint32_t REG_CPURESET                 = 0x00302020UL;
    const uint32_t REG_CRC                      = 0x00302178UL;
    const uint32_t REG_CSPREAD                  = 0x00302068UL;
    const uint32_t REG_CTOUCH_EXTENDED          = 0x00302108UL;
    const uint32_t REG_CTOUCH_TOUCH0_XY         = 0x00302124UL;
    const uint32_t REG_CTOUCH_TOUCH1_XY         = 0x0030211CUL;
    const uint32_t REG_CTOUCH_TOUCH2_XY         = 0x0030218CUL;
    const uint32_t REG_CTOUCH_TOUCH3_XY         = 0x00302190UL;
    const uint32_t REG_CTOUCH_TOUCH4_X          = 0x0030216CUL;
    const uint32_t REG_CTOUCH_TOUCH4_Y          = 0x00302120UL;
    const uint32_t REG_CYA_TOUCH                = 0x00302168UL;
    const uint32_t REG_DATESTAMP                = 0x00302564UL;
    const uint32_t REG_DITHER                   = 0x00302060UL;
    const uint32_t REG_DLSWAP                   = 0x00302054UL;
    const uint32_t REG_FRAMES                   = 0x00302004UL;
    const uint32_t REG_FREQUENCY                = 0x0030200CUL;
    const uint32_t REG_GPIO                     = 0x00302094UL;
    const uint32_t REG_GPIO_DIR                 = 0x00302090UL;
    const uint32_t REG_GPIOX                    = 0x0030209CUL;
    const uint32_t REG_GPIOX_DIR                = 0x00302098UL;
    const uint32_t REG_HCYCLE                   = 0x0030202CUL;
    const uint32_t REG_HOFFSET                  = 0x00302030UL;
    const uint32_t REG_HSIZE                    = 0x00302034UL;
    const uint32_t REG_HSYNC0                   = 0x00302038UL;
    const uint32_t REG_HSYNC1                   = 0x0030203CUL;
    const uint32_t REG_ID                       = 0x00302000UL;
    const uint32_t REG_INT_EN                   = 0x003020ACUL;
    const uint32_t REG_INT_FLAGS                = 0x003020A8UL;
    const uint32_t REG_INT_MASK                 = 0x003020B0UL;
    const uint32_t REG_MACRO_0                  = 0x003020D8UL;
    const uint32_t REG_MACRO_1                  = 0x003020DCUL;
    const uint32_t REG_MEDIAFIFO_READ           = 0x00309014UL;
    const uint32_t REG_MEDIAFIFO_WRITE          = 0x00309018UL;
    const uint32_t REG_OUTBITS                  = 0x0030205CUL;
    const uint32_t REG_PATCHED_ANALOG           = 0x00302170UL;
    const uint32_t REG_PATCHED_TOUCH_FAULT      = 0x0030216CUL;
    const uint32_t REG_PCLK                     = 0x00302070UL;
    const uint32_t REG_PCLK_POL                 = 0x0030206CUL;
    const uint32_t REG_PLAY                     = 0x0030208CUL;
    const uint32_t REG_PLAYBACK_FORMAT          = 0x003020C4UL;
    const uint32_t REG_PLAYBACK_FREQ            = 0x003020C0UL;
    const uint32_t REG_PLAYBACK_LENGTH          = 0x003020B8UL;
    const uint32_t REG_PLAYBACK_LOOP            = 0x003020C8UL;
    const uint32_t REG_PLAYBACK_PLAY            = 0x003020CCUL;
    const uint32_t REG_PLAYBACK_READPTR         = 0x003020BCUL;
    const uint32_t REG_PLAYBACK_START           = 0x003020B4UL;
    const uint32_t REG_PWM_DUTY                 = 0x003020D4UL;
    const uint32_t REG_PWM_HZ                   = 0x003020D0UL;
    const uint32_t REG_RENDERMODE               = 0x00302010UL;
    const uint32_t REG_ROMSUB_SEL               = 0x003020F0UL;
    const uint32_t REG_ROTATE                   = 0x00302058UL;
    const uint32_t REG_SNAPFORMAT               = 0x0030201CUL;
    const uint32_t REG_SNAPSHOT                 = 0x00302018UL;
    const uint32_t REG_SNAPY                    = 0x00302014UL;
    const uint32_t REG_SOUND                    = 0x00302088UL;
    const uint32_t REG_SPI_EARLY_TX             = 0x0030217CUL;
    const uint32_t REG_SPI_WIDTH                = 0x00302188UL;
    const uint32_t REG_SWIZZLE                  = 0x00302064UL;
    const uint32_t REG_TAG                      = 0x0030207CUL;
    const uint32_t REG_TAG_X                    = 0x00302074UL;
    const uint32_t REG_TAG_Y                    = 0x00302078UL;
    const uint32_t REG_TAP_CRC                  = 0x00302024UL;
    const uint32_t REG_TAP_MASK                 = 0x00302028UL;
    const uint32_t REG_TOUCH_ADC_MODE           = 0x00302108UL;
    const uint32_t REG_TOUCH_CHARGE             = 0x0030210CUL;
    const uint32_t REG_TOUCH_DIRECT_XY          = 0x0030218CUL;
    const uint32_t REG_TOUCH_DIRECT_Z1Z2        = 0x00302190UL;
    const uint32_t REG_TOUCH_FAULT              = 0x00302170UL;
    const uint32_t REG_TOUCH_MODE               = 0x00302104UL;
    const uint32_t REG_TOUCH_OVERSAMPLE         = 0x00302114UL;
    const uint32_t REG_TOUCH_RAW_XY             = 0x0030211CUL;
    const uint32_t REG_TOUCH_RZ                 = 0x00302120UL;
    const uint32_t REG_TOUCH_RZTHRESH           = 0x00302118UL;
    const uint32_t REG_TOUCH_SCREEN_XY          = 0x00302124UL;
    const uint32_t REG_TOUCH_SETTLE             = 0x00302110UL;
    const uint32_t REG_TOUCH_TAG                = 0x0030212CUL;
    const uint32_t REG_TOUCH_TAG_XY             = 0x00302128UL;
    const uint32_t REG_TOUCH_TAG1               = 0x00302134UL;
    const uint32_t REG_TOUCH_TAG1_XY            = 0x00302130UL;
    const uint32_t REG_TOUCH_TAG2               = 0x0030213CUL;
    const uint32_t REG_TOUCH_TAG2_XY            = 0x00302138UL;
    const uint32_t REG_TOUCH_TAG3               = 0x00302144UL;
    const uint32_t REG_TOUCH_TAG3_XY            = 0x00302140UL;
    const uint32_t REG_TOUCH_TAG4               = 0x0030214CUL;
    const uint32_t REG_TOUCH_TAG4_XY            = 0x00302148UL;
    const uint32_t REG_TOUCH_TRANSFORM_A        = 0x00302150UL;
    const uint32_t REG_TOUCH_TRANSFORM_B        = 0x00302154UL;
    const uint32_t REG_TOUCH_TRANSFORM_C        = 0x00302158UL;
    const uint32_t REG_TOUCH_TRANSFORM_D        = 0x0030215CUL;
    const uint32_t REG_TOUCH_TRANSFORM_E        = 0x00302160UL;
    const uint32_t REG_TOUCH_TRANSFORM_F        = 0x00302164UL;
    const uint32_t REG_TRACKER                  = 0x00309000UL;
    const uint32_t REG_TRACKER_1                = 0x00309004UL;
    const uint32_t REG_TRACKER_2                = 0x00309008UL;
    const uint32_t REG_TRACKER_3                = 0x0030900CUL;
    const uint32_t REG_TRACKER_4                = 0x00309010UL;
    const uint32_t REG_TRIM                     = 0x00302180UL;
    const uint32_t REG_VCYCLE                   = 0x00302040UL;
    const uint32_t REG_VOFFSET                  = 0x00302044UL;
    const uint32_t REG_VOL_PB                   = 0x00302080UL;
    const uint32_t REG_VOL_SOUND                = 0x00302084UL;
    const uint32_t REG_VSIZE                    = 0x00302048UL;
    const uint32_t REG_VSYNC0                   = 0x0030204CUL;
    const uint32_t REG_VSYNC1                   = 0x00302050UL;

    // Display list commands (ProgGuide ch.4)
    typedef enum
    {
        // Display List commands
        ENC_CMD_DISPLAY                 = 0x00000000,
        ENC_CMD_BITMAP_SOURCE           = 0x01000000,
        ENC_CMD_CLEAR_COLOR_RGB         = 0x02000000,
        ENC_CMD_CLEAR_COLOR             = 0x02000000, // Alias with 24-bit RGB value
        ENC_CMD_TAG                     = 0x03000000,
        ENC_CMD_COLOR_RGB               = 0x04000000,
        ENC_CMD_COLOR                   = 0x04000000, // Alias with 24-bit RGB value
        ENC_CMD_BITMAP_HANDLE           = 0x05000000,
        ENC_CMD_CELL                    = 0x06000000,
        ENC_CMD_BITMAP_LAYOUT           = 0x07000000,
        ENC_CMD_BITMAP_SIZE             = 0x08000000,
        ENC_CMD_ALPHA_FUNC              = 0x09000000,
        ENC_CMD_STENCIL_FUNC            = 0x0A000000,
        ENC_CMD_BLEND_FUNC              = 0x0B000000,
        ENC_CMD_STENCIL_OP              = 0x0C000000,
        ENC_CMD_POINT_SIZE              = 0x0D000000,
        ENC_CMD_LINE_WIDTH              = 0x0E000000,
        ENC_CMD_CLEAR_COLOR_A           = 0x0F000000,
        ENC_CMD_COLOR_A                 = 0x10000000,
        ENC_CMD_CLEAR_STENCIL           = 0x11000000,
        ENC_CMD_CLEAR_TAG               = 0x12000000,
        ENC_CMD_STENCIL_MASK            = 0x13000000,
        ENC_CMD_TAG_MASK                = 0x14000000,
        ENC_CMD_BITMAP_TRANSFORM_A      = 0x15000000,
        ENC_CMD_BITMAP_TRANSFORM_B      = 0x16000000,
        ENC_CMD_BITMAP_TRANSFORM_C      = 0x17000000,
        ENC_CMD_BITMAP_TRANSFORM_D      = 0x18000000,
        ENC_CMD_BITMAP_TRANSFORM_E      = 0x19000000,
        ENC_CMD_BITMAP_TRANSFORM_F      = 0x1A000000,
        ENC_CMD_SCISSOR_XY              = 0x1B000000,
        ENC_CMD_SCISSOR_SIZE            = 0x1C000000,
        ENC_CMD_CALL                    = 0x1D000000,
        ENC_CMD_JUMP                    = 0x1E000000,
        ENC_CMD_BEGIN                   = 0x1F000000,
        ENC_CMD_COLOR_MASK              = 0x20000000,
        ENC_CMD_END                     = 0x21000000,
        ENC_CMD_SAVE_CONTEXT            = 0x22000000,
        ENC_CMD_RESTORE_CONTEXT         = 0x23000000,
        ENC_CMD_RETURN                  = 0x24000000,
        ENC_CMD_MACRO                   = 0x25000000,
        ENC_CMD_CLEAR                   = 0x26000000,
        ENC_CMD_VERTEX_FORMAT           = 0x27000000,
        ENC_CMD_BITMAP_LAYOUT_H         = 0x28000000,
        ENC_CMD_BITMAP_SIZE_H           = 0x29000000,
        ENC_CMD_PALETTE_SOURCE          = 0x2A000000,
        ENC_CMD_VERTEX_TRANSLATE_X      = 0x2B000000,
        ENC_CMD_VERTEX_TRANSLATE_Y      = 0x2C000000,
        ENC_CMD_NOP                     = 0x2D000000,
        ENC_CMD_VERTEX2F                = 0x40000000, // Encoded in ProgGuide as 1<<22
        ENC_CMD_VERTEX2II               = 0x80000000, // Encoded in ProgGuide as 2<<22

        // Co-processor commands
        ENC_CMD_DLSTART                 = 0xFFFFFF00,
        ENC_CMD_SWAP                    = 0xFFFFFF01,
        ENC_CMD_INTERRUPT               = 0xFFFFFF02,
        ENC_CMD_BGCOLOR                 = 0xFFFFFF09,
        ENC_CMD_FGCOLOR                 = 0xFFFFFF0A,
        ENC_CMD_GRADIENT                = 0xFFFFFF0B,
        ENC_CMD_TEXT                    = 0xFFFFFF0C,
        ENC_CMD_BUTTON                  = 0xFFFFFF0D,
        ENC_CMD_KEYS                    = 0xFFFFFF0E,
        ENC_CMD_PROGRESS                = 0xFFFFFF0F,
        ENC_CMD_SLIDER                  = 0xFFFFFF10,
        ENC_CMD_SCROLLBAR               = 0xFFFFFF11,
        ENC_CMD_TOGGLE                  = 0xFFFFFF12,
        ENC_CMD_GAUGE                   = 0xFFFFFF13,
        ENC_CMD_CLOCK                   = 0xFFFFFF14,
        ENC_CMD_CALIBRATE               = 0xFFFFFF15,
        ENC_CMD_SPINNER                 = 0xFFFFFF16,
        ENC_CMD_STOP                    = 0xFFFFFF17,
        ENC_CMD_MEMCRC                  = 0xFFFFFF18,
        ENC_CMD_REGREAD                 = 0xFFFFFF19,
        ENC_CMD_MEMWRITE                = 0xFFFFFF1A,
        ENC_CMD_MEMSET                  = 0xFFFFFF1B,
        ENC_CMD_MEMZERO                 = 0xFFFFFF1C,
        ENC_CMD_MEMCPY                  = 0xFFFFFF1D,
        ENC_CMD_APPEND                  = 0xFFFFFF1E,
        ENC_CMD_SNAPSHOT                = 0xFFFFFF1F,
        ENC_CMD_INFLATE                 = 0xFFFFFF22,
        ENC_CMD_GETPTR                  = 0xFFFFFF23,
        ENC_CMD_LOADIMAGE               = 0xFFFFFF24,
        ENC_CMD_GETPROPS                = 0xFFFFFF25,
        ENC_CMD_LOADIDENTITY            = 0xFFFFFF26,
        ENC_CMD_TRANSLATE               = 0xFFFFFF27,
        ENC_CMD_SCALE                   = 0xFFFFFF28,
        ENC_CMD_ROTATE                  = 0xFFFFFF29,
        ENC_CMD_SETMATRIX               = 0xFFFFFF2A,
        ENC_CMD_SETFONT                 = 0xFFFFFF2B,
        ENC_CMD_TRACK                   = 0xFFFFFF2C,
        ENC_CMD_DIAL                    = 0xFFFFFF2D,
        ENC_CMD_NUMBER                  = 0xFFFFFF2E,
        ENC_CMD_SCREENSAVER             = 0xFFFFFF2F,
        ENC_CMD_SKETCH                  = 0xFFFFFF30,
        ENC_CMD_LOGO                    = 0xFFFFFF31,
        ENC_CMD_COLDSTART               = 0xFFFFFF32,
        ENC_CMD_GETMATRIX               = 0xFFFFFF33,
        ENC_CMD_GRADCOLOR               = 0xFFFFFF34,
        ENC_CMD_CSKETCH                 = 0xFFFFFF35,
        ENC_CMD_SETROTATE               = 0xFFFFFF36,
        ENC_CMD_SNAPSHOT2               = 0xFFFFFF37,
        ENC_CMD_SETBASE                 = 0xFFFFFF38,
        ENC_CMD_MEDIAFIFO               = 0xFFFFFF39,
        ENC_CMD_PLAYVIDEO               = 0xFFFFFF3A,
        ENC_CMD_SETFONT2                = 0xFFFFFF3B,
        ENC_CMD_SETSCRATCH              = 0xFFFFFF3C,
        ENC_CMD_ROMFONT                 = 0xFFFFFF3F,
        ENC_CMD_VIDEOSTART              = 0xFFFFFF40,
        ENC_CMD_VIDEOFRAME              = 0xFFFFFF41,
        ENC_CMD_SETBITMAP               = 0xFFFFFF43,

/*
        ENC_CMD_BITMAP_TRANSFORM        = 0xFFFFFF21,
        ENC_CMD_CRC                     = 0xFFFFFF03,
        ENC_CMD_EXECUTE                 = 0xFFFFFF07,
        ENC_CMD_GETPOINT                = 0xFFFFFF08,
        ENC_CMD_HAMMERAUX               = 0xFFFFFF04,
        ENC_CMD_IDCT_DELETED            = 0xFFFFFF06,
        ENC_CMD_INT_RAMSHARED           = 0xFFFFFF3D,
        ENC_CMD_INT_SWLOADIMAGE         = 0xFFFFFF3E,
        ENC_CMD_MARCH                   = 0xFFFFFF05,
        ENC_CMD_SYNC                    = 0xFFFFFF42,
        ENC_CMD_TOUCH_TRANSFORM         = 0xFFFFFF20,
*/

    }   ENC_CMD;

    // Alpha test function for ALPHA_FUNC (ProgGuide 4.4 p.92) and
    // and STENCIL_FUNC (ProgGuide 4.42 p.139)
    // See ProgGuide Table 5 p.92.
    typedef enum
    {
        FUNC_NEVER                  = 0,
        FUNC_LESS                   = 1,
        FUNC_LEQUAL                 = 2,
        FUNC_GREATER                = 3,
        FUNC_GEQUAL                 = 4,
        FUNC_EQUAL                  = 5,
        FUNC_NOTEQUAL               = 6,
        FUNC_ALWAYS                 = 7,
    }   FUNC;

    // Graphics primitive operations for BEGIN (ProgGuide 4.5 Table 6 p.94)
    typedef enum
    {
        BEGIN_BITMAPS               = 1,
        BEGIN_POINTS                = 2,
        BEGIN_LINES                 = 3,
        BEGIN_LINE_STRIP            = 4,
        BEGIN_EDGE_STRIP_R          = 5,
        BEGIN_EDGE_STRIP_L          = 6,
        BEGIN_EDGE_STRIP_A          = 7,
        BEGIN_EDGE_STRIP_B          = 8,
        BEGIN_RECTS                 = 9,
    }   BEGIN;

    // Bitmap format for BITMAP_LAYOUT (ProgGuide 4.7 Table 7 p.97)
    typedef enum
    {
        FORMAT_ARGB1555             = 0,
        FORMAT_L1                   = 1,
        FORMAT_L4                   = 2,
        FORMAT_L8                   = 3,
        FORMAT_RGB332               = 4,
        FORMAT_ARGB2                = 5,
        FORMAT_ARGB4                = 6,
        FORMAT_RGB565               = 7,
        FORMAT_PALETTED             = 8,
        FORMAT_TEXT8X8              = 9,
        FORMAT_TEXTVGA              = 10,
        FORMAT_BARGRAPH             = 11,
        FORMAT_PALETTED565          = 14,
        FORMAT_PALETTED4444         = 15,
        FORMAT_PALETTED8            = 16,
        FORMAT_L2                   = 17,

        FORMAT_ARGB8                = 0x20, // Used by SNAPSHOT2 only
    }   FORMAT;

    // Bitmap filtering mode for BITMAP_SIZE (ProgGuide 4.9 p.103)
    typedef enum
    {
        FILTER_NEAREST              = 0,
        FILTER_BILINEAR             = 1,
    }   FILTER;

    // Bitmap wrap mode for BITMAP_SIZE (ProgGuide 4.9 p.103)
    typedef enum
    {
        WRAP_BORDER                 = 0,
        WRAP_REPEAT                 = 1,
    }   WRAP;

    // Blending factor for BLEND_FUNC (ProgGuide 4.18 Table 8 p.114)
    typedef enum
    {
        BLEND_ZERO                  = 0,
        BLEND_ONE                   = 1,
        BLEND_SRC_ALPHA             = 2,
        BLEND_DST_ALPHA             = 3,
        BLEND_ONE_MINUS_SRC_ALPHA   = 4,
        BLEND_ONE_MINUS_DST_ALPHA   = 5,
    }   BLEND;

    // Stencil operations for STENCIL_OP (ProgGuide 4.44 Figure 10 p.141)
    typedef enum {
        STENCIL_ZERO                = 0,
        STENCIL_KEEP                = 1,
        STENCIL_REPLACE             = 2,
        STENCIL_INCR                = 3,
        STENCIL_DECR                = 4,
        STENCIL_INVERT              = 5,
    }   STENCIL;

    // Options for coprocessor commands (ProgGuide 5.8 p.158)
    // NOTE: These are flags; they may be combined.
    typedef enum {
        OPT_NONE                    = 0x0000,
        OPT_3D                      = 0x0000, // BUTTON, CLOCK, KEYS, GAUGE, SLIDER, DIAL, TOGGLE, PROGRESS, SCROLLBAR
        OPT_RGB565                  = 0x0000, // LOADIMAGE
        OPT_MONO                    = 0x0001, // LOADIMAGE
        OPT_NODL                    = 0x0002, // LOADIMAGE
        OPT_NOTEAR                  = 0x0004, // PLAYVIDEO
        OPT_FULLSCREEN              = 0x0008, // PLAYVIDEO
        OPT_MEDIAFIFO               = 0x0010, // PLAYVIDEO
        OPT_SOUND                   = 0x0020, // PLAYVIDEO
        OPT_FLAT                    = 0x0100, // BUTTON, CLOCK, KEYS, GAUGE, SLIDER, DIAL, TOGGLE, PROGRESS, SCROLLBAR
        OPT_SIGNED                  = 0x0100, // NUMBER
        OPT_CENTERX                 = 0x0200, // KEYS, TEXT, NUMBER
        OPT_CENTERY                 = 0x0400, // KEYS, TEXT, NUMBER
        OPT_CENTER                  = 0x0600, // KEYS, TEXT, NUMBER
        OPT_RIGHTX                  = 0x0800, // KEYS, TEXT, NUMBER
        OPT_NOBACK                  = 0x1000, // CLOCK, GAUGE
        OPT_NOTICKS                 = 0x2000, // CLOCK, GAUGE
        OPT_NOHM                    = 0x4000, // CLOCK
        OPT_NOPOINTER               = 0x4000, // GAUGE
        OPT_NOSECS                  = 0x8000, // CLOCK
        OPT_NOHANDS                 = 0xC000, // CLOCK
    }   OPT; // 16 bits

    // Constants for registers and command parameters
    const uint32_t DLSWAP_DONE                 = 0x00000000UL;
    const uint32_t DLSWAP_FRAME                = 0x00000002UL;
    const uint32_t DLSWAP_LINE                 = 0x00000001UL;
    const uint32_t INT_CMDEMPTY                = 0x00000020UL;
    const uint32_t INT_CMDFLAG                 = 0x00000040UL;
    const uint32_t INT_CONVCOMPLETE            = 0x00000080UL;
    const uint32_t INT_G8                      = 0x00000012UL;
    const uint32_t INT_L8C                     = 0x0000000CUL;
    const uint32_t INT_PLAYBACK                = 0x00000010UL;
    const uint32_t INT_SOUND                   = 0x00000008UL;
    const uint32_t INT_SWAP                    = 0x00000001UL;
    const uint32_t INT_TAG                     = 0x00000004UL;
    const uint32_t INT_TOUCH                   = 0x00000002UL;
    const uint32_t INT_VGA                     = 0x0000000DUL;
    const uint32_t LINEAR_SAMPLES              = 0x00000000UL;
    const uint32_t TOUCHMODE_CONTINUOUS        = 0x00000003UL;
    const uint32_t TOUCHMODE_FRAME             = 0x00000002UL;
    const uint32_t TOUCHMODE_OFF               = 0x00000000UL;
    const uint32_t TOUCHMODE_ONESHOT           = 0x00000001UL;
    const uint32_t ULAW_SAMPLES                = 0x00000001UL;
    const uint32_t VOL_ZERO                    = 0x00000000UL;
    const uint32_t ADC_DIFFERENTIAL            = 0x00000001UL;
    const uint32_t ADC_SINGLE_ENDED            = 0x00000000UL;
    const uint32_t ADPCM_SAMPLES               = 0x00000002UL;

    //-----------------------------------------------------------------------
    // "Host commands", see Datasheet 4.1.5 p.16
    enum HOSTCMD
    {
        ACTIVE          = 0x00,
        STANDBY         = 0x41,
        PWRDOWN         = 0x50, // or 0x43
        PD_ROMS         = 0x49,
        CLKEXT          = 0x44,
        CLKINT          = 0x48,
        CLKSEL          = 0x61, // or 0x62
        RST_PULSE       = 0x68,
        PINDRIVE        = 0x70,
        PIN_PD_STATE    = 0x71,
    };

    // Host memory operations
    enum MEM_OPERATION
    {
        READ            = 0x0,          // Read from FT81X to host
        WRITE           = 0x80,         // Write from host to FT81X
    };

    // Clock selection parameter; also used in the initialization parameters
    enum CLOCK
    {
        // If the internal clock should be used, the value in the init
        // parameters is set to the following and the value of the parameter
        // isn't sent to the FT81X.
        CLOCK_INT = -1,                 // Use internal clock (60MHz)

        // The following values indicate the required clock multiplier, and
        // the resulting internal frequency based on a connected 12 MHz
        // crystal or other clock source.
        CLOCK_EXT_DEFAULT   = (0x00),   // External clock, but no multiplier
        CLOCK_EXT_x2_24MHz  = (0x02),   // 2x multiplier
        CLOCK_EXT_x3_36MHz  = (0x03),   // 3x multiplier
        CLOCK_EXT_x4_48MHz  = (0x44),   // 4x multiplier and high PLL range
        CLOCK_EXT_x5_60MHz  = (0x45),   // 5x multiplier and high PLL range
        // The data sheet doesn't mention the following; they were in the
        // demo code.
        CLOCK_EXT_x6_72MHz  = (0x46),   // 6x multiplier and high PLL range
        CLOCK_EXT_x7_84MHz  = (0x47),   // 7x multiplier and high PLL range
    };

    // Chip identifiers
    enum CHIPID
    {
        // Use this value in the init parameters to skip chip ID checking
        ANY = 0,

        // Following are values in the chip ID register just after the
        // processor has been started.
        FT810 = 0x00011008,
        FT811 = 0x00011108,
        FT812 = 0x00011208,
        FT813 = 0x00011308,
    };

    // This struct is used to describe the hardware parameters for a 
    // particular LCD display panel.
    struct DisplayProfile
    {
        CLOCK       _clock;             // Clock source and multiplier
        CHIPID      _chipid;            // Expected chip ID; ANY=don't care
        uint32_t    _frequency;         // ClockFreq to store; 0 = don't store
        bool        _lcd10ma;           // True=drive LCD with 10mA (false=5)
        bool        _cspread;           // True=enable RGB clock spreading, see datasheet 4.4 p.27
        bool        _dither;            // True=enable dither, see datasheet 4.4 p.27
        uint16_t    _outbits;           // 3x3 bits indicating num LCD bits used, see datasheet 4.4 p.27

        // TODO: Add a constructor that automatically calculates some of these
        uint16_t    _hsize;             // active display width
        uint16_t    _hcycle;            // total number of clocks per line, incl front/back porch
        uint16_t    _hoffset;           // start of active line
        uint16_t    _hsync0;            // start of horizontal sync pulse
        uint16_t    _hsync1;            // end of horizontal sync pulse
        uint16_t    _vsize;             // active display height
        uint16_t    _vcycle;            // total number of lines per screen, incl pre/post
        uint16_t    _voffset;           // start of active screen
        uint16_t    _vsync0;            // start of vertical sync pulse
        uint16_t    _vsync1;            // end of vertical sync pulse
        uint8_t     _swizzle;           // FT800 output to LCD - pin order
        uint8_t     _pclkpol;           // LCD data is clocked in on this PCLK edge
        uint8_t     _pclk;              // Clock divisor
    };

protected:
    // Constructor parameters
    const DisplayProfile
                   &_profile;           // Display init parameters
    SPIClass       &_spi;               // SPI interface, not NULL
    const SPISettings
                    _spi_settings;      // SPI settings
    const int       _pin_cs;            // Chip Select Not Pin (-1 = none)
    const int       _pin_pd;            // Power Down Not Pin (-1 = none)
    const int       _pin_int;           // Interrupt Pin (-1 = none)

    // Cached constants
    const uint16_t  _hcenter;           // Horizontal center in pixels
    const uint16_t  _vcenter;           // Vertical center in pixels_

    // State variables
    uint16_t        _cmd_index;         // Graphics engine cmd write index
                                        //   (offset from RAM_CMD)
    uint16_t        _dl_index;          // Display list write index
                                        //   (offset from RAM_DL)
    bool            _selected;          // True if CS active

    /////////////////////////////////////////////////////////////////////////
    // CONSTRUCTOR
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Constructor
    StEVE(
        const DisplayProfile &profile,  // Display profile
        SPIClass &spi,                  // SPI instance; call begin() first
        uint32_t spi_clock,             // SPI clock
        int pin_cs = -1,                // Chip Select Not pin, -1 = none
        int pin_pd = -1,                // Power Down Not pin, -1 = none
        int pin_int = -1)               // Interrupt pin, -1 = none
        : _profile(profile)
        , _spi(spi)
        , _spi_settings(spi_clock, MSBFIRST, SPI_MODE0)
        , _pin_cs(pin_cs)
        , _pin_pd(pin_pd)
        , _pin_int(pin_int)
        , _hcenter(profile._hsize / 2)
        , _vcenter(profile._vsize / 2)
    {
        // Set the output pins before switching the pins to output, to
        // avoid glitches
        _selected = true; // Make sure the CS pin is changed next
        Select(false); // De-select
        Power(true); // Power on

        // Configure the interrupt pin first, to avoid output-to-output
        // conflicts
        if (_pin_int >= 0)
        {
            pinMode(_pin_int, INPUT_PULLUP);
        }

        // Configure the Power Down Not pin; it's also used as reset.
        // This will power up the panel.
        if (_pin_pd >= 0)
        {
            pinMode(_pin_pd, OUTPUT);
        }

        // Finally configure the chip select pin
        if (_pin_cs >= 0)
        {
            pinMode(_pin_cs, OUTPUT);
        }

        // NOTE: at this point, the display is not active.
        // Don't forget to call Begin().
    }

    /////////////////////////////////////////////////////////////////////////
    // FUNCTIONS TO ACCESS DISPLAY PARAMETERS AND CONSTANTS
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Get a pointer to the profile
    const DisplayProfile *Profile() const
    {
        return &_profile;
    }

public:
    //-----------------------------------------------------------------------
    // Get display width
    uint16_t                            // Returns width in pixels
    Width() const
    {
        return _profile._hsize;
    }

public:
    //-----------------------------------------------------------------------
    // Get display height
    uint16_t                            // Returns height in pixels
    Height() const
    {
        return _profile._vsize;
    }

public:
    //-----------------------------------------------------------------------
    // Get horizontal center
    uint16_t                            // Returns hor. center in pixels
    HCenter() const
    {
        return _hcenter;
    }

public:
    //-----------------------------------------------------------------------
    // Get vertical center
    uint16_t                            // Returns ver. center in pixels
    VCenter() const
    {
        return _vcenter;
    }

    /////////////////////////////////////////////////////////////////////////
    // HARDWARE CONTROL
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Turn the power on or off if a -PD pin is set
    void Power(
        bool enable)                    // True = on, false = off / reset
    {
        if (_pin_pd >= 0)
        {
            // Set the pin HIGH to power up
            digitalWrite(_pin_pd, enable ? HIGH : LOW);
        }
    }

public:
    //-----------------------------------------------------------------------
    // Select the FT81X if a -CS pin is set
    void Select(
        bool enable,                    // Select the FT81X display
        bool dospi = false)             // True to also reprogram SPI port
    {
        if (enable != _selected)
        {
            _selected = enable;

            if (_pin_cs >= 0)
            {
                DBG_TRAFFIC("Select %u\n", !!enable);

                // Set the pin LOW to enable the chip
                digitalWrite(_pin_cs, enable ? LOW : HIGH);
            }
        }

        if (dospi)
        {
            if (enable)
            {
                DBG_TRAFFIC("beginTransaction\n");
                _spi.beginTransaction(_spi_settings);
            }
            else
            {
                DBG_TRAFFIC("endTransaction\n");
                _spi.endTransaction();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////
    // INITIALIZATION
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Initialize the display
    //
    // See ProgGuide 2.3 p.11
    bool                                // Returns true=success false=failure
    Begin()
    {
        // Wake up the EVE
        DBG_GEEK("Resetting\n");
        delay(20);                      // Wait a few ms before waking it up
        Power(false);                   // Reset
        delay(6);                       // Hold for a little while
        Power(true);                    // Power on
        delay(21);

        // Select the chip and initialize the SPI bus
        Select(true, true);

        // Select the internal or external clock, and select the PLL
        // multiplier for an external clock as necessary.
        if (_profile._clock == CLOCK_INT)
        {
            DBG_TRAFFIC("Setting clock mode to internal\n");
            //Command_Write(CLKINT);
        }
        else
        {
            DBG_TRAFFIC("Setting clock mode to %X\n", _profile._clock);
            HostCommand(CLKEXT);
            HostCommand(CLKSEL, (uint8_t)_profile._clock);
        }

        // Activate the FT81X and give it some time to initialize
        HostCommand(ACTIVE, 0);
        delay(40);

        // Repeatedly poll REG_ID with up to 250 maximum retries and a 1 ms
        // delay between retries.
        // The register should return 0x7C when the processor is running.
        if (!REG_Wait8(REG_ID, 0x7C, 250, 1))
        {
            // TODO: report problem and return false
            DBG_STAT("Timeout waiting for ID of 0x7C.\n");
            DBG_STAT("Is the device connected? Is the right EVE device selected?\n");
            return false;
        }

        // Repeatedly poll REG_CPURESET until it returns 0 meaning the reset
        // is complete
        while (!REG_Wait8(REG_CPURESET, 0, 250, 1))
        {
            // TODO: report problem and return false
            DBG_STAT("Timeout waiting for EVE CPU reset.\n");
            DBG_STAT("Is the device connected? Is the right EVE device selected?\n");
            // TODO: in at the top, out at the bottom
            return false;
        }

        // Read the chip ID and match it with the expected value
        if (_profile._chipid != ANY)
        {
            uint32_t chip_id = REG_Read_32(CHIP_ID_ADDRESS);
            if (_profile._chipid != (CHIPID)chip_id)
            {
                DBG_STAT("Chip ID mismatch: Wanted %08X, got %08X\n", _profile._chipid, chip_id);
                // TODO: in at the top, out at the bottom
                return false;
            }
        }

        // Store the frequency in the register if requested
        if (_profile._frequency)
        {
            REG_Write_32(REG_FREQUENCY, _profile._frequency);
        }

        // Get the current write pointer from the EVE
        CmdInitWriteIndex();

        // Execute bug workarounds for specific subclasses
        if (!EarlyInit())
        {
            // TODO: in at the top, out at the bottom
            return false;
        }

        // Set PCLK to zero; don't clock the LCD until later
        REG_Write_8(REG_PCLK, 0);

        // Turn off backlight
        REG_Write_8(REG_PWM_DUTY, 0);

        // Initialize display parameters
        REG_Write_16(REG_HSIZE,    _profile._hsize);   // active display width
        REG_Write_16(REG_HCYCLE,   _profile._hcycle);  // total number of clocks per line, incl front/back porch
        REG_Write_16(REG_HOFFSET,  _profile._hoffset); // start of active line
        REG_Write_16(REG_HSYNC0,   _profile._hsync0);  // start of horizontal sync pulse
        REG_Write_16(REG_HSYNC1,   _profile._hsync1);  // end of horizontal sync pulse
        REG_Write_16(REG_VSIZE,    _profile._vsize);   // active display height
        REG_Write_16(REG_VCYCLE,   _profile._vcycle);  // total number of lines per screen, incl pre/post
        REG_Write_16(REG_VOFFSET,  _profile._voffset); // start of active screen
        REG_Write_16(REG_VSYNC0,   _profile._vsync0);  // start of vertical sync pulse
        REG_Write_16(REG_VSYNC1,   _profile._vsync1);  // end of vertical sync pulse
        REG_Write_8( REG_SWIZZLE,  _profile._swizzle); // FT800 output to LCD - pin order
        REG_Write_8( REG_PCLK_POL, _profile._pclkpol); // LCD data is clocked in on this PCLK edge
        // Don't set PCLK yet - wait for just after the first display list

        // Set 10 mA or 5 mA drive for PCLK, DISP, VSYNC, DE, RGB lines and
        // back light PWM.
        if (_profile._lcd10ma)
        {
            REG_Write_16(REG_GPIOX, REG_Read_16(REG_GPIOX) | 0x1000);
        }
        else
        {
            REG_Write_16(REG_GPIOX, REG_Read_16(REG_GPIOX) & ~0x1000);
        }

        // Enable or disable RGB clock spreading for reduced noise
        REG_Write_8(REG_CSPREAD, _profile._cspread ? 1 : 0);

        // Enable or disable dithering
        REG_Write_8(REG_DITHER, _profile._dither ? 1 : 0);

        // Enable output bits on LCD outputs
        // Encoded as 3 values in 3 groups of 3 bits.
        // 0b0000_000R_RRGG_GBBB
        //                   --- Number of bits used for Blue
        //               ----    Number of bits used for Green
        //           ----        Number of bits used for Red
        //   --------            Reserved
        // If set to 0 (default), the EVE uses 8 bits (FT812/FT813) or 6 bits
        // (FT810/FT811).
        // This was commented out in the original code
        //REG_Write_16(REG_OUTBITS, _profile._outbits);

        // Touch screen initialization
        if (!TouchInit())
        {
            // TODO: in at the top, out at the bottom
            return false;
        }

        // TODO: Audio off

        // Write the initial display list directly to RAM_DL; the coprocessor
        // may not be available this early.
        // This just shows a black screen
        _dl_index = 0;
        DLAdd(ENC_CLEAR_COLOR(0));
        DLAdd(ENC_CLEAR(1,1,1));// color, stencil, tags
        DLAdd(ENC_DISPLAY());

        // Tell the EVE that it can swap display lists at the next available
        // frame boundary
        REG_Write_32(REG_DLSWAP, 2 /* DLSWAP_FRAME */);

        // Enable the DISP line of the LCD
        REG_Write_16(REG_GPIOX, REG_Read_16(REG_GPIOX) | 0x8000);

        // Now start clocking the data to the LCD panel
        REG_Write_8(REG_PCLK, _profile._pclk);

        REG_Write_16(REG_PWM_HZ, 250);
        REG_Write_8(REG_PWM_DUTY, 128);

        // TODO: Calibrate touch screen if necessary

        return true;
    }

protected:
    //-----------------------------------------------------------------------
    // Early initialization virtual function
    //
    // This gets called by Begin after starting the chip but before
    // initializing the timing registers.
    // Specific subclasses can use this to work around bugs.
    virtual bool                        // Returns true=success false=failure
    EarlyInit()
    {
        // EVE_Init_Goodix_GT911()
        // EVE_Init_Pen_Up_Bug_Fix()
        return true;
    }

protected:
    //-----------------------------------------------------------------------
    // Touch screen initialization
    //
    // This gets called by Begin to initialize the touch screen and
    // work around bugs.
    // The implementation here switches the touch functionality off. This
    // can be used for projects that don't require touch, regardless of
    // whether the EVE has touch screen support.
    virtual bool                        // Returns true=success false=failure
    TouchInit()
    {
        // Disable touch
        REG_Write_8(REG_TOUCH_MODE, 0);
        // Eliminate any false touches
        REG_Write_16(REG_TOUCH_RZTHRESH, 0);

        return true;
    }

    /////////////////////////////////////////////////////////////////////////
    // LOW LEVEL SPI CONTROL
    /////////////////////////////////////////////////////////////////////////

protected:
    //-----------------------------------------------------------------------
    // Send a host command
    //
    // Referred to as "host_command()" in the FT81X documentation
    void HostCommand(
        HOSTCMD cmd,                    // Command to send
        uint8_t parameter = 0)          // Parameter for the command
    {
        DBG_TRAFFIC("Host command %X\n", cmd);

        Select(true);

        // 1st byte is the command
        _spi.transfer((uint8_t)cmd);

        // 2nd byte is the parameter
        _spi.transfer(parameter);

        // 3rd byte is always 0
        _spi.transfer(0);

        Select(false);
    }

protected:
    //-----------------------------------------------------------------------
    // Select the chip and send the host command to read or write data
    //
    // Referred to as "Host Memory Read/Write" in the documentation
    void SelectAndAddress(
        uint32_t address,               // Address to access
        MEM_OPERATION operation)        // Read or write
    {
        DBG_TRAFFIC("Address %X %s\n", address, operation == WRITE ? "WRITE" : "READ");
        Select(true);

        // Send Operation plus high address byte
        _spi.transfer((uint8_t)(address >> 16) | (uint8_t)operation);

        // Send middle address byte
        _spi.transfer((uint8_t)(address >> 8));

        // Send low address byte
        _spi.transfer((uint8_t)(address));
    }

protected:
    //-----------------------------------------------------------------------
    // Read a one-byte register from FT81X memory
    //
    // Referred to as "rd8" in the documentation
    uint8_t                             // Returns data at given address
    REG_Read_8(
        uint32_t address)               // Address (22 bits; not checked)
    {
        uint8_t result;

        // Send the 24-bit address and operation flag.
        SelectAndAddress(address, READ);

        // Send dummy byte
        _spi.transfer(0);

        // Send another dummy; the EVE returns the data
        result = _spi.transfer(0);

        Select(false);

        DBG_TRAFFIC("Reg %X = %X\n", address, result);

        return result;
    }

protected:
    //-----------------------------------------------------------------------
    // Read a 2-byte register from FT81X memory
    //
    // Referred to as "rd16" in the documentation
    uint16_t                            // Returns data at given address
    REG_Read_16(
        uint32_t address)               // Address (22 bits; not checked)
    {
        uint16_t result;
        
        // Send the 24-bit address and operation flag.
        SelectAndAddress(address, READ);

        // Send dummy byte
        _spi.transfer(0);

        // Send more dummies; the EVE returns the data
        result =  (uint16_t)_spi.transfer(0);
        result |= (uint16_t)_spi.transfer(0) << 8;

        Select(false);

        DBG_TRAFFIC("Reg %X = %X\n", address, result);

        return result;
    }

protected:
    //-----------------------------------------------------------------------
    // Read a 4-byte register from FT81X memory
    //
    // Referred to as "rd32" in the documentation
    uint32_t                            // Returns data at given address
    REG_Read_32(
        uint32_t address)               // Address (22 bits; not checked)
    {
        uint32_t result;
        
        // Send the 24-bit address and operation flag.
        SelectAndAddress(address, READ);

        // Send dummy byte
        _spi.transfer(0);

        // Send more dummies; the EVE returns the data
        result =  (uint32_t)_spi.transfer(0);
        result |= (uint32_t)_spi.transfer(0) << 8;
        result |= (uint32_t)_spi.transfer(0) << 16;
        result |= (uint32_t)_spi.transfer(0) << 24;

        Select(false);

        DBG_TRAFFIC("Reg %X = %X\n", address, result);

        return result;
    }

protected:
    //-----------------------------------------------------------------------
    // Repeat reading a register until it matches the given value
    //
    // The function returns 0 if the register didn't contain the expected
    // value after the given number of retries. In other words, the result
    // is successful if the function returns nonzero.
    uint8_t                             // Returns num retries remaining
    REG_Wait8(
        uint32_t address,               // Address to read
        uint8_t value,                  // Expected value
        uint8_t maxtries,               // Maximum number of tries
        uint32_t delay_between_tries)   // Delay between tries in ms
    {
        uint8_t result = maxtries;
        uint8_t read_value = 0;

        while (result)
        {
            read_value = REG_Read_8(address);

            if (read_value == value)
            {
                DBG_TRAFFIC("Match after %u tries\n", maxtries - result);
                return result;
            }

            delay(delay_between_tries);

            result--;
        }

        DBG_GEEK("Timeout waiting for %X to become %X, last read value was %X\n", address, value, read_value);
        return result;
    }

protected:
    //-----------------------------------------------------------------------
    // Read a block of memory
    void ReadArray(
        uint32_t address,               // Address to read from
        uint16_t length,                // Number of bytes to read
        uint8_t *destination)           // Destination buffer
    {
        DBG_TRAFFIC("Reading %X length %X (%u dec)\n", address, length, length);

        SelectAndAddress(address, READ);

        // Send dummy byte
        _spi.transfer(0);

        // Read the block of memory
        // TODO: Use block transfer of SPI class?
        for (uint16_t i = 0; i < length; i++)
        {
            *destination++ = _spi.transfer(0);
        }

        Select(false);
    }

protected:
    //-----------------------------------------------------------------------
    // Read a 32 bit value from the given Cmd index
    //
    // This can be used to retrieve a value that gets stored by the
    // co-processor into the output parameters of a command.
    uint32_t                            // Returns value
    CmdRead32(
        uint16_t cmdindex)
    {
        return REG_Read_32(RAM_CMD + (cmdindex % RAM_CMD_SIZE));
    }

protected:
    //-----------------------------------------------------------------------
    // Write a 32 bit value in little-endian format
    //
    // Internal variables aren't updated and the Select line is not changed.
    void Send32(
        uint32_t value) const
    {
        _spi.transfer((uint8_t)(value));
        _spi.transfer((uint8_t)(value >> 8));
        _spi.transfer((uint8_t)(value >> 16));
        _spi.transfer((uint8_t)(value >> 24));
    }

protected:
    //-----------------------------------------------------------------------
    // Write a 16-bit value in little-endian format
    //
    // Internal variables aren't updated and the Select line is not changed.
    void Send16(
        uint16_t value) const
    {
        _spi.transfer((uint8_t)(value));
        _spi.transfer((uint8_t)(value >> 8));
    }

protected:
    //-----------------------------------------------------------------------
    // Write an 8 bit value
    //
    // Internal variables aren't updated and the Select line is not changed.
    //
    // Yes it's a little silly to declare this function which is basically
    // just an equivalent of the SPI transfer function but this way it makes
    // multiple function calls more symmetrical in the code.
    void Send8(
        uint8_t value) const
    {
        _spi.transfer(value);
    }

protected:
    //-----------------------------------------------------------------------
    // Send data from RAM to the chip
    //
    // The function sends a block of data of the given size. If the number
    // of requested bytes is not a multiple of 4, it sends extra nul bytes
    // but doesn't read beyond the end of the buffer.
    //
    // The return value is the number of bytes that was sent to the EVE,
    // which may be more than the requested length.
    uint16_t                            // Returns number of bytes sent
    SendData(
        const uint8_t *data,            // Data buffer to send
        uint16_t len)                   // Buffer length
    {
        uint16_t result = 0;

        const uint8_t *p = data;
        while (len)
        {
            // Send in blocks of 4
            for (unsigned i = 0; i < 4; i++)
            {
                uint8_t c = 0;

                // If we've reached the end, send 0. Otherwise, get the
                // next byte
                if (len)
                {
                    c = *p++;
                    len--;
                }

                Send8(c);
            }

            // We just sent 4 bytes
            result += 4;
        }

        return result;
    }

protected:
    //-----------------------------------------------------------------------
    // Send a string as part of a co-processor command
    //
    // The function reads a string from RAM, and transfers it to the EVE
    // It stops either when it finds the end of the source string, or when
    // it has transferred the requested length minus one. Then it sends
    // a nul terminator byte followed by however many bytes it takes to make
    // the total number of transmitted bytes a multiple of 4.
    //
    // The return value is the number of bytes that was sent to the EVE,
    // which may be longer than the string length.
    //
    // The maximum length parameter includes the nul terminator. If 0 is
    // used, the function sends the entire string until it encounters the
    // terminating \0.
    uint16_t                            // Returns number of bytes sent
    SendString(
        const char *message,            // Characters to send, '\0' is end
        uint16_t maxlen)                // Max input length including \0
    {
        uint16_t result = 0;

        // Initialize the remaining length
        uint16_t len = maxlen; // Underflows for input value 0
        if ((!len) || (len > 256)) // Max 256 bytes supported
        {
            len = 256;
        }

        const char *s = message;

        // NOTE: The outer loop shouldn't have a conditional expression.
        // The remaining length is checked at the end of the outer loop
        // so that at least 4 bytes are sent, even if the string is empty.
        for(;;)
        {
            // Send 4 bytes regardless of the input.
            for (unsigned i = 0; i < 4; i++)
            {
                char c = '\0';

                // Read a byte if allowed (if not allowed, the nul byte is sent)
                if (len)
                {
                    c = *s++;

                    // If the byte in our RAM was a terminator, don't read
                    // any more bytes after the current one. It's okay to
                    // send this one of course.
                    if (!c)
                    {
                        len = 0;
                    }
                    else
                    {
                        len--;
                    }
                }

                DBG_TRAFFIC("SendString 0x%02X\n", (uint8_t)c);

                Send8((uint8_t)(c));
            }

            // We just sent 4 characters regardless of the input
            result += 4;

            // If there's nothing left to process (we reached end of string
            // or the maximum length), break out.
            if (!len)
            {
                break;
            }
        }

        return result;
    }

protected:
    //-----------------------------------------------------------------------
    // Send a string from program memory
    //
    // Same as for RAM strings, but the string parameter is for strings
    // stored in program memory. This takes some extra processing for some
    // Arduinos; use the F() macro to pass a literal string or program
    // memory string.
    uint16_t                            // Returns number of bytes sent
    SendStringF(
        const __FlashStringHelper *message, // Characters to send, '\0' is end
        uint16_t maxlen)                // Max input length including \0
    {
        char buf[256];

        if ((!maxlen) || (maxlen > sizeof(buf)))
        {
            maxlen = sizeof(buf);
        }

        // NOTE: because of the way we process the string when we send it
        // to the EVE, there is no need to make sure that the string is
        // nul-terminated after the copy below.
        strncpy_P(buf, (const char *)message, maxlen);

        return SendString(buf, maxlen);
    }

protected:
    //-----------------------------------------------------------------------
    // Write an 8 bit register
    //
    // Referred to as "wr8" in the documentation
    void REG_Write_8(
        uint32_t address,               // Address to store data
        uint8_t value)                  // Value to store
    {
        DBG_TRAFFIC("Writing %X = %02X\n", address, value);

        SelectAndAddress(address, WRITE);

        _spi.transfer(value);

        Select(false);
    }

protected:
    //-----------------------------------------------------------------------
    // Write a 16 bit register
    //
    // Referred to as "wr16" in the documentation
    void REG_Write_16(
        uint32_t address,               // Address to store data
        uint16_t value)                 // Value to store
    {
        DBG_TRAFFIC("Writing %X = %04X\n", address, value);

        SelectAndAddress(address, WRITE);

        Send16(value);

        Select(false);
    }

protected:
    //-----------------------------------------------------------------------
    // Write a 32 bit register
    //
    // Referred to as "wr32" in the documentation
    void REG_Write_32(
        uint32_t address,               // Address to store data
        uint32_t value)                 // Value to store
    {
        DBG_TRAFFIC("Writing %X = %08X\n", address, value);

        SelectAndAddress(address, WRITE);

        Send32(value);

        Select(false);
    }

    /////////////////////////////////////////////////////////////////////////
    // DISPLAY LIST
    /////////////////////////////////////////////////////////////////////////
    // NOTE: The display list is used internally. It only supports
    // the "simple" commands that are encoded with the ENC_* functions.
    // In most cases it's easiest to use the co-processor, so this section
    // is safe to ignore when you're trying to understand the EVE.
    //
    // See ProgGuide 5.4 p.154 about how to synchronize the display list
    // with the co-processor command queue.

protected:
    //-----------------------------------------------------------------------
    // Reset the display list index
    //
    // NOTE: It shouldn't be necessary to get the display list index
    // without storing a command first, so there is no function to get the
    // current index without storing anything.
    void DLReset(
        uint16_t index = 0)             // New index
    {
        DBG_TRAFFIC("DL reset %u\n", index);

        // The optimizer should be smart enough to turn the mod operator
        // into an AND operation.
        _dl_index = index % RAM_DL_SIZE;
    }

protected:
    //-----------------------------------------------------------------------
    // Store a 32 bit display list command
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    //
    // Referred to as "dl" in the documentation.
    uint16_t                            // Returns DL index after increment
    DLAdd(
        uint32_t value)                 // Value to write
    {
        DBG_TRAFFIC("dl(%08X)\n", value);

        REG_Write_32(RAM_DL + _dl_index, value);

        // Update the index. The optimizer should be smart enough to turn
        // the mod operator into an AND operation.
        _dl_index = (_dl_index + 4) % RAM_DL_SIZE;

        return _dl_index;
    }

    /////////////////////////////////////////////////////////////////////////
    // CO-PROCESSOR SUPPORT
    /////////////////////////////////////////////////////////////////////////

protected:
    //-----------------------------------------------------------------------
    // Synchronize the local command write index
    //
    // The co-processor executes commands from, and updates the command
    // read index, until it reaches the command write index. So when we
    // want to start to write a sequence of commands for the co-processor,
    // we should synchronize our own write index to the write index of the
    // co-processor. When we're done writing commands, we will set the
    // write index to the end of the list.
    //
    // This function basically cancels all commands that were already queued
    // for the co-processor (if any), and restarts the building of the queue.
    int16_t                             // Returns updated Cmd index
    CmdInitWriteIndex()
    {
        DBG_TRAFFIC("Reading REG_CMD_WRITE\n");

        _cmd_index = REG_Read_16(REG_CMD_WRITE);

        return _cmd_index;
    }

protected:
    //-----------------------------------------------------------------------
    // Get amount of free space in the command queue
    //
    // The number is based on the location where the chip is reading
    // (not writing).
    uint16_t                            // Returns number of bytes free
    CmdGetFreeCmdSpace()
    {
        uint16_t result = ((4096 - 4) - ((_cmd_index - REG_Read_16(REG_CMD_READ)) & 0xFFF));

        DBG_TRAFFIC("Free command space is %u", result);

        return result;
    }

protected:
    //-----------------------------------------------------------------------
    // Internal function to update the command index and wraps it around
    //
    // Note that a negative bump is allowed. This is useful e.g. to find out
    // where the previously stored data is. Since the internal index is
    // an unsigned, there are no issues with mod-operators using negative
    // values, which is not defined in the C/C++ standards. 
    uint16_t                            // Returns updated Cmd index
    BumpCmdIndex(
        int16_t len)                    // Length to add (not checked)
    {
        _cmd_index += (uint16_t)len;

        // Wrap around if necessary. The optimizer should be smart enought
        // to substitue an AND operation for the MOD operation.
        return (_cmd_index %= RAM_CMD_SIZE);
    }

protected:
    //-----------------------------------------------------------------------
    // Store a co-processor command with no parameters and keep EVE selected
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    uint16_t                            // Returns Cmd index after increment
    CmdSelect(
        uint32_t command)               // Command to queue
    {
        DBG_GEEK("cmd(%08X)\n", command);

        SelectAndAddress(RAM_CMD + _cmd_index, WRITE);

        // Send the command
        Send32(command);

        return BumpCmdIndex(4);
    }

protected:
    //-----------------------------------------------------------------------
    // Store a co-processor command with no parameters
    uint16_t                            // Returns Cmd index after increment
    Cmd(
        uint32_t command)               // Command to queue
    {
        uint16_t result = CmdSelect(command);

        Select(false);

        return result;
    }

public:
    //-----------------------------------------------------------------------
    // Wait until the co-processor has caught up.
    //
    // This can be used to wait for the end of a frame, and to retrieve
    // the location where the next command will be stored without storing
    // another command first.
    uint16_t                            // Returns Cmd index
    CmdWaitComplete()
    {
        DBG_TRAFFIC("Waiting for coprocessor\n");

        while (REG_Read_16(REG_CMD_READ) != _cmd_index);

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Tell the co-processor to start executing commands
    //
    // This updates the write pointer on the engine to the current write
    // location so that the coprocessor starts executing commands in the
    // command queue.
    uint16_t                            // Returns Cmd index
    CmdExecute(
        bool waituntilcomplete = false) // True = wait until done
    {
        DBG_TRAFFIC("Executing command queue\n");

        REG_Write_16(REG_CMD_WRITE, _cmd_index);

        if (waituntilcomplete)
        {
            CmdWaitComplete();
        }

        return _cmd_index;
    }

protected:
    //-----------------------------------------------------------------------
    // Get pointer to first available byte in RAM_G
    uint32_t                            // Returns pointer in RAM_G
    CmdGetPtr()
    {
        uint32_t result;

        CmdWaitComplete();

        uint16_t p; // Cmd index of output stored here
        cmd_GETPTR(&p);

        // Execute the command
        CmdExecute(true);

        // Retrieve the result
        result = CmdRead32(p);

        DBG_TRAFFIC("RAM_G first free byte is at %08X\n", result);

        return result;
    }

    /////////////////////////////////////////////////////////////////////////
    // CO-PROCESSOR QUEUEING OF BASIC COMMANDS
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Set clearing color and optionally clear the screen, stencil and tag
    uint16_t                            // Returns updated Cmd index
    CmdClear(
        uint8_t red,                    // Red
        uint8_t green,                  // Green
        uint8_t blue,                   // Blue
        bool clearscreen = true,        // Clear the screen if true
        bool clearcolor = true,         // Clear the current color if true
        bool clearstencil = true,       // Clear the stencil if true
        bool cleartag = true)           // Clear the tag if true
    {
        cmd_CLEAR_COLOR_RGB(red, green, blue);

        if (clearscreen || clearcolor || clearstencil || cleartag)
        {
            cmd_CLEAR(!!clearcolor, !!clearstencil, !!cleartag);
        }

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Set color for next commands
    uint16_t                            // Returns updated Cmd index
    CmdColor(
        uint8_t red,                    // Red
        uint8_t green,                  // Green
        uint8_t blue)                   // Blue
    {
        return cmd_COLOR_RGB(red, green, blue);
    }

public:
    //-----------------------------------------------------------------------
    // Set color for next commands using RGB value
    uint16_t                            // Returns updated Cmd index
    CmdColor(
        uint32_t rgb24)                 // red/green/blue combination
    {
        return cmd_COLOR(rgb24);
    }

public:
    //-----------------------------------------------------------------------
    // Set alpha (transparency) for next commands
    uint16_t                            // Returns updated Cmd index
    CmdAlpha(
        uint8_t alpha)                  // Alpha value
    {
        return cmd_COLOR_A(alpha);
    }

public:
    /////////////////////////////////////////////////////////////////////////
    // COMMAND ENCODING
    /////////////////////////////////////////////////////////////////////////

    //-----------------------------------------------------------------------
    // Macro to encodes a bit field in a uint32_t.
    //
    // This is used to encode the fields in the display list commands in
    // ProgGuide chapter 4.
    //
    // The Programmer's Guide shows the fields with the leftmost
    // and rightmost bit numbers, so it makes sense to use the left and
    // right bit numbers of the fields for the encoding expression.
    //
    // Example: Given a field of 3 bits at bit positions 10 to 8 inclusive,
    // N(value, 10, 8) should return ((value & 7) << 8).
    // In that expression, 8 corresponds to the rightmost bit, and 7
    // corresponds to a bit pattern of ((leftbit - rightbit) + 1) one-bits.
    //
    // In other words:
    // Say D = (leftbit) - (rightbit)   The Difference between the first and
    //                                  last bit locations of the field.
    // Say W = (D) + 1:                 The Width of the field in bits.
    // Say M = (1 << (W)) - 1:          A bitmask with W right-justified
    //                                  bits set to '1'.
    // Say V = (value) & (M):           The Value with all insignificant bits
    //                                  set to '0'.
    // Say N = (V) << (rightbit)        The field shifted into place; the eNd
    //                                  result of the macro
    //
    // Substituting this in reverse, the formula becomes:
    // N = ((V) << rightbit)
    // N = (((value) & (M)) << rightbit)
    // N = (((value) & ((1 << (W)) - 1)) << rightbit)
    // N = (((value) & ((1 << ((D) + 1)) - 1)) << rightbit)
    // N = (((value) & ((1 << (((leftbit) - (rightbit)) + 1)) - 1)) << rightbit)
    //
    // Because the "value" in the macro is used with parameters that can
    // be enums or variables of shorter length, we cast it to uint32_t in
    // the macro expansion.
    //
    // We also apply extra parentheses to avoid problems with expression
    // evaluation order.
    //
    // The result may look pretty daunting but the optimizer will reduce it
    // to code that's basically equivalent to (value & constant) << constant.
    #define N(value, leftbit, rightbit) ((((uint32_t)value) & ((1 << (((leftbit) - (rightbit)) + 1)) - 1)) << rightbit)

    //-----------------------------------------------------------------------
    // Functions for Display List commands
    //
    // A single invocation of the macro generates:
    // * A function (starting with ENC_...) to encode the bit fields into a
    //   uint32_t value
    // * A function (starting with dl_...) to add an encoded command to the
    //   display list
    // * A function (starting with cmd_...) to add an encoded command to the
    //   command list for the co-processor.
    //
    // Note: The names of the non-enum parameters include the actual number
    // of bits that are used, as a reminder. Keep in mind that some
    // parameters are encoded as unsigned or signed fixed-point value
    // and that negative fixed-point does not use 2's complement.
    #define ENC(name, declaration, parameters, value) \
        uint32_t ENC_##name declaration { return ENC_CMD_##name | value; } \
        uint16_t dl_##name declaration { return DLAdd(ENC_##name parameters); } \
        uint16_t cmd_##name declaration { return Cmd(ENC_##name parameters); }
    ENC(ALPHA_FUNC,        (FUNC     func,     uint8_t  ref8),                                                          (func, ref8),                               N(func,     10,  8) | N(ref8,      7,  0)                                                            ) // ProgGuide 4.4 p.92
    ENC(BEGIN,             (BEGIN    prim),                                                                             (prim),                                     N(prim,      3,  0)                                                                                  ) // ProgGuide 4.5 p.94
    ENC(BITMAP_HANDLE,     (uint8_t  handle5),                                                                          (handle5),                                  N(handle5,   4,  0)                                                                                  ) // ProgGuide 4.6 p.96
    ENC(BITMAP_LAYOUT,     (FORMAT   format,   uint32_t stride10, uint32_t height9),                                    (format, stride10, height9),                N(format,   23, 19) | N(stride10, 18,  9) | N(height9,  8,  0)                                       ) // ProgGuide 4.7 p.97
    ENC(BITMAP_LAYOUT_H,   (uint32_t strideh2, uint32_t heighth2),                                                      (strideh2, heighth2),                       N(strideh2,  3,  2) | N(heighth2,  1,  0)                                                            ) // ProgGuide 4.8 p.103
    ENC(BITMAP_SIZE,       (FILTER   filter,   WRAP     wrapx,    WRAP     wrapy,   uint16_t width9, uint16_t height9), (filter, wrapx, wrapy, width9, height9),    N(filter,   20, 20) | N(wrapx,    19, 19) | N(wrapy,   18, 18) | N(width9, 17,  9) | N(height9, 8, 0)) // ProgGuide 4.9 p.103
    ENC(BITMAP_SIZE_H,     (uint16_t widthh2,  uint16_t heighth2),                                                      (widthh2, heighth2),                        N(widthh2,   3,  2) | N(heighth2,  1,  0)                                                            ) // ProgGuide 4.10 p.105
    ENC(BITMAP_SOURCE,     (uint32_t addr22),                                                                           (addr22),                                   N(addr22,   21,  0)                                                                                  ) // ProgGuide 4.11 p.106
    ENC(BITMAP_TRANSFORM_A,(uint32_t a17),                                                                              (a17),                                      N(a17,      16,  0)                                                                                  ) // ProgGuide 4.12 p.108
    ENC(BITMAP_TRANSFORM_B,(uint32_t b17),                                                                              (b17),                                      N(b17,      16,  0)                                                                                  ) // ProgGuide 4.13 p.109
    ENC(BITMAP_TRANSFORM_C,(uint32_t c24),                                                                              (c24),                                      N(c24,      23,  0)                                                                                  ) // ProgGuide 4.14 p.110
    ENC(BITMAP_TRANSFORM_D,(uint32_t d17),                                                                              (d17),                                      N(d17,      16,  0)                                                                                  ) // ProgGuide 4.15 p.111
    ENC(BITMAP_TRANSFORM_E,(uint32_t e17),                                                                              (e17),                                      N(e17,      16,  0)                                                                                  ) // ProgGuide 4.16 p.112
    ENC(BITMAP_TRANSFORM_F,(uint32_t f24),                                                                              (f24),                                      N(f24,      23,  0)                                                                                  ) // ProgGuide 4.17 p.113
    ENC(BLEND_FUNC,        (BLEND    src,      BLEND    dst),                                                           (src, dst),                                 N(src,       5,  3) | N(dst,       2,  0)                                                            ) // ProgGuide 4.18 p.114
    ENC(CALL,              (uint32_t dest22),                                                                           (dest22),                                   N(dest22,   15,  0)                                                                                  ) // ProgGuide 4.19 p.116
    ENC(CELL,              (uint8_t  cell7),                                                                            (cell7),                                    N(cell7,     6,  0)                                                                                  ) // ProgGuide 4.20 p.117
    ENC(CLEAR,             (uint8_t  color1,   uint8_t  stencil1, uint8_t  tag1),                                       (color1, stencil1, tag1),                   N(color1,    2,  2) | N(stencil1,  1,  1) | N(tag1,     0,  0)                                       ) // ProgGuide 4.21 p.118
    ENC(CLEAR_COLOR_A,     (uint8_t  alpha8),                                                                           (alpha8),                                   N(alpha8,    7,  0)                                                                                  ) // ProgGuide 4.22 p.120
    ENC(CLEAR_COLOR_RGB,   (uint8_t  red8,     uint8_t  green8,   uint8_t  blue8),                                      (red8, green8, blue8),                      N(red8,     23, 16) | N(green8,   15,  8) | N(blue8,    7,  0)                                       ) // ProgGuide 4.23 p.121
    ENC(CLEAR_COLOR,       (uint32_t rgb24),                                                                            (rgb24),                                    N(rgb24,    23,  0)                                                                                  ) // ProgGuide 4.23 p.121
    ENC(CLEAR_STENCIL,     (uint8_t  stencil8),                                                                         (stencil8),                                 N(stencil8,  7,  0)                                                                                  ) // ProgGuide 4.24 p.122
    ENC(CLEAR_TAG,         (uint8_t  tag8),                                                                             (tag8),                                     N(tag8,      7,  0)                                                                                  ) // ProgGuide 4.25 p.123
    ENC(COLOR_A,           (uint8_t  alpha8),                                                                           (alpha8),                                   N(alpha8,    7,  0)                                                                                  ) // ProgGuide 4.26 p.124
    ENC(COLOR_MASK,        (uint8_t  red1,     uint8_t  green1,   uint8_t  blue1,   uint8_t  alpha1),                   (red1, green1, blue1, alpha1),              N(red1,      3,  3) | N(green1,    2,  2) | N(blue1,    1,  1) | N(alpha1,  0,  0)                   ) // ProgGuide 4.27 p.125
    ENC(COLOR_RGB,         (uint8_t  red8,     uint8_t  green8,   uint8_t  blue8),                                      (red8, green8, blue8),                      N(red8,     23, 16) | N(green8,   15,  8) | N(blue8,    7,  0)                                       ) // ProgGuide 4.28 p.126
    ENC(COLOR,             (uint32_t rgb24),                                                                            (rgb24),                                    N(rgb24,    23,  0)                                                                                  ) // ProgGuide 4.28 p.126
    ENC(DISPLAY,           (),                                                                                          (),                                         0                                                                                                    ) // ProgGuide 4.29 p.127
    ENC(END,               (),                                                                                          (),                                         0                                                                                                    ) // ProgGuide 4.30 p.128
    ENC(JUMP,              (uint16_t dest16),                                                                           (dest16),                                   N(dest16,   15,  0)                                                                                  ) // ProgGuide 4.31 p.129
    ENC(LINE_WIDTH,        (uint16_t width12),                                                                          (width12),                                  N(width12,  11,  0)                                                                                  ) // ProgGuide 4.32 p.130
    ENC(MACRO,             (uint8_t  index1),                                                                           (index1),                                   N(index1,    0,  0)                                                                                  ) // ProgGuide 4.33 p.131
    ENC(NOP,               (),                                                                                          (),                                         0                                                                                                    ) // ProgGuide 4.34 p.131
    ENC(PALETTE_SOURCE,    (uint32_t addr22),                                                                           (addr22),                                   N(addr22,   21,  0)                                                                                  ) // ProgGuide 4.35 p.132
    ENC(POINT_SIZE,        (uint16_t size13),                                                                           (size13),                                   N(size13,   12,  0)                                                                                  ) // ProgGuide 4.36 p.133
    ENC(RESTORE_CONTEXT,   (),                                                                                          (),                                         0                                                                                                    ) // ProgGuide 4.37 p.134
    ENC(RETURN,            (),                                                                                          (),                                         0                                                                                                    ) // ProgGuide 4.38 p.135
    ENC(SAVE_CONTEXT,      (),                                                                                          (),                                         0                                                                                                    ) // ProgGuide 4.39 p.136
    ENC(SCISSOR_SIZE,      (uint16_t width12,  uint16_t height12),                                                      (width12, height12),                        N(width12,  23, 12) | N(height12, 11,  0)                                                            ) // ProgGuide 4.40 p.137
    ENC(SCISSOR_XY,        (uint16_t x11,      uint16_t y11),                                                           (x11, y11),                                 N(x11,      21, 11) | N(y11,      10,  0)                                                            ) // ProgGuide 4.41 p.138
    ENC(STENCIL_FUNC,      (FUNC     func,     uint8_t  ref8,     uint8_t  mask8),                                      (func, ref8, mask8),                        N(func,     19, 16) | N(ref8,     15,  8) | N(mask8,    7,  0)                                       ) // ProgGuide 4.42 p.139
    ENC(STENCIL_MASK,      (uint8_t  mask8),                                                                            (mask8),                                    N(mask8,     7,  0)                                                                                  ) // ProgGuide 4.43 p.140
    ENC(STENCIL_OP,        (STENCIL  sfail,    STENCIL  spass),                                                         (sfail, spass),                             N(sfail,     5,  3) | N(spass,     2,  0)                                                            ) // ProgGuide 4.44 p.141
    ENC(TAG,               (uint8_t  tag8),                                                                             (tag8),                                     N(tag8,      7,  0)                                                                                  ) // ProgGuide 4.45 p.143
    ENC(TAG_MASK,          (uint8_t  mask1),                                                                            (mask1),                                    N(mask1,     0,  0)                                                                                  ) // ProgGuide 4.46 p.144
    ENC(VERTEX2F,          (uint16_t x15,      uint16_t y15),                                                           (x15, y15),                                 N(x15,      29, 15) | N(y15,      14,  0)                                                            ) // ProgGuide 4.47 p.145
    ENC(VERTEX2II,         (uint16_t x9,       uint16_t y9,       uint8_t  handle5, uint8_t  cell6),                    (x9, y9, handle5, cell6),                   N(x9,       29, 21) | N(y9,       20, 12) | N(handle5, 11,  7) | N(cell6,  6,  0)                    ) // ProgGuide 4.48 p.146
    ENC(VERTEX_FORMAT,     (uint8_t  frac3),                                                                            (frac3),                                    N(frac3,     2,  0)                                                                                  ) // ProgGuide 4.49 p.147
    ENC(VERTEX_TRANSLATE_X,(uint32_t x17),                                                                              (x17),                                      N(x17,      16,  0)                                                                                  ) // ProgGuide 4.50 p.148
    ENC(VERTEX_TRANSLATE_Y,(uint32_t y17),                                                                              (y17),                                      N(y17,      16,  0)                                                                                  ) // ProgGuide 4.51 p.149
    #undef ENC
    #undef N

    //-----------------------------------------------------------------------
    // Graphics coprocessor commands
    //
    // Co-processor commands are encoded as bytes (not bits) so they can't
    // be encoded the same way as display list commands.
    //
    // The following macros are used to generate functions that start with
    // cmd_ and send the given command and parameters to the co-processor.
    //
    // 2 byte input value
    #define V2(value) (Send16((uint16_t)(value)), result += 2)
    // 4 byte input value
    #define V4(value) (Send32((uint32_t)(value)), result += 4)
    // String value
    #define SS(value, maxlen) (result += SendString(value, maxlen))
    // Program memory string value
    #define SF(value, maxlen) (result += SendStringF(value, maxlen))
    // Transfer from host RAM
    #define MM(value, len) (result += SendData(value, len))
    // 4 byte output value: Store Cmd index to parameter and bump cmd index
    #define Q4(name) ((*name = _cmd_index + (result % RAM_CMD_SIZE)), V4(0))
    // Send command and data, keep EVE selected
    #define CMDSEL(name, declaration, value) \
        uint16_t cmd_##name declaration { CmdSelect(ENC_CMD_##name); int16_t result = 0; return BumpCmdIndex(value); }
    // Send command and de-select
    #define CMD(name, declaration, value) \
        uint16_t cmd_##name declaration { CmdSelect(ENC_CMD_##name); int16_t result = 0; BumpCmdIndex(value); Select(false); return result; }
    // Send command that has outputs (To be changed later)
    #define CMDOUT CMDSEL
    CMD(DLSTART,        (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.11 p.162
    CMD(SWAP,           (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.12 p.163
    CMD(COLDSTART,      (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.13 p.163
    CMD(INTERRUPT,      (uint32_t ms32),                                                                                                                (V4(ms32)                                                                           )) // ProgGuide 5.14 p.164
    CMD(APPEND,         (uint32_t ptr32, uint32_t num32),                                                                                               (V4(ptr32),V4(num32)                                                                )) // ProgGuide 5.15 p.165
    CMD(REGREAD,        (uint32_t ptr32, uint32_t result32),                                                                                            (V4(ptr32),V4(result32)                                                             )) // ProgGuide 5.16 p.166
    CMD(MEMWRITE,       (uint32_t ptr32, uint32_t num, const uint8_t *data),                                                                            (V4(ptr32),V4(num),MM(data, num)                                                    )) // ProgGuide 5.17 p.167
    CMDSEL(INFLATE,     (uint32_t ptr32, uint32_t num, const uint8_t *data),                                                                            (V4(ptr32),MM(data, num)                                                            )) // ProgGuide 5.18 p.168
    CMDSEL(LOADIMAGE,   (uint32_t ptr32, OPT options32, uint32_t num, const uint8_t *data),                                                             (V4(ptr32),V4(options32),MM(data, num)                                              )) // ProgGuide 5.19 p.169
    CMD(MEDIAFIFO,      (uint32_t ptr32, uint32_t size32),                                                                                              (V4(ptr32),V4(size32)                                                               )) // ProgGuide 5.20 p.170
    CMD(PLAYVIDEO,      (OPT options),                                                                                                                  (V4(options)                                                                        )) // ProgGuide 5.21 p.171
    CMD(VIDEOSTART,     (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.22 p.172
    CMD(VIDEOFRAME,     (uint32_t dst32, uint32_t ptr32),                                                                                               (V4(dst32),V4(ptr32)                                                                )) // ProgGuide 5.23 p.173
    CMDOUT(MEMCRC,      (uint32_t ptr32, uint32_t num32, uint16_t *xresult32),                                                                          (V4(ptr32),V4(num32),Q4(xresult32)                                                  )) // ProgGuide 5.24 p.173
    CMD(MEMZERO,        (uint32_t ptr32, uint32_t num32),                                                                                               (V4(ptr32),V4(num32)                                                                )) // ProgGuide 5.25 p.174
    CMD(MEMSET,         (uint32_t ptr32, uint32_t value8, uint32_t num32),                                                                              (V4(ptr32),V4(value8),V4(num32)                                                     )) // ProgGuide 5.26 p.175
    CMD(MEMCPY,         (uint32_t dest32, uint32_t src32, uint32_t num32),                                                                              (V4(dest32),V4(src32),V4(num32)                                                     )) // ProgGuide 5.27 p.176
    CMD(BUTTON,         (int16_t x16, int16_t y16, int16_t w16, int16_t h16, int16_t font5, OPT options, const char *message, uint16_t len = 0),        (V2(x16),V2(y16),V2(w16),V2(h16),V2(font5),V2(options),SS(message, len)             )) // ProgGuide 5.28 p.176
    CMD(CLOCK,          (int16_t x16, int16_t y16, int16_t r16, OPT options, uint16_t h16, uint16_t m16, uint16_t s16, uint16_t ms16),                  (V2(x16),V2(y16),V2(r16),V2(options),V2(h16),V2(m16),V2(s16),V2(ms16)               )) // ProgGuide 5.29 p.179
    CMD(FGCOLOR,        (uint32_t c24),                                                                                                                 (V4(c24)                                                                            )) // ProgGuide 5.30 p.183
    CMD(BGCOLOR,        (uint32_t c24),                                                                                                                 (V4(c24)                                                                            )) // ProgGuide 5.31 p.184
    CMD(GRADCOLOR,      (uint32_t c24),                                                                                                                 (V4(c24)                                                                            )) // ProgGuide 5.32 p.185
    CMD(GAUGE,          (int16_t x16, int16_t y16, int16_t r16, OPT options, uint16_t major16, uint16_t minor16, uint16_t val16, uint16_t range16),     (V2(x16),V2(y16),V2(r16),V2(options),V2(major16),V2(minor16),V2(val16),V2(range16)  )) // ProgGuide 5.33 p.187
    CMD(GRADIENT,       (int16_t x016, int16_t y016, uint32_t rgb024, int16_t x116, int16_t y116, int32_t rgb124),                                      (V2(x016),V2(y016),V4(rgb024),V2(x116),V2(y116),V4(rgb124)                          )) // ProgGuide 5.34 p.193
    CMD(KEYS,           (int16_t x16, int16_t y16, int16_t w16, int16_t h16, int16_t font5, OPT options, const char *message, uint16_t len = 0),        (V2(x16),V2(y16),V2(w16),V2(h16),V2(options),SS(message, len)                       )) // ProgGuide 5.35 p.196
    CMD(PROGRESS,       (int16_t x16, int16_t y16, int16_t w16, int16_t h16, OPT options, uint16_t val16, uint16_t range16),                            (V2(x16),V2(y16),V2(w16),V2(h16),V2(options),V2(val16),V2(range16),V2(0)            )) // ProgGuide 5.36 p.200
    CMD(SCROLLBAR,      (int16_t x16, int16_t y16, int16_t w16, int16_t h16, OPT options, uint16_t val16, uint16_t size16, uint16_t range16),           (V2(x16),V2(y16),V2(w16),V2(h16),V2(options),V2(val16),V2(size16),V2(range16)       )) // ProgGuide 5.37 p.201
    CMD(SLIDER,         (int16_t x16, int16_t y16, int16_t w16, int16_t h16, OPT options, uint16_t val16, uint16_t range16),                            (V2(x16),V2(y16),V2(w16),V2(h16),V2(options),V2(val16),V2(range16),V2(0)            )) // ProgGuide 5.38 p.205
    CMD(DIAL,           (int16_t x16, int16_t y16, int16_t r16, OPT options, uint16_t val16),                                                           (V2(x16),V2(y16),V2(r16),V2(options),V2(val16),V2(0)                                )) // ProgGuide 5.39 p.207
    CMD(TOGGLE,         (int16_t x16, int16_t y16, int16_t w16, uint16_t font5, OPT options, uint16_t state16, const char *message, uint16_t len = 0),  (V2(x16),V2(y16),V2(w16),V2(font5),V2(options),V2(state16),SS(message, len)         )) // ProgGuide 5.40 p.210
    CMD(TEXT,           (int16_t x16, int16_t y16, int16_t font5, OPT options, const char *message, uint16_t len = 0),                                  (V2(x16),V2(y16),V2(font5),V2(options),SS(message, len)                             )) // ProgGuide 5.41 p.213
    CMD(SETBASE,        (uint32_t b6),                                                                                                                  (V4(b6)                                                                             )) // ProgGuide 5.42 p.216
    CMD(NUMBER,         (int16_t x16, uint16_t y16, int16_t font5, OPT options, int32_t n32),                                                           (V2(x16),V2(y16),V2(font5),V2(options),V4(n32)                                      )) // ProgGuide 5.43 p.217
    CMD(LOADIDENTITY,   (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.44 p.220
    CMD(SETMATRIX,      (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.45 p.220
    CMDOUT(GETMATRIX,   (uint16_t *xa32, uint16_t *xb32, uint16_t *xc32, uint16_t *xd32, int16_t *xe32, int16_t *xf32),                                 (Q4(xa32),Q4(xb32),Q4(xc32),Q4(xd32),Q4(xe32),Q4(xf32)                              )) // ProgGuide 5.46 p.221
    CMDOUT(GETPTR,      (uint16_t *xptr),                                                                                                               (Q4(xptr)                                                                           )) // ProgGuide 5.47 p.222
    CMDOUT(GETPROPS,    (uint16_t *xptr32, uint16_t *xwidth32, uint16_t *xheight32),                                                                    (Q4(xptr32),Q4(xwidth32),Q4(xheight32)                                              )) // ProgGuide 5.48 p.223
    CMD(SCALE,          (int32_t sx32, int32_t sy32),                                                                                                   (V4(sx32),V4(sy32)                                                                  )) // ProgGuide 5.49 p.223
    CMD(ROTATE,         (int32_t a32),                                                                                                                  (V4(a32)                                                                            )) // ProgGuide 5.50 p.225
    CMD(TRANSLATE,      (int32_t tx32, int32_t ty32),                                                                                                   (V4(tx32),V4(ty32)                                                                  )) // ProgGuide 5.51 p.226
    CMDOUT(CALIBRATE,   (uint16_t *xresult32),                                                                                                          (Q4(xresult32)                                                                      )) // ProgGuide 5.52 p.227
    CMD(SETROTATE,      (uint32_t r32),                                                                                                                 (V4(r32)                                                                            )) // ProgGuide 5.53 p.228
    CMD(SPINNER,        (int16_t x16, int16_t y16, uint16_t style2, uint16_t scale2),                                                                   (V2(x16),V2(y16),V2(style2),V2(scale2)                                              )) // ProgGuide 5.54 p.229
    CMD(SCREENSAVER,    (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.55 p.233
    CMD(SKETCH,         (int16_t x16, int16_t y16, uint16_t w16, uint16_t h16, uint32_t ptr32, FORMAT format),                                          (V2(x16),V2(y16),V2(w16),V2(h16),V4(ptr32),V2(format),V2(0)                         )) // ProgGuide 5.55 p.234
    CMD(STOP,           (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.57 p.236
    CMD(SETFONT,        (uint32_t font5, uint32_t ptr32),                                                                                               (V4(font5),V4(ptr32)                                                                )) // ProgGuide 5.58 p.237
    CMD(SETFONT2,       (uint32_t font5, uint32_t ptr32, uint32_t firstchar8),                                                                          (V4(font5),V4(ptr32),V4(firstchar8)                                                 )) // ProgGuide 5.59 p.237
    CMD(SETSCRATCH,     (uint32_t handle5),                                                                                                             (V4(handle5)                                                                        )) // ProgGuide 5.60 p.239
    CMD(ROMFONT,        (uint32_t font5, uint32_t romslot6),                                                                                            (V4(font5),V4(romslot6)                                                             )) // ProgGuide 5.61 p.240
    CMD(TRACK,          (int16_t x16, int16_t y16, int16_t w16, int16_t h16, int16_t tag8),                                                             (V2(x16),V2(y16),V2(w16),V2(h16),V2(tag8),V2(0)                                     )) // ProgGuide 5.62 p.240
    CMD(SNAPSHOT,       (uint32_t ptr32),                                                                                                               (V4(ptr32)                                                                          )) // ProgGuide 5.63 p.245
    CMD(SNAPSHOT2,      (FORMAT format, uint32_t ptr32, int16_t x16, int16_t y16, int16_t w16, int16_t h16),                                            (V4(format),V4(ptr32),V2(x16),V2(y16),V2(w16),V2(h16)                               )) // ProgGuide 5.64 p.246
    CMD(SETBITMAP,      (uint32_t addr32, FORMAT format, uint16_t width16, uint16_t height16),                                                          (V4(addr32),V2(format),V2(width16),V2(height16),V2(0)                               )) // ProgGuide 5.65 p.247
    CMD(LOGO,           (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.66 p.249
    CMD(CSKETCH,        (int16_t x16, int16_t y16, uint16_t w16, uint16_t h16, uint32_t ptr32, FORMAT format, uint16_t freq16),                         (V2(x16),V2(y16),V2(w16),V2(h16),V4(ptr32),V2(format),V2(freq16)                    )) // ProgGuide 5.67 p.249

    /////////////////////////////////////////////////////////////////////////
    // CO-PROCESSOR HELPER FUNCTIONS
    /////////////////////////////////////////////////////////////////////////
    // The following functions combine some commands that are often
    // used together

public:
    //-----------------------------------------------------------------------
    // Finish the current display list and swap and execute
    uint16_t                            // Returns updated Cmd index
    CmdDlFinish(
        bool waituntilcomplete = false) // True = wait until done
    {
        cmd_DISPLAY();

        Cmd(ENC_CMD_SWAP);

        return CmdExecute(waituntilcomplete);
    }

public:
    //-----------------------------------------------------------------------
    // Draw a point at the given location
    uint16_t                            // Returns updated Cmd index
    Point(
        uint16_t point_x,               // X coordinate
        uint16_t point_y,               // Y coordinate
        uint16_t ball_size)             // Diameter
    {
        // Select the size of the dot to draw
        cmd_POINT_SIZE(ball_size);

        // Indicate to draw a point (dot)
        cmd_BEGIN(BEGIN_POINTS);

        // Set the point center location
        cmd_VERTEX2F(point_x, point_y);

        // End the point
        cmd_END();

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Draw a line between two points
    uint16_t                            // Returns updated Cmd index
    Line(
        uint16_t x0,                    // Start X
        uint16_t y0,                    // Start Y
        uint16_t x1,                    // End X
        uint16_t y1,                    // End Y
        uint16_t width)                 // Line thickness
    {
        //Set the line width
        cmd_LINE_WIDTH(width);

        // Start a line
        cmd_BEGIN(BEGIN_LINES);

        // Set the first point
        cmd_VERTEX2F(x0, y0);

        // Set the second point
        cmd_VERTEX2F(x1, y1);

        // End the line
        cmd_END();

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Draw a closed rectangle
    uint16_t                            // Returns updated Cmd index
    FilledRectangle(
        uint16_t x0,                    // X coord of 1st pt in cur precision
        uint16_t y0,                    // Y coord of 1st pt in cur precision
        uint16_t x1,                    // X coord of 2nd pt in cur precision
        uint16_t y1)                    // Y coord of 2nd pt in cur precision
    {
        //Set the line width (16/16 of a pixel--appears to be about as sharp as it gets)
        cmd_LINE_WIDTH(16); // TODO: track current precision

        // Start a rectangle
        cmd_BEGIN(BEGIN_RECTS);

        // Set the first point
        cmd_VERTEX2F(x0, y0);

        // Set the second point
        cmd_VERTEX2F(x1, y1);

        // End the rectangle
        cmd_END();

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Draw an open rectangle
    uint16_t                            // Returns updated Cmd index
    OpenRectangle(
        uint16_t x0,                    // X coord of 1st pt in cur precision
        uint16_t y0,                    // Y coord of 1st pt in cur precision
        uint16_t x1,                    // X coord of 2nd pt in cur precision
        uint16_t y1,                    // Y coord of 2nd pt in cur precision
        uint16_t width)                 // Line width in cur precision
    {
        // Set the line width
        cmd_LINE_WIDTH(width*16);

        // Start a line set
        cmd_BEGIN(BEGIN_LINES);

        // Top
        cmd_VERTEX2F(x0, y0);
        cmd_VERTEX2F(x1, y0);

        //Right
        cmd_VERTEX2F(x1, y0);
        cmd_VERTEX2F(x1, y1);

        //Bottom
        cmd_VERTEX2F(x1, y1);
        cmd_VERTEX2F(x0, y1);

        //Left
        cmd_VERTEX2F(x0, y1);
        cmd_VERTEX2F(x0, y0);

        // End the line set
        cmd_END();

        return _cmd_index;
    }

};
