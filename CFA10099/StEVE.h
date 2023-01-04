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

    // Display list command encoding
    #define E(value, mask, shift) (((value) & (mask)) << (shift))
    #define C(command) ((command) << 24)
    #define ENC(name, ...) uint32_t ENC_##name(__VA_ARGS__)
    ENC(ALPHA_FUNC        , uint32_t func, uint32_t ref)                                                        { return (C(0x09) | E(func,       0x07UL,       8)  | E(ref,        0xffUL,   0))                                                                          ; }
    ENC(BEGIN             , uint32_t prim)                                                                      { return (C(0x1f) | E(prim,       0x0FUL,       0))                                                                                                        ; }
    ENC(BITMAP_HANDLE     , uint32_t handle)                                                                    { return (C(0x05) | E(handle,     0x1fUL,       0))                                                                                                        ; }
    ENC(BITMAP_LAYOUT_H   , uint32_t linestride, uint32_t height)                                               { return (C(0x28) | E(linestride, 0x03UL,       2)  | E(height,     0x3UL,    0))                                                                          ; }
    ENC(BITMAP_LAYOUT     , uint32_t format, uint32_t linestride, uint32_t height)                              { return (C(0x07) | E(format,     0x1fUL,       19) | E(linestride, 0x3ffUL,  9)  | E(height, 0x1ffUL, 0))                                                 ; }
    ENC(BITMAP_SIZE_H     , uint32_t width, uint32_t height)                                                    { return (C(0x29) | E(width,      0x3UL,        2)  | E(height,     0x3UL,    0))                                                                          ; }
    ENC(BITMAP_SIZE       , uint32_t filter, uint32_t wrapx, uint32_t wrapy, uint32_t width, uint32_t height)   { return (C(0x08) | E(filter,     0x1UL,        20) | E(wrapx,      0x1UL,    19) | E(wrapy,  0x1UL,   18) | E(width, 0x1ffUL, 9) | E(height, 0x1ffUL, 0)) ; }
    ENC(BITMAP_SOURCE     , uint32_t addr)                                                                      { return (C(0x01) | E(addr,       0x3FFFFFUL,   0))                                                                                                        ; }
    ENC(BITMAP_TRANSFORM_A, uint32_t a)                                                                         { return (C(0x15) | E(a,          0x1FFFFUL,    0))                                                                                                        ; }
    ENC(BITMAP_TRANSFORM_B, uint32_t b)                                                                         { return (C(0x16) | E(b,          0x1FFFFUL,    0))                                                                                                        ; }
    ENC(BITMAP_TRANSFORM_C, uint32_t c)                                                                         { return (C(0x17) | E(c,          0xFFFFFFUL,   0))                                                                                                        ; }
    ENC(BITMAP_TRANSFORM_D, uint32_t d)                                                                         { return (C(0x18) | E(d,          0x1FFFFUL,    0))                                                                                                        ; }
    ENC(BITMAP_TRANSFORM_E, uint32_t e)                                                                         { return (C(0x19) | E(e,          0x1FFFFUL,    0))                                                                                                        ; }
    ENC(BITMAP_TRANSFORM_F, uint32_t f)                                                                         { return (C(0x1a) | E(f,          0xFFFFFFUL,   0))                                                                                                        ; }
    ENC(BLEND_FUNC        , uint32_t src, uint32_t dst)                                                         { return (C(0x0b) | E(src,        0x7UL,        3)  | E(dst,        0x7UL,    0))                                                                          ; }
    ENC(CALL              , uint32_t dest)                                                                      { return (C(0x1d) | E(dest,       0xFFFFUL,     0))                                                                                                        ; }
    ENC(CELL              , uint32_t cell)                                                                      { return (C(0x06) | E(cell,       0x7fUL,       0))                                                                                                        ; }
    ENC(CLEAR_COLOR_A     , uint32_t alpha)                                                                     { return (C(0x0f) | E(alpha,      0xffUL,       0))                                                                                                        ; }
    ENC(CLEAR_COLOR_RGB   , uint32_t red, uint32_t green, uint32_t blue)                                        { return (C(0x02) | E(red,        0xffUL,       16) | E(green,      0xffUL,   8)  | E(blue,   0xffUL,  0))                                                 ; }
    ENC(CLEAR_COLOR       , uint32_t c)                                                                         { return (C(0x02) | E(c,          0x00ffffffUL, 0))                                                                                                        ; }
    ENC(CLEAR_STENCIL     , uint32_t s)                                                                         { return (C(0x11) | E(s,          0xffUL,       0))                                                                                                        ; }
    ENC(CLEAR_TAG         , uint32_t s)                                                                         { return (C(0x12) | E(s,          0xffUL,       0))                                                                                                        ; }
    ENC(CLEAR             , uint32_t c, uint32_t s, uint32_t t)                                                 { return (C(0x26) | E(c,          0x1UL,        2)  | E(s,          0x1UL,    1)  | E(t,      0x1UL,   0))                                                 ; }
    ENC(COLOR_A           , uint32_t alpha)                                                                     { return (C(0x10) | E(alpha,      0xffUL,       0))                                                                                                        ; }
    ENC(COLOR_MASK        , uint32_t r, uint32_t g, uint32_t b, uint32_t a)                                     { return (C(0x20) | E(r,          0x1UL,        3)  | E(g,          0x1UL,    2)  | E(b,      0x1UL,   1)  | E(a,     0x1UL, 0))                           ; }
    ENC(COLOR_RGB         , uint32_t red, uint32_t green, uint32_t blue)                                        { return (C(0x04) | E(red,        0xffUL,       16) | E(green,      0xffUL,   8)  | E(blue,   0xffUL,  0))                                                 ; }
    ENC(COLOR             , uint32_t c)                                                                         { return (C(0x04) | E(c,          0x00ffffffUL, 0))                                                                                                        ; }
    ENC(DISPLAY           , )                                                                                   { return (C(0x00))                                                                                                                                         ; }
    ENC(END               , )                                                                                   { return (C(0x21))                                                                                                                                         ; }
    ENC(JUMP              , uint32_t dest)                                                                      { return (C(0x1e) | E(dest,       0xFFFFUL,     0))                                                                                                        ; }
    ENC(LINE_WIDTH        , uint32_t width)                                                                     { return (C(0x0e) | E(width,      0xFFFUL,      0))                                                                                                        ; }
    ENC(MACRO             , uint32_t m)                                                                         { return (C(0x25) | E(m,          0x1UL,        0))                                                                                                        ; }
    ENC(NOP               , )                                                                                   { return (C(0x2d))                                                                                                                                         ; }
    ENC(PALETTE_SOURCE    , uint32_t addr)                                                                      { return (C(0x2a) | E(addr,       0x3FFFFFUL,   0))                                                                                                        ; }
    ENC(POINT_SIZE        , uint32_t size)                                                                      { return (C(0x0d) | E(size,       0x1FFFUL,     0))                                                                                                        ; }
    ENC(RESTORE_CONTEXT   , )                                                                                   { return (C(0x23))                                                                                                                                         ; }
    ENC(RETURN            , )                                                                                   { return (C(0x24))                                                                                                                                         ; }
    ENC(SAVE_CONTEXT      , )                                                                                   { return (C(0x22))                                                                                                                                         ; }
    ENC(SCISSOR_SIZE      , uint32_t width, uint32_t height)                                                    { return (C(0x1c) | E(width,      0xFFFUL,      12) | E(height,     0xFFFUL,  0))                                                                          ; }
    ENC(SCISSOR_XY        , uint32_t x, uint32_t y)                                                             { return (C(0x1b) | E(x,          0x7FFUL,      11) | E(y,          0x7FFUL,  0))                                                                          ; }
    ENC(STENCIL_FUNC      , uint32_t func, uint32_t ref, uint32_t mask)                                         { return (C(0x0a) | E(func,       0x7UL,        16) | E(ref,        0xffUL,   8) | E(mask,    0xffUL, 0))                                                  ; }
    ENC(STENCIL_MASK      , uint32_t mask)                                                                      { return (C(0x13) | E(mask,       0xffUL,       0))                                                                                                        ; }
    ENC(STENCIL_OP        , uint32_t sfail, uint32_t spass)                                                     { return (C(0x0c) | E(sfail,      0x7UL,        3)  | E(spass,      0x7UL,    0))                                                                          ; }
    ENC(TAG_MASK          , uint32_t mask)                                                                      { return (C(0x14) | E(mask,       0x1UL,        0))                                                                                                        ; }
    ENC(TAG               , uint32_t s)                                                                         { return (C(0x03) | E(s,          0xffUL,       0))                                                                                                        ; }
    ENC(VERTEX_FORMAT     , uint32_t frac)                                                                      { return (C(0x27) | E(frac,       0x7UL,        0))                                                                                                        ; }
    ENC(VERTEX_TRANSLATE_X, uint32_t x)                                                                         { return (C(0x2b) | E(x,          0x1FFFFUL,    0))                                                                                                        ; }
    ENC(VERTEX_TRANSLATE_Y, uint32_t y)                                                                         { return (C(0x2c) | E(y,          0x1FFFFUL,    0))                                                                                                        ; }
    ENC(VERTEX2F          , uint32_t x, uint32_t y)                                                             { return (C(0x40) | E(x,          0xffffUL,     15) | E(y,          0xffffUL, 0))                                                                          ; }
    ENC(VERTEX2II         , uint32_t x, uint32_t y, uint32_t handle, uint32_t cell)                             { return (C(0x80) | E(x,          0x1ffUL,      21) | E(y,          0x1ffUL,  12) | E(handle, 0x1fUL, 7) | E(cell,    0x7fUL, 0))                          ; }
    #undef C
    #undef E

    // Graphics engine coprocessor commands
    const uint32_t ENC_CMD_APPEND              = 0xFFFFFF1EUL;
    const uint32_t ENC_CMD_BGCOLOR             = 0xFFFFFF09UL;
    const uint32_t ENC_CMD_BITMAP_TRANSFORM    = 0xFFFFFF21UL;
    const uint32_t ENC_CMD_BUTTON              = 0xFFFFFF0DUL;
    const uint32_t ENC_CMD_CALIBRATE           = 0xFFFFFF15UL;
    const uint32_t ENC_CMD_CLOCK               = 0xFFFFFF14UL;
    const uint32_t ENC_CMD_COLDSTART           = 0xFFFFFF32UL;
    const uint32_t ENC_CMD_CRC                 = 0xFFFFFF03UL;
    const uint32_t ENC_CMD_CSKETCH             = 0xFFFFFF35UL;
    const uint32_t ENC_CMD_DIAL                = 0xFFFFFF2DUL;
    const uint32_t ENC_CMD_DLSTART             = 0xFFFFFF00UL;
    const uint32_t ENC_CMD_EXECUTE             = 0xFFFFFF07UL;
    const uint32_t ENC_CMD_FGCOLOR             = 0xFFFFFF0AUL;
    const uint32_t ENC_CMD_GAUGE               = 0xFFFFFF13UL;
    const uint32_t ENC_CMD_GETMATRIX           = 0xFFFFFF33UL;
    const uint32_t ENC_CMD_GETPOINT            = 0xFFFFFF08UL;
    const uint32_t ENC_CMD_GETPROPS            = 0xFFFFFF25UL;
    const uint32_t ENC_CMD_GETPTR              = 0xFFFFFF23UL;
    const uint32_t ENC_CMD_GRADCOLOR           = 0xFFFFFF34UL;
    const uint32_t ENC_CMD_GRADIENT            = 0xFFFFFF0BUL;
    const uint32_t ENC_CMD_HAMMERAUX           = 0xFFFFFF04UL;
    const uint32_t ENC_CMD_IDCT_DELETED        = 0xFFFFFF06UL;
    const uint32_t ENC_CMD_INFLATE             = 0xFFFFFF22UL;
    const uint32_t ENC_CMD_INT_RAMSHARED       = 0xFFFFFF3DUL;
    const uint32_t ENC_CMD_INT_SWLOADIMAGE     = 0xFFFFFF3EUL;
    const uint32_t ENC_CMD_INTERRUPT           = 0xFFFFFF02UL;
    const uint32_t ENC_CMD_KEYS                = 0xFFFFFF0EUL;
    const uint32_t ENC_CMD_LOADIDENTITY        = 0xFFFFFF26UL;
    const uint32_t ENC_CMD_LOADIMAGE           = 0xFFFFFF24UL;
    const uint32_t ENC_CMD_LOGO                = 0xFFFFFF31UL;
    const uint32_t ENC_CMD_MARCH               = 0xFFFFFF05UL;
    const uint32_t ENC_CMD_MEDIAFIFO           = 0xFFFFFF39UL;
    const uint32_t ENC_CMD_MEMCPY              = 0xFFFFFF1DUL;
    const uint32_t ENC_CMD_MEMCRC              = 0xFFFFFF18UL;
    const uint32_t ENC_CMD_MEMSET              = 0xFFFFFF1BUL;
    const uint32_t ENC_CMD_MEMWRITE            = 0xFFFFFF1AUL;
    const uint32_t ENC_CMD_MEMZERO             = 0xFFFFFF1CUL;
    const uint32_t ENC_CMD_NUMBER              = 0xFFFFFF2EUL;
    const uint32_t ENC_CMD_PLAYVIDEO           = 0xFFFFFF3AUL;
    const uint32_t ENC_CMD_PROGRESS            = 0xFFFFFF0FUL;
    const uint32_t ENC_CMD_REGREAD             = 0xFFFFFF19UL;
    const uint32_t ENC_CMD_ROMFONT             = 0xFFFFFF3FUL;
    const uint32_t ENC_CMD_ROTATE              = 0xFFFFFF29UL;
    const uint32_t ENC_CMD_SCALE               = 0xFFFFFF28UL;
    const uint32_t ENC_CMD_SCREENSAVER         = 0xFFFFFF2FUL;
    const uint32_t ENC_CMD_SCROLLBAR           = 0xFFFFFF11UL;
    const uint32_t ENC_CMD_SETBASE             = 0xFFFFFF38UL;
    const uint32_t ENC_CMD_SETBITMAP           = 0xFFFFFF43UL;
    const uint32_t ENC_CMD_SETFONT             = 0xFFFFFF2BUL;
    const uint32_t ENC_CMD_SETFONT2            = 0xFFFFFF3BUL;
    const uint32_t ENC_CMD_SETMATRIX           = 0xFFFFFF2AUL;
    const uint32_t ENC_CMD_SETROTATE           = 0xFFFFFF36UL;
    const uint32_t ENC_CMD_SETSCRATCH          = 0xFFFFFF3CUL;
    const uint32_t ENC_CMD_SKETCH              = 0xFFFFFF30UL;
    const uint32_t ENC_CMD_SLIDER              = 0xFFFFFF10UL;
    const uint32_t ENC_CMD_SNAPSHOT            = 0xFFFFFF1FUL;
    const uint32_t ENC_CMD_SNAPSHOT2           = 0xFFFFFF37UL;
    const uint32_t ENC_CMD_SPINNER             = 0xFFFFFF16UL;
    const uint32_t ENC_CMD_STOP                = 0xFFFFFF17UL;
    const uint32_t ENC_CMD_SWAP                = 0xFFFFFF01UL;
    const uint32_t ENC_CMD_SYNC                = 0xFFFFFF42UL;
    const uint32_t ENC_CMD_TEXT                = 0xFFFFFF0CUL;
    const uint32_t ENC_CMD_TOGGLE              = 0xFFFFFF12UL;
    const uint32_t ENC_CMD_TOUCH_TRANSFORM     = 0xFFFFFF20UL;
    const uint32_t ENC_CMD_TRACK               = 0xFFFFFF2CUL;
    const uint32_t ENC_CMD_TRANSLATE           = 0xFFFFFF27UL;
    const uint32_t ENC_CMD_VIDEOFRAME          = 0xFFFFFF41UL;
    const uint32_t ENC_CMD_VIDEOSTART          = 0xFFFFFF40UL;

    // Constants for parameters to commands
    const uint32_t BEGIN_BITMAPS               = 0x00000001UL;
    const uint32_t BEGIN_EDGE_STRIP_A          = 0x00000007UL;
    const uint32_t BEGIN_EDGE_STRIP_B          = 0x00000008UL;
    const uint32_t BEGIN_EDGE_STRIP_L          = 0x00000006UL;
    const uint32_t BEGIN_EDGE_STRIP_R          = 0x00000005UL;
    const uint32_t BEGIN_LINE_STRIP            = 0x00000004UL;
    const uint32_t BEGIN_LINES                 = 0x00000003UL;
    const uint32_t BEGIN_POINTS                = 0x00000002UL;
    const uint32_t BEGIN_RECTS                 = 0x00000009UL;
    const uint32_t BLEND_DST_ALPHA             = 0x00000003UL;
    const uint32_t BLEND_ONE                   = 0x00000001UL;
    const uint32_t BLEND_ONE_MINUS_DST_ALPHA   = 0x00000005UL;
    const uint32_t BLEND_ONE_MINUS_SRC_ALPHA   = 0x00000004UL;
    const uint32_t BLEND_SRC_ALPHA             = 0x00000002UL;
    const uint32_t BLEND_ZERO                  = 0x00000000UL;
    const uint32_t DLSWAP_DONE                 = 0x00000000UL;
    const uint32_t DLSWAP_FRAME                = 0x00000002UL;
    const uint32_t DLSWAP_LINE                 = 0x00000001UL;
    const uint32_t FILTER_BILINEAR             = 0x00000001UL;
    const uint32_t FILTER_NEAREST              = 0x00000000UL;
    const uint32_t FORMAT_ARGB1555             = 0x00000000UL;
    const uint32_t FORMAT_ARGB2                = 0x00000005UL;
    const uint32_t FORMAT_ARGB4                = 0x00000006UL;
    const uint32_t FORMAT_BARGRAPH             = 0x0000000BUL;
    const uint32_t FORMAT_L1                   = 0x00000001UL;
    const uint32_t FORMAT_L2                   = 0x00000011UL;
    const uint32_t FORMAT_L4                   = 0x00000002UL;
    const uint32_t FORMAT_L8                   = 0x00000003UL;
    const uint32_t FORMAT_PALETTED             = 0x00000008UL;
    const uint32_t FORMAT_PALETTED4444         = 0x0000000FUL;
    const uint32_t FORMAT_PALETTED565          = 0x0000000EUL;
    const uint32_t FORMAT_PALETTED8            = 0x00000010UL;
    const uint32_t FORMAT_RGB332               = 0x00000004UL;
    const uint32_t FORMAT_RGB565               = 0x00000007UL;
    const uint32_t FORMAT_TEXT8X8              = 0x00000009UL;
    const uint32_t FORMAT_TEXTVGA              = 0x0000000AUL;
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
    const uint32_t OPT_CENTER                  = 0x00000600UL;
    const uint32_t OPT_CENTERX                 = 0x00000200UL;
    const uint32_t OPT_CENTERY                 = 0x00000400UL;
    const uint32_t OPT_FLAT                    = 0x00000100UL;
    const uint32_t OPT_FULLSCREEN              = 0x00000008UL;
    const uint32_t OPT_MEDIAFIFO               = 0x00000010UL;
    const uint32_t OPT_MONO                    = 0x00000001UL;
    const uint32_t OPT_NOBACK                  = 0x00001000UL;
    const uint32_t OPT_NODL                    = 0x00000002UL;
    const uint32_t OPT_NOHANDS                 = 0x0000C000UL;
    const uint32_t OPT_NOHM                    = 0x00004000UL;
    const uint32_t OPT_NOPOINTER               = 0x00004000UL;
    const uint32_t OPT_NOSECS                  = 0x00008000UL;
    const uint32_t OPT_NOTEAR                  = 0x00000004UL;
    const uint32_t OPT_NOTICKS                 = 0x00002000UL;
    const uint32_t OPT_RIGHTX                  = 0x00000800UL;
    const uint32_t OPT_SIGNED                  = 0x00000100UL;
    const uint32_t OPT_SOUND                   = 0x00000020UL;
    const uint32_t STENCIL_DECR                = 0x00000004UL;
    const uint32_t STENCIL_INCR                = 0x00000003UL;
    const uint32_t STENCIL_INVERT              = 0x00000005UL;
    const uint32_t STENCIL_KEEP                = 0x00000001UL;
    const uint32_t STENCIL_REPLACE             = 0x00000002UL;
    const uint32_t STENCIL_ZERO                = 0x00000000UL;
    const uint32_t TEST_ALWAYS                 = 0x00000007UL;
    const uint32_t TEST_EQUAL                  = 0x00000005UL;
    const uint32_t TEST_GEQUAL                 = 0x00000004UL;
    const uint32_t TEST_GREATER                = 0x00000003UL;
    const uint32_t TEST_LEQUAL                 = 0x00000002UL;
    const uint32_t TEST_LESS                   = 0x00000001UL;
    const uint32_t TEST_NEVER                  = 0x00000000UL;
    const uint32_t TEST_NOTEQUAL               = 0x00000006UL;
    const uint32_t TOUCHMODE_CONTINUOUS        = 0x00000003UL;
    const uint32_t TOUCHMODE_FRAME             = 0x00000002UL;
    const uint32_t TOUCHMODE_OFF               = 0x00000000UL;
    const uint32_t TOUCHMODE_ONESHOT           = 0x00000001UL;
    const uint32_t ULAW_SAMPLES                = 0x00000001UL;
    const uint32_t VOL_ZERO                    = 0x00000000UL;
    const uint32_t WRAP_BORDER                 = 0x00000000UL;
    const uint32_t WRAP_REPEAT                 = 0x00000001UL;
    const uint32_t ADC_DIFFERENTIAL            = 0x00000001UL;
    const uint32_t ADC_SINGLE_ENDED            = 0x00000000UL;
    const uint32_t ADPCM_SAMPLES               = 0x00000002UL;

    //-----------------------------------------------------------------------
    // "Host commands", see paragraph 4.1.5 p.16 of FT81X datasheet
    enum HOSTCMD
    {
        // Codes that are commented out weren't tested but should work
        // according to the data sheet.

        ACTIVE          = 0x00,
        STANDBY         = 0x41,
        PWRDOWN         = 0x50, // or 0x43
        //PD_ROMS         = 0x49,
        CLKEXT          = 0x44,
        CLKINT          = 0x48,
        CLKSEL          = 0x61, // or 0x62
        //RST_PULSE       = 0x68,
        //PINDRIVE        = 0x70,
        //PIN_PD_STATE    = 0x71,
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

    struct DisplayProfile
    {
        CLOCK       _clock;             // Clock source and multiplier
        CHIPID      _chipid;            // Expected chip ID; ANY=don't care
        uint32_t    _frequency;         // ClockFreq to store; 0 = don't store
        bool        _lcd10ma;           // True=drive LCD with 10mA (false=5)
        bool        _cspread;           // True=enable RGB clock spreading, see datasheet 4.4 p.27
        bool        _dither;            // True=enable dither, see datasheet 4.4 p.27
        uint16_t    _outbits;           // 3x3 bits indicating num LCD bits used, see datasheet 4.4 p.27

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
    DisplayProfile  _profile;           // Display init parameters
    SPIClass       &_spi;               // SPI interface
    SPISettings     _spi_settings;      // SPI settings
    int             _pin_cs;            // Chip Select Not Pin (-1 = none)
    int             _pin_pd;            // Power Down Not Pin (-1 = none)
    int             _pin_int;           // Interrupt Pin (-1 = none)

    uint16_t        _cmd_index;         // Graphics engine cmd write index
    //   (based on RAM_CMD)
    uint16_t        _dl_index;          // Display list write index
    //   (based on RAM_DL)

public:
    //-----------------------------------------------------------------------
    // Constructor
    StEVE(
        DisplayProfile &profile,        // Display profile
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
    {
        // Set the output pins before switching the pins to output, to
        // avoid glitches
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

public:
    //-----------------------------------------------------------------------
    // Get a pointer to the profile
    const DisplayProfile *Profile()
    {
        return &_profile;
    }

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
        if (_pin_cs >= 0)
        {
            // TODO use a member, and also use a member to keep track of transactions
            static uint8_t selected = 2;

            if (!!enable != selected)
            {
                selected = !!enable;
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

public:
    //-----------------------------------------------------------------------
    // Initialize the display
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
            Command_Write(CLKEXT);
            Command_Write(CLKSEL, (uint8_t)_profile._clock);
        }

        // Activate the FT81X and give it some time to initialize
        Command_Write(ACTIVE, 0);
        delay(40);

        // Repeatedly poll REG_ID with up to 250 maximum retries and a 1 ms
        // delay between retries.
        // The register should return 0x7C when the processor is running.
        if (!WaitReg8(REG_ID, 0x7C, 250, 1))
        {
            // TODO: report problem and return false
            DBG_STAT("Timeout waiting for ID of 0x7C.\n");
            DBG_STAT("Is the device connected? Is the right EVE device selected?\n");
            return false;
        }

        // Repeatedly poll REG_CPURESET until it returns 0 meaning the reset
        // is complete
        while (!WaitReg8(REG_CPURESET, 0, 250, 1))
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
        InitWritePointer();

        // Execute bug workarounds for specific subclasses
        // TODO: Evaluate result and return if false
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
        dl(ENC_CLEAR_COLOR_RGB(0, 0, 0));
        dl(ENC_CLEAR(1,1,1));// color, stencil, tags
        dl(ENC_DISPLAY());

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
    // Send a command
    //
    // Referred to as "host_command()" in the FT81X documentation
    void Command_Write(
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
    // Access memory of the FT81X
    //
    // Referred to as "Host Memory Read/Write" in the documentation
    void Select_and_Address(
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
    uint8_t                             // Returns data at given address
    REG_Read_8(
        uint32_t address)               // Address (22 bits; not checked)
    {
        uint8_t result;

        // Send the 24-bit address and operation flag.
        Select_and_Address(address, READ);

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
    uint16_t                            // Returns data at given address
    REG_Read_16(
        uint32_t address)               // Address (22 bits; not checked)
    {
        uint16_t result;
        
        // Send the 24-bit address and operation flag.
        Select_and_Address(address, READ);

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
    uint32_t                            // Returns data at given address
    REG_Read_32(
        uint32_t address)               // Address (22 bits; not checked)
    {
        uint32_t result;
        
        // Send the 24-bit address and operation flag.
        Select_and_Address(address, READ);

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
    WaitReg8(
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

        Select_and_Address(address, READ);

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
    // Write a 32 bit value in little-endian format
    void Send_32(
        uint32_t value)
    {
        _spi.transfer((uint8_t)(value));
        _spi.transfer((uint8_t)(value >> 8));
        _spi.transfer((uint8_t)(value >> 16));
        _spi.transfer((uint8_t)(value >> 24));
    }

protected:
    //-----------------------------------------------------------------------
    // Write an 8 bit register
    void REG_Write_8(
        uint32_t address,               // Address to store data
        uint8_t value)                  // Value to store
    {
        DBG_TRAFFIC("Writing %X = %02X\n", address, value);

        Select_and_Address(address, WRITE);

        _spi.transfer(value);

        Select(false);
    }

protected:
    //-----------------------------------------------------------------------
    // Write a 16 bit register
    void REG_Write_16(
        uint32_t address,               // Address to store data
        uint16_t value)                 // Value to store
    {
        DBG_TRAFFIC("Writing %X = %04X\n", address, value);

        Select_and_Address(address, WRITE);

        _spi.transfer((uint8_t)(value));
        _spi.transfer((uint8_t)(value >> 8));

        Select(false);
    }

protected:
    //-----------------------------------------------------------------------
    // Write a 32 bit register
    void REG_Write_32(
        uint32_t address,               // Address to store data
        uint32_t value)                 // Value to store
    {
        DBG_TRAFFIC("Writing %X = %08X\n", address, value);

        Select_and_Address(address, WRITE);

        Send_32(value);

        Select(false);
    }

protected:
    //-----------------------------------------------------------------------
    // Initialize the write pointer from the chip
    void InitWritePointer()
    {
        DBG_TRAFFIC("Reading REG_CMD_WRITE\n");

        _cmd_index = REG_Read_16(REG_CMD_WRITE);
    }

protected:
    //-----------------------------------------------------------------------
    // Get amount of free space in the command queue
    //
    // The number is based on the location where the chip is reading
    // (not writing).
    uint16_t                            // Returns number of bytes free
    GetFreeCmdSpace()
    {
        uint16_t result = ((4096 - 4) - ((_cmd_index - REG_Read_16(REG_CMD_READ)) & 0xFFF));

        DBG_TRAFFIC("Free command space is %u", result);

        return result;
    }

protected:
    //-----------------------------------------------------------------------
    // Reset the display list index
    //
    // NOTE: It shouldn't be necessary to get the display list index
    // without storing a command first, so there is no getdl function.
    void dlreset(
        uint16_t index = 0)             // New index
    {
        DBG_TRAFFIC("DL reset %u\n", index);

        // The optimizer should be smart enough to turn the mod operator
        // into an AND operation.
        _dl_index = index % RAM_DL_SIZE;
    }

protected:
    //-----------------------------------------------------------------------
    // Store a 32 bit display list command; does not select/deselect chip
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    uint16_t                            // Returns DL index after increment
    dl(
        uint32_t value)                 // Value to write
    {
        DBG_TRAFFIC("dl(%08X)\n", value);

        REG_Write_32(RAM_DL + _dl_index, value);

        // Update the index. The optimizer should be smart enough to turn
        // the mod operator into an AND operation.
        _dl_index = (_dl_index + 4) % RAM_DL_SIZE;

        return _dl_index;
    }

public:
    //-----------------------------------------------------------------------
    // Send a graphics engine command with no parameters
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    uint16_t                            // Returns Cmd index after increment
    Cmd(
        uint32_t command)               // Command to queue
    {
        DBG_GEEK("cmd(%08X)\n", command);

        Select_and_Address(RAM_CMD + _cmd_index, WRITE);

        // Send the command
        Send_32(command);

        Select(false);

        // Update the index. The optimizer should be smart enough to turn
        // the mod operator into an AND operation.
        _cmd_index = (_cmd_index + 4) % RAM_CMD_SIZE;

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Send a graphics engine command with one parameter
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    uint16_t                            // Returns Cmd index after increment
    Cmd(
        uint32_t command,               // Command to queue
        uint32_t data0)                 // First parameter
    {
        DBG_TRAFFIC("cmd(%08X, %08X)\n", command, data0);

        Select_and_Address(RAM_CMD + _cmd_index, WRITE);

        // Send the command
        Send_32(command);
        Send_32(data0);

        Select(false);

        // Update the index. The optimizer should be smart enough to turn
        // the mod operator into an AND operation.
        _cmd_index = (_cmd_index + 8) % RAM_CMD_SIZE;

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Send a graphics engine command with two parameters
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    uint16_t                            // Returns Cmd index after increment
    Cmd(
        uint32_t command,               // Command to queue
        uint32_t data0,                 // First parameter
        uint32_t data1)                 // Second parameter
    {
        DBG_TRAFFIC("cmd(%08X, %08X, %08X)\n", command, data0, data1);

        Select_and_Address(RAM_CMD + _cmd_index, WRITE);

        // Send the command
        Send_32(command);
        Send_32(data0);
        Send_32(data1);

        Select(false);

        // Update the index. The optimizer should be smart enough to turn
        // the mod operator into an AND operation.
        _cmd_index = (_cmd_index + 12) % RAM_CMD_SIZE;

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Send a graphics engine command with three parameters
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    uint16_t                            // Returns Cmd index after increment
    Cmd(
        uint32_t command,               // Command to queue
        uint32_t data0,                 // First parameter
        uint32_t data1,                 // Second parameter
        uint32_t data2)                 // Third parameter
    {
        DBG_TRAFFIC("cmd(%08X, %08X, %08X, %08X)\n", command, data0, data1, data2);

        Select_and_Address(RAM_CMD + _cmd_index, WRITE);

        // Send the command
        Send_32(command);
        Send_32(data0);
        Send_32(data1);
        Send_32(data2);

        Select(false);

        // Update the index. The optimizer should be smart enough to turn
        // the mod operator into an AND operation.
        _cmd_index = (_cmd_index + 16) % RAM_CMD_SIZE;

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Wait until the graphics engine has caught up.
    //
    // This can also be used to wait for the end of a frame, and to retrieve
    // the location where the next command will be stored without storing
    // another command first.
    uint16_t                            // Returns Cmd index
    WaitComplete()
    {
        DBG_TRAFFIC("Waiting for coprocessor\n");

        while (REG_Read_16(REG_CMD_READ) != _cmd_index);

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Tell the graphic engine to start executing commands
    //
    // This updates the write pointer on the engine to the current write
    // location so that the coprocessor starts executing commands in the
    // command queue.
    uint16_t                            // Returns Cmd index
    Execute(
        bool waituntilcomplete = false) // True = wait until done
    {
        DBG_TRAFFIC("Executing command queue\n");

        REG_Write_16(REG_CMD_WRITE, _cmd_index);

        if (waituntilcomplete)
        {
            WaitComplete();
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

        WaitComplete();

        Cmd(ENC_CMD_GETPTR, 0);

        Execute(true);

        result = REG_Read_32((_cmd_index - 4) % RAM_CMD_SIZE);

        DBG_TRAFFIC("RAM_G first free byte is at %08X\n", result);

        return result;
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
        Cmd(ENC_POINT_SIZE(ball_size));

        // Indicate to draw a point (dot)
        Cmd(ENC_BEGIN(BEGIN_POINTS));

        // Set the point center location
        Cmd(ENC_VERTEX2F(point_x, point_y));

        // End the point
        Cmd(ENC_END());

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
        Cmd(ENC_LINE_WIDTH(width * 16));

        // Start a line
        Cmd(ENC_BEGIN(BEGIN_LINES));

        // Set the first point
        Cmd(ENC_VERTEX2F(x0 * 16, y0 * 16));

        // Set the second point
        Cmd(ENC_VERTEX2F(x1 * 16, y1 * 16));

        // End the line
        Cmd(ENC_END());

        return _cmd_index;
    }

};
