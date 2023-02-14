#include "Steve.h"

// Bitmap for VU meter
//
// The bitmap contains two copies of a horizontal LED VU meter: The top half
// is a version with all segments on and the bottom is a version with all
// segments off.
static const uint8_t vu[] = {
  // NOTE: Raw files generated with EVE Asset Builder which can be
  // downloaded from BridgeTek.com
  // The file MUST be in the src subdirectory and the name MUST end in .h.
  // Arduino made those rules, not me.
  #include "src/PPMDemo/Jac/jac_480x80_RGB565.rawh.h"
  //#include "src/PPMDemo/Philip/philip_480x80_RGB565.rawh.h"
  //#include "src/PPMDemo/Svilen/svilen_480x80_RGB565.rawh.h"
};

// Lookup table for displaying values on the meter.
//
// Each value in the table represents how many pixels of the "segments on"
// bitmap should be overlaid on top of the "segments off" bitmap.
static const uint16_t xlut[98] = {
  #include "src/PPMDemo/Jac/jac_xlut.h"
  //#include "src/PPMDemo/Philip/philip_xlut.h" // Identical to jac_xlut.h
  //#include "src/PPMDemo/Svilen/svilen_xlut.h"
};

class VUDemo
{
  // Reference to the display
  Steve            &_eve;

  // Information about the bitmap
  uint8_t           _handle;
  uint32_t          _RAMGStartAddress;
  int16_t           _width;
  int16_t           _height;
  Steve::FORMAT     _format;

  // State
  uint8_t           _xl;
  uint8_t           _xr;
  uint32_t          _t;
  uint32_t          _framesperstep;
  bool              _up = true;

public:
  VUDemo(
  Steve &eve)
  : _eve(eve)
  {
    _xl = 95;
    _xr = 0;
    _t = 0;
    _up = true;
  }

public:
  uint32_t Init(
    uint8_t handle,
    uint32_t RAMGStartAddress,
    const uint8_t *bitmap,
    uint32_t bitmapsize,
    int16_t width,
    int16_t height,
    Steve::FORMAT format = Steve::FORMAT_RGB565,
    uint32_t framesperstep = 3)
  {
    _handle = handle;
    _RAMGStartAddress = RAMGStartAddress;
    _width = width;
    _height = height;
    _format = format;
    _framesperstep = framesperstep;

    // Write the bitmap to the buffer
    // Return the address past the stored bitmap
    return _eve.RegWriteBuffer(_RAMGStartAddress, bitmapsize, bitmap);
  }

public:
  void AddCommands()
  {
    // Set the drawing color to white
    _eve.cmd_COLOR_RGB(0xFF, 0xFF, 0xFF);

    // Solid color -- not transparent
    _eve.cmd_COLOR_A(255);

    _eve.cmd_NUMBER(0, 0, 31, _eve.OPT_NONE, _xl);

    // Pull the uncompressed image from RAM_G onto the screen
    _eve.cmd_BITMAP_HANDLE(_handle);
    _eve.cmd_BEGIN(_eve.BEGIN_BITMAPS);
    _eve.cmd_SETBITMAP(_RAMGStartAddress, _format, _width, _height);
    _eve.cmd_VERTEX2II(0, _eve.Height() - _height, _handle, 1);

    // Left channel
    _eve.cmd_SCISSOR_SIZE(xlut[_xl], _height / 2);
    _eve.cmd_SCISSOR_XY(0, _eve.Height() - _height);
    _eve.cmd_VERTEX2II(0, _eve.Height() - _height, _handle, 0);

    // Right channel
    _eve.cmd_SCISSOR_SIZE(xlut[_xr], _height / 2);
    _eve.cmd_SCISSOR_XY(0, (_eve.Height() - _height) + (_height / 2));
    _eve.cmd_VERTEX2II(0, _eve.Height() - _height, _handle, 0);

    // Reset scissor area
    _eve.cmd_SCISSOR_XY(0, 0);
    _eve.cmd_SCISSOR_SIZE(2048, 2048);

    _eve.cmd_END();
  }

public:
  void Cycle()
  {
    if (++_t == _framesperstep)
    {
      _t = 0;

      if (_xl == 0 || _xl == 95)
      {
        _up = !_xl;
      }
      _xl += (_up ? 1 : -1);
      _xr = 95 - _xl;
    }
  }
};
