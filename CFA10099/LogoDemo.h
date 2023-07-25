#include "Steve.h"

class LogoDemo
{
  // Reference to the display
  Steve &_eve;

  //Remember where we put the logo image data in RAM_G
  uint32_t          _RAMGStartAddress;

  //Remember how big the Logo image is.
  uint32_t          _width;
  uint32_t          _height;

  // Keep track of the logo's orientation
  uint16_t          _rotation_degrees;

  //Used to pause the logo between spinning sesions
  uint16_t          _rotate_pause;

  //-------------------------------------------------------------------------
  //Test code to crash coprocessor ever other time it is called --
  //for testing Reset_EVE_Coprocessor()
#if (0!=DEBUG_COPROCESSOR_RESET)
  uint8_t           first = 0;
#endif // (0!=DEBUG_COPROCESSOR_RESET)

public:
  //-------------------------------------------------------------------------
  // Constructor
  LogoDemo(Steve eve)
  : _eve(eve)
  {
    // Nothing
  }


  //-------------------------------------------------------------------------
  // Initialize the demo
  //
  // This should be called from the setup() function
  uint32_t                              // Returns updated RAM_G address
  Init(
    uint32_t RAMGStartAddress)          // Start address in RAM_G
  {
    uint32_t result = RAMGStartAddress;

    // No rotation to start.
    _rotation_degrees = 0;
    // Don't spin the logo right away, delay ~8 seconds at 60 frames per second
    _rotate_pause = 8 * 60;

#if (0==LOGO_PNG_0_ARGB2_1)
    //Load and expand our 24-bit PNG (true color, lossless, with
    //transparency) into RAM_G
    //An indexed PNG is smaller, but the EVE does not handle
    //indexed color PNGs.
    FWol = EVE_Load_PNG_to_RAM_G(FWol,
      CFA480128Ex_039Tx_PNG_LOGO,
      LOGO_SIZE_PNG,
      RAM_G_Unused_Start,
      &_width,
      &_height);
#endif // 0==LOGO_PNG_0_ARGB2_1

#if (1==LOGO_PNG_0_ARGB2_1)
    //Load and INFLATE our 8-bit A2R2G2B2 (lossless) image into RAM_G

    //You have to know before hand how big the logo is. The INFLATE
    //is not aware of the content or format of the data.
    _width = LOGO_WIDTH_ARGB2;
    _height = LOGO_HEIGHT_ARGB2;


    //Test code to crash coprocessor ever other time it is called --
    //for testing Reset_EVE_Coprocessor()
#if (0 != DEBUG_COPROCESSOR_RESET)
    if (0 == first)
    {
      //good
      FWol = EVE_Inflate_to_RAM_G(FWol,
        CFA480128Ex_039Tx_ARGB2_LOGO,
        LOGO_SIZE_ARGB2,
        RAM_G_Unused_Start);
      DBG_GEEK("Good inflate\n");
      first = 1;
    }
    else
    {
      //bad
      FWol = EVE_Inflate_to_RAM_G(FWol,
        CFA480128Ex_039Tx_ARGB2_LOGO + 20,
        LOGO_SIZE_ARGB2,
        RAM_G_Unused_Start);
      DBG_GEEK("Bad inflate\n");
      first = 0;
    }
#else //(0!=DEBUG_COPROCESSOR_RESET)

    FWol = EVE_Inflate_to_RAM_G(FWol,
      CFA480128Ex_039Tx_ARGB2_LOGO,
      LOGO_SIZE_ARGB2,
      RAM_G_Unused_Start);

#endif //(0!=DEBUG_COPROCESSOR_RESET)

#endif // 1==LOGO_PNG_0_ARGB2_1
    //Pass our updated offset back to the caller
    return(FWol);
  }

  //----------------------------------------------------------------------------
  uint16_t Add_Logo_To_Display_List(uint16_t FWol)
  {
    //========== PUT LOGO ON SCREEN ==========
    // Set the drawing color to white
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(0xFF, 0xFF, 0xFF));
    //Solid color -- not transparent
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_A(255));
    //Point to the uncompressed logo in RAM_G
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_BEGIN(EVE_BEGIN_BITMAPS));
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_BITMAP_SOURCE(_RAMGStartAddress));

#if 0==LOGO_PNG_0_ARGB2_1
    //Transparent PNG comes in as ARGB4, 2 bytes per pixel
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_BITMAP_LAYOUT(EVE_FORMAT_ARGB4, _width * 2, _height));
#endif // 0==LOGO_PNG_0_ARGB2_1

#if 1==LOGO_PNG_0_ARGB2_1
    //ARGB2 comes in as EVE_FORMAT_ARGB2, 1 byte per pixel
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_BITMAP_LAYOUT(EVE_FORMAT_ARGB2, _width, _height));
#endif // 1==LOGO_PNG_0_ARGB2_1

    //In order to have a 240x240 logo rotate without clipping, we have
    //a 240*sqrt(2) x 240*sqrt(2) = 340 x 340 logo, with all the non
    //transparent content kept inside a 240 circle. Whatevs.
    FWol = EVE_Cmd_Dat_0(FWol,
      EVE_ENC_BITMAP_SIZE(EVE_FILTER_BILINEAR,
        EVE_WRAP_BORDER,
        EVE_WRAP_BORDER,
        _width,
        _height));
    //Rotate the bitmap
    FWol = EVE_Cmd_Dat_0(FWol,
      EVE_ENC_CMD_LOADIDENTITY);
    //Translate to the center
    FWol = EVE_Cmd_Dat_2(FWol,
      EVE_ENC_CMD_TRANSLATE,
      to_16_16_fp(_width, 0) / 2,
      to_16_16_fp(_height, 0) / 2);
    //The actual rotate command
    FWol = EVE_Cmd_Dat_1(FWol,
      EVE_ENC_CMD_ROTATE,
      to_16_16_fp(_rotation_degrees, 0) / 360);
    //Undo the translation, and move to the center of the screen.
    FWol = EVE_Cmd_Dat_2(FWol, EVE_ENC_CMD_TRANSLATE,
      (to_16_16_fp(-_width / 2 + 0 / 2, 0)),
      (to_16_16_fp(-_height / 2 + 0 / 2, 0)));
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_CMD_SETMATRIX);
    //Render the bitmap to the current frame
    FWol = EVE_Cmd_Dat_0(FWol,
      EVE_ENC_VERTEX2F((LCD_WIDTH - _width) * (16 / 2),
        (LCD_HEIGHT - _height) * (16 / 2)));
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_END());

    // ROTATE THE LOGO -- MAINLY BECAUSE WE CAN.
    if (0 == _rotation_degrees)
    {
      //upright, stay here for logo_rotate_pause frames
      if (0 != _rotate_pause)
      {
        _rotate_pause--;
      }
      else
      {
        // ~ 7 seconds at 60 frames per second
        _rotate_pause = 7 * 60;
        _rotation_degrees = 1;
      }
    }
    else
    {
      if (_rotation_degrees < 359)
        _rotation_degrees++;
      else
        _rotation_degrees = 0;
    }
    //Pass our updated offset back to the caller
    return(FWol);
  }
};
