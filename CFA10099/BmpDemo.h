#include "Steve.h"

class BmpDemo
{
  // Reference to the display
  Steve          &_eve;

  // Information about the bitmap
  uint32_t        _RAMGStartAddress;
  int16_t         _width;
  int16_t         _height;
  Steve::FORMAT   _format;

  // Scrolling parameters
  uint32_t        _framesperstep;
  int16_t         _xstep;
  int16_t         _ystep;

  // Scrolling state
  uint32_t        _framecounter;
  int16_t         _xpos;
  int16_t         _ypos;

public:
  BmpDemo(
    Steve &eve)
    : _eve(eve)
  {
    // Nothing
  }

public:
  //-----------------------------------------------------------------------
  // Initialize the demo
  //
  // This should be called from the setup() function
  uint32_t                              // Returns updated RAM_G address
  Init(
    uint32_t RAMGStartAddress,          // Start address in RAM_G
    const uint8_t *bitmap,              // Bitmap data to load
    uint32_t bitmapsize,                // Size of data in bytes
    int16_t width,                      // Width of bitmap
    int16_t height,                     // Height of bitmap
    Steve::FORMAT format = Steve::FORMAT_RGB565, // Bitmap format
    uint32_t framesperstep = 3,         // Number of frames to wait for step
    int16_t xstep = 1,                  // Horizontal step (<=width)
    int16_t ystep = 0)                  // Vertical step (<=height)
  {
    _RAMGStartAddress = RAMGStartAddress;
    _format = format;
    _width = width;
    _height = height;
    _framesperstep = framesperstep;
    _xstep = xstep;
    _ystep = ystep;

    // Start at top left
    _framecounter = 0;
    _xpos = 0;
    _ypos = 0;

    // Write the bitmap to the buffer
    // TODO: Allow loading from SD card, and let the chip decompress the data if necessary
    // TODO: Make sure there's enough space
    // Return the address past the stored bitmap
    return _eve.RegWriteBuffer(_RAMGStartAddress, bitmapsize, bitmap);
  }

public:
  //-----------------------------------------------------------------------
  // Add commands for the demo to the list
  //
  // This should be called from the loop() function
  void AddCommands()
  {
    // Set the drawing color to white
    _eve.cmd_COLOR_RGB(0xFF, 0xFF, 0xFF);

    // Solid color -- not transparent
    _eve.cmd_COLOR_A(255);

    // Pull the uncompressed image from RAM_G onto the screen
    _eve.cmd_BEGIN(_eve.BEGIN_BITMAPS);
    _eve.cmd_BITMAP_SOURCE(_RAMGStartAddress);
    _eve.cmd_SETBITMAP(_RAMGStartAddress, _format, _width, _height);

    // Tile the bitmap as many times as needed
    // _xpos is between 0 and _width  - 1 (inclusive)
    // _ypos is between 0 and _height - 1 (inclusive)
    for (int16_t x = _xpos - _width; x < _eve.Width(); x += _width)
    {
      for (int16_t y = _ypos - _height; y < _eve.Height(); y += _height)
      {
        _eve.cmd_VERTEX2F(x * 16, y * 16);
      }
    }

    _eve.cmd_END();
  }

public:
  //-------------------------------------------------------------------------
  // Update the state for the next frame
  void Cycle()
  {
    // Bump the frame counter. We only move the picture when the count
    // reaches the number of frames per step
    if (++_framecounter >= _framesperstep)
    {
      _framecounter = 0;

      // Move X direction
      _xpos += _xstep;
      if (_xpos >= _width)
      {
        _xpos -= _width;
      }
      else if (_xpos < 0)
      {
        _xpos += _width;
      }

      // Move Y direction
      _ypos += _ystep;
      if (_ypos >= _height)
      {
        _ypos -= _height;
      }
      else if (_ypos < 0)
      {
        _ypos += _height;
      }
    }
  }
};
