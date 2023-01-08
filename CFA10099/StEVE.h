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
        CMD_DISPLAY                 = 0x00000000,
        CMD_BITMAP_SOURCE           = 0x01000000,
        CMD_CLEAR_COLOR_RGB         = 0x02000000,
        CMD_TAG                     = 0x03000000,
        CMD_COLOR_RGB               = 0x04000000,
        CMD_BITMAP_HANDLE           = 0x05000000,
        CMD_CELL                    = 0x06000000,
        CMD_BITMAP_LAYOUT           = 0x07000000,
        CMD_BITMAP_SIZE             = 0x08000000,
        CMD_ALPHA_FUNC              = 0x09000000,
        CMD_STENCIL_FUNC            = 0x0A000000,
        CMD_BLEND_FUNC              = 0x0B000000,
        CMD_STENCIL_OP              = 0x0C000000,
        CMD_POINT_SIZE              = 0x0D000000,
        CMD_LINE_WIDTH              = 0x0E000000,
        CMD_CLEAR_COLOR_A           = 0x0F000000,
        CMD_COLOR_A                 = 0x10000000,
        CMD_CLEAR_STENCIL           = 0x11000000,
        CMD_CLEAR_TAG               = 0x12000000,
        CMD_STENCIL_MASK            = 0x13000000,
        CMD_TAG_MASK                = 0x14000000,
        CMD_BITMAP_TRANSFORM_A      = 0x15000000,
        CMD_BITMAP_TRANSFORM_B      = 0x16000000,
        CMD_BITMAP_TRANSFORM_C      = 0x17000000,
        CMD_BITMAP_TRANSFORM_D      = 0x18000000,
        CMD_BITMAP_TRANSFORM_E      = 0x19000000,
        CMD_BITMAP_TRANSFORM_F      = 0x1A000000,
        CMD_SCISSOR_XY              = 0x1B000000,
        CMD_SCISSOR_SIZE            = 0x1C000000,
        CMD_CALL                    = 0x1D000000,
        CMD_JUMP                    = 0x1E000000,
        CMD_BEGIN                   = 0x1F000000,
        CMD_COLOR_MASK              = 0x20000000,
        CMD_END                     = 0x21000000,
        CMD_SAVE_CONTEXT            = 0x22000000,
        CMD_RESTORE_CONTEXT         = 0x23000000,
        CMD_RETURN                  = 0x24000000,
        CMD_MACRO                   = 0x25000000,
        CMD_CLEAR                   = 0x26000000,
        CMD_VERTEX_FORMAT           = 0x27000000,
        CMD_BITMAP_LAYOUT_H         = 0x28000000,
        CMD_BITMAP_SIZE_H           = 0x29000000,
        CMD_PALETTE_SOURCE          = 0x2A000000,
        CMD_VERTEX_TRANSLATE_X      = 0x2B000000,
        CMD_VERTEX_TRANSLATE_Y      = 0x2C000000,
        CMD_NOP                     = 0x2D000000,
        CMD_VERTEX2F                = 0x40000000, // Encoded in ProgGuide as 1<<22
        CMD_VERTEX2II               = 0x80000000, // Encoded in ProgGuide as 2<<22
    }   CMD;

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

    // Display list command encoding
    //
    // Following macro is used for easy encoding of a field in a 
    // uint32_t. The fields are shown in the Programmer's Guide with the
    // leftmost and rightmost bit numbers, so this macro should make it easy
    // to write and verify the encoding of the fields.
    //
    // Example: E(value, 10, 8) shifts the value 8 bits to the left after
    // masking out all but the lowest 3 bits (bits 10 to 8 inclusive is a
    // total of 3 bits).
    //
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
    // Because the macro is used with enum values in some cases, we replace
    // "value" with (uint32_t)value.
    //
    // We also apply extra parentheses to avoid problems with expression
    // evaluation order.
    //
    // The result looks pretty daunting but the optimizer will reduce it
    // to code that's basically equivalent to (value & constant) << constant.
    #define N(value, leftbit, rightbit) ((((uint32_t)value) & ((1 << (((leftbit) - (rightbit)) + 1)) - 1)) << rightbit)
    // Calculate command
    #define BODY(cmd, value) return ((uint32_t)CMD_##cmd) | value;
    // Print and calculate command (Very chatty!)
    #define DBG_BODY(cmd, value) uint32_t result = ((uint32_t)CMD_##cmd) | value; DBG_STAT("Command %18s=%08X=%s\n", STRINGIZE(cmd), result, STRINGIZE(value)); return result;

    // Functions to encode display list commands
    //
    // These have names in upper case to be consistent with the macros for
    // co-processor commands, and with code in other FT81X libraries. This
    // makes the names somewhat inconsistent, but it makes code easier to
    // port.
    //
    // Note: The names of the non-enum parameters include the actual number
    // of bits that are used, as a reminder. Keep in mind that some
    // parameters are encoded as unsigned or signed fixed-point value
    // and that negative fixed-point does not use 2's complement.
    uint32_t ENC_ALPHA_FUNC        (FUNC func, uint8_t ref8)                                                    { BODY(ALPHA_FUNC        , N(func,     10,  8) | N(ref8,      7,  0)                                                            ) } // ProgGuide 4.4 p.92
    uint32_t ENC_BEGIN             (BEGIN prim)                                                                 { BODY(BEGIN             , N(prim,      3,  0)                                                                                  ) } // ProgGuide 4.5 p.94
    uint32_t ENC_BITMAP_HANDLE     (uint8_t handle5)                                                            { BODY(BITMAP_HANDLE     , N(handle5,   4,  0)                                                                                  ) } // ProgGuide 4.6 p.96
    uint32_t ENC_BITMAP_LAYOUT     (FORMAT format, uint32_t stride10, uint32_t height9)                         { BODY(BITMAP_LAYOUT     , N(format,   23, 19) | N(stride10, 18,  9) | N(height9,  8,  0)                                       ) } // ProgGuide 4.7 p.97
    uint32_t ENC_BITMAP_LAYOUT_H   (uint32_t strideh2, uint32_t heighth2)                                       { BODY(BITMAP_LAYOUT_H   , N(strideh2,  3,  2) | N(heighth2,  1,  0)                                                            ) } // ProgGuide 4.8 p.103
    uint32_t ENC_BITMAP_SIZE       (FILTER filter, WRAP wrapx, WRAP wrapy, uint16_t width9, uint16_t height9)   { BODY(BITMAP_SIZE       , N(filter,   20, 20) | N(wrapx,    19, 19) | N(wrapy,   18, 18) | N(width9, 17,  9) | N(height9, 8, 0)) } // ProgGuide 4.9 p.103
    uint32_t ENC_BITMAP_SIZE_H     (uint16_t width2, uint16_t height2)                                          { BODY(BITMAP_SIZE_H     , N(width2,    3,  2) | N(height2,   1,  0)                                                            ) } // ProgGuide 4.10 p.105
    uint32_t ENC_BITMAP_SOURCE     (uint32_t addr22)                                                            { BODY(BITMAP_SOURCE     , N(addr22,   21,  0)                                                                                  ) } // ProgGuide 4.11 p.106
    uint32_t ENC_BITMAP_TRANSFORM_A(uint32_t a17)                                                               { BODY(BITMAP_TRANSFORM_A, N(a17,      16,  0)                                                                                  ) } // ProgGuide 4.12 p.108
    uint32_t ENC_BITMAP_TRANSFORM_B(uint32_t b17)                                                               { BODY(BITMAP_TRANSFORM_B, N(b17,      16,  0)                                                                                  ) } // ProgGuide 4.13 p.109
    uint32_t ENC_BITMAP_TRANSFORM_C(uint32_t c24)                                                               { BODY(BITMAP_TRANSFORM_C, N(c24,      23,  0)                                                                                  ) } // ProgGuide 4.14 p.110
    uint32_t ENC_BITMAP_TRANSFORM_D(uint32_t d17)                                                               { BODY(BITMAP_TRANSFORM_D, N(d17,      16,  0)                                                                                  ) } // ProgGuide 4.15 p.111
    uint32_t ENC_BITMAP_TRANSFORM_E(uint32_t e17)                                                               { BODY(BITMAP_TRANSFORM_E, N(e17,      16,  0)                                                                                  ) } // ProgGuide 4.16 p.112
    uint32_t ENC_BITMAP_TRANSFORM_F(uint32_t f24)                                                               { BODY(BITMAP_TRANSFORM_F, N(f24,      23,  0)                                                                                  ) } // ProgGuide 4.17 p.113
    uint32_t ENC_BLEND_FUNC        (BLEND src, BLEND dst)                                                       { BODY(BLEND_FUNC        , N(src,       5,  3) | N(dst,       2,  0)                                                            ) } // ProgGuide 4.18 p.114
    uint32_t ENC_CALL              (uint32_t dest22)                                                            { BODY(CALL              , N(dest22,   15,  0)                                                                                  ) } // ProgGuide 4.19 p.116
    uint32_t ENC_CELL              (uint8_t cell7)                                                              { BODY(CELL              , N(cell7,     6,  0)                                                                                  ) } // ProgGuide 4.20 p.117
    uint32_t ENC_CLEAR             (uint8_t color1, uint8_t stencil1, uint8_t tag1)                             { BODY(CLEAR             , N(color1,    2,  2) | N(stencil1,  1,  1) | N(tag1,     0,  0)                                       ) } // ProgGuide 4.21 p.118
    uint32_t ENC_CLEAR_COLOR_A     (uint8_t alpha8)                                                             { BODY(CLEAR_COLOR_A     , N(alpha8,    7,  0)                                                                                  ) } // ProgGuide 4.22 p.120
    uint32_t ENC_CLEAR_COLOR_RGB   (uint8_t red8, uint8_t green8, uint8_t blue8)                                { BODY(CLEAR_COLOR_RGB   , N(red8,     23, 16) | N(green8,   15,  8) | N(blue8,    7,  0)                                       ) } // ProgGuide 4.23 p.121
    uint32_t ENC_CLEAR_COLOR       (uint32_t rgb24)                                                             { BODY(CLEAR_COLOR_RGB   , N(rgb24,    23,  0)                                                                                  ) } // ProgGuide 4.23 p.121
    uint32_t ENC_CLEAR_STENCIL     (uint8_t stencil8)                                                           { BODY(CLEAR_STENCIL     , N(stencil8,  7,  0)                                                                                  ) } // ProgGuide 4.24 p.122
    uint32_t ENC_CLEAR_TAG         (uint8_t tag8)                                                               { BODY(CLEAR_TAG         , N(tag8,      7,  0)                                                                                  ) } // ProgGuide 4.25 p.123
    uint32_t ENC_COLOR_A           (uint8_t alpha8)                                                             { BODY(COLOR_A           , N(alpha8,    7,  0)                                                                                  ) } // ProgGuide 4.26 p.124
    uint32_t ENC_COLOR_MASK        (uint8_t red1, uint8_t green1, uint8_t blue1, uint8_t alpha1)                { BODY(COLOR_MASK        , N(red1,      3,  3) | N(green1,    2,  2) | N(blue1,    1,  1) | N(alpha1,  0,  0)                   ) } // ProgGuide 4.27 p.125
    uint32_t ENC_COLOR_RGB         (uint8_t red8, uint8_t green8, uint8_t blue8)                                { BODY(COLOR_RGB         , N(red8,     23, 16) | N(green8,   15,  8) | N(blue8,    7,  0)                                       ) } // ProgGuide 4.28 p.126
    uint32_t ENC_COLOR             (uint32_t rgb24)                                                             { BODY(COLOR_RGB         , N(rgb24,    23,  0)                                                                                  ) } // ProgGuide 4.28 p.126
    uint32_t ENC_DISPLAY           ()                                                                           { BODY(DISPLAY           , 0                                                                                                    ) } // ProgGuide 4.29 p.127
    uint32_t ENC_END               ()                                                                           { BODY(END               , 0                                                                                                    ) } // ProgGuide 4.30 p.128
    uint32_t ENC_JUMP              (uint16_t dest16)                                                            { BODY(JUMP              , N(dest16,   15,  0)                                                                                  ) } // ProgGuide 4.31 p.129
    uint32_t ENC_LINE_WIDTH        (uint16_t width12)                                                           { BODY(LINE_WIDTH        , N(width12,  11,  0)                                                                                  ) } // ProgGuide 4.32 p.130
    uint32_t ENC_MACRO             (uint8_t index1)                                                             { BODY(MACRO             , N(index1,    0,  0)                                                                                  ) } // ProgGuide 4.33 p.131
    uint32_t ENC_NOP               ()                                                                           { BODY(NOP               , 0                                                                                                    ) } // ProgGuide 4.34 p.131
    uint32_t ENC_PALETTE_SOURCE    (uint32_t addr22)                                                            { BODY(PALETTE_SOURCE    , N(addr22,   21,  0)                                                                                  ) } // ProgGuide 4.35 p.132
    uint32_t ENC_POINT_SIZE        (uint16_t size13)                                                            { BODY(POINT_SIZE        , N(size13,   12,  0)                                                                                  ) } // ProgGuide 4.36 p.133
    uint32_t ENC_RESTORE_CONTEXT   ()                                                                           { BODY(RESTORE_CONTEXT   , 0                                                                                                    ) } // ProgGuide 4.37 p.134
    uint32_t ENC_RETURN            ()                                                                           { BODY(RETURN            , 0                                                                                                    ) } // ProgGuide 4.38 p.135
    uint32_t ENC_SAVE_CONTEXT      ()                                                                           { BODY(SAVE_CONTEXT      , 0                                                                                                    ) } // ProgGuide 4.39 p.136
    uint32_t ENC_SCISSOR_SIZE      (uint16_t width12, uint16_t height12)                                        { BODY(SCISSOR_SIZE      , N(width12,  23, 12) | N(height12, 11,  0)                                                            ) } // ProgGuide 4.40 p.137
    uint32_t ENC_SCISSOR_XY        (uint16_t x11, uint16_t y11)                                                 { BODY(SCISSOR_XY        , N(x11,      21, 11) | N(y11,      10,  0)                                                            ) } // ProgGuide 4.41 p.138
    uint32_t ENC_STENCIL_FUNC      (FUNC func, uint8_t ref8, uint8_t mask8)                                     { BODY(STENCIL_FUNC      , N(func,     19, 16) | N(ref8,     15,  8) | N(mask8,    7,  0)                                       ) } // ProgGuide 4.42 p.139
    uint32_t ENC_STENCIL_MASK      (uint8_t mask8)                                                              { BODY(STENCIL_MASK      , N(mask8,     7,  0)                                                                                  ) } // ProgGuide 4.43 p.140
    uint32_t ENC_STENCIL_OP        (STENCIL sfail, STENCIL spass)                                               { BODY(STENCIL_OP        , N(sfail,     5,  3) | N(spass,     2,  0)                                                            ) } // ProgGuide 4.44 p.141
    uint32_t ENC_TAG               (uint8_t tag8)                                                               { BODY(TAG               , N(tag8,      7,  0)                                                                                  ) } // ProgGuide 4.45 p.143
    uint32_t ENC_TAG_MASK          (uint8_t mask1)                                                              { BODY(TAG_MASK          , N(mask1,     0,  0)                                                                                  ) } // ProgGuide 4.46 p.144
    uint32_t ENC_VERTEX2F          (uint16_t x15, uint16_t y15)                                                 { BODY(VERTEX2F          , N(x15,      29, 15) | N(y15,      14,  0)                                                            ) } // ProgGuide 4.47 p.145
    uint32_t ENC_VERTEX2II         (uint16_t x9, uint16_t y9, uint8_t handle5, uint8_t cell6)                   { BODY(VERTEX2II         , N(x9,       29, 21) | N(y9,       20, 12) | N(handle5, 11,  7) | N(cell6,  6,  0)                    ) } // ProgGuide 4.48 p.146
    uint32_t ENC_VERTEX_FORMAT     (uint8_t frac3)                                                              { BODY(VERTEX_FORMAT     , N(frac3,     2,  0)                                                                                  ) } // ProgGuide 4.49 p.147
    uint32_t ENC_VERTEX_TRANSLATE_X(uint32_t x17)                                                               { BODY(VERTEX_TRANSLATE_X, N(x17,      16,  0)                                                                                  ) } // ProgGuide 4.50 p.148
    uint32_t ENC_VERTEX_TRANSLATE_Y(uint32_t y17)                                                               { BODY(VERTEX_TRANSLATE_Y, N(y17,      16,  0)                                                                                  ) } // ProgGuide 4.51 p.149
    #undef N
    #undef DBG_BODY
    #undef BODY

    // Graphics co-processor commands
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
    // "Host commands", see paragraph 4.1.5 p.16 of FT81X datasheet
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
        dl(ENC_CLEAR_COLOR(0));
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
