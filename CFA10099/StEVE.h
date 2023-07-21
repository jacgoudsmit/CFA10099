/****************************************************************************
Steve.h
(C) 2023 Jac Goudsmit
MIT License.

The Steve class (Static Type-safe EVE) implements functionality to control
one or more EVE-based LCD displays, with or without (capacitive or resistive)
touch screen. EVE stands for Embedded Video Engine. It was first developed
by FTDI (well known for its USB-to-serial interface chips), who then
founded BridgeTek Pte Ltd. The first chip type numbers started with FT (FTDI)
but later on, the BT prefix for Bridgetek was used.

This library is different from other libraries because it deals with
variations between LCD panel parameters (such as width and height) in code,
not in preprocessor #defines. This makes it possible to use the same
binary code for multiple displays, even multiple dissimilar displays that
are controlled at the same time. This is the "S" ("Static") in "Steve".

Another difference is that even though this header file uses macros
extensively, it enforces or at least encourages developers to use the
appropriate types and values for operations. For example, the Begin function
only accepts one of the values that are valid with the BEGIN command,
because in this library the values are defined as an enum type, not as
preprocessor #defines. This is the "T" ("Type-safe") in "sTeve".

All the hardware control such as toggling the !CS and !PD chip, handling
interrupts, and sending and receiving data over the SPI/QSPI bus, is handled
by the SteveHAL class. The library was originally developed for the Arduino
platform, but should be easy to port to other platforms by writing a
subclass of SteveHAL that implements all the abstract virtual functions. Some
optimizations are possible; for example if there is a more efficient way to
send or receive multiple bytes or memory buffers on your platform, you can
override the necessary virtual functions in your SteveHAL derived class
to accomplish this.

All the parameters that are display-specific are stored in an instance of
the Steve::DisplayProfile struct. This should make it easy to write and
reuse software that can work on multiple displays, even if they have
different major parameters such as width and height.


References:

Data sheets: See https://brtchip.com/document/ic-datasheets
=============================================================================
Ref:      [DS2]
URL:      https://brtchip.com/wp-content/uploads/2021/09/DS_FT81x.pdf
Title     FT81x Embedded Video Engine Datasheet
Version:  1.4
Date:     2017-06-30
Chips:    FT810/FT811/FT812/FT813 "EVE2"
=============================================================================
Ref:      [DS3]
URL:      https://brtchip.com/wp-content/uploads/sites/3/2021/07/DS_BT81X.pdf
Title:    BT81X Advanced Embedded Video Engine
Version:  1.0
Date:     2018-08-15
Chips:    BT815/BT816 "EVE3"
=============================================================================
Ref:      [DS4]
URL:      https://brtchip.com/wp-content/uploads/2022/04/DS_BT817_8.pdf
Title:    BT817/8 Advanced Embedded Video Engine Datasheet
Version:  1.2
Date:     2022-04-04
Chips:    BT817/BT818 "EVE4"
=============================================================================


Programming Guides: See https://brtchip.com/document/programming-guides/
=============================================================================
Ref:      [PG2]
URL:      https://brtchip.com/wp-content/uploads/sites/3/2021/10/FT81X_Series_Programmer_Guide.pdf
Title:    FT81X_Series_Programmer_Guide
Version:  1.2
Date:     2018-10-02
Chips:    BT810/BT811/BT812/BT813 "EVE2"
=============================================================================
Ref:      [PG34]
URL:      https://brtchip.com/wp-content/uploads/2022/12/BRT_AN_033_BT81X-Series-Programming-Guide.pdf
Title:    BRT_AN_033_BT81X-Series-Programming-Guide
Version:  2.3
Date:     2022-11-24
Chips:    BT815/BT816/BT817/BT818 "EVE3/EVE4"
=============================================================================


Module overview:
(EVE1)
* FT800: 480x320,  18 bit RGB, 256K RAM, resistive  touch (NOT SUPPORTED)
* FT801: 480x320,  18 bit RGB, 256K RAM, capacitive touch (NOT SUPPORTED)
(EVE2)
* FT810: 800x600,  18 bit RGB, 1MB RAM,  resistive  touch
* FT811: 800x600,  18 bit RGB, 1MB RAM,  capacitive touch (TESTED)
* FT812: 800x600,  24 bit RGB, 1MB RAM,  resistive  touch
* FT813: 800x600,  24 bit RGB, 1MB RAM,  capacitive touch
(EVE3)
* BT815: 800x600,  24 bit RGB, 1MB RAM,  capacitive touch
* BT816: 800x600,  24 bit RGB, 1MB RAM,  resistive  touch
(EVE4)
* BT817: 1280x800, 24 bit RGB, 1MB RAM,  capacitive touch
* BT818: 1280x800, 24 bit RGB, 1MB RAM,  resistive  touch

NOTE: The FT800 and FT801 (EVE1, 480x320, 18 bit RGB, 256K RAM) work mostly
the same as the supported modules, but the memory map of the FT80X is
different. Because of this, the FT800 and FT801 aren't supported. It may be
possible to change the constants to make the library work for the FT80X chips
but this is currently not on the agenda.

****************************************************************************/


#pragma once


/////////////////////////////////////////////////////////////////////////////
// MACROS
/////////////////////////////////////////////////////////////////////////////


// Redefine these macros for debugging
#ifndef DBG_TRAFFIC
#define DBG_TRAFFIC(...)
#endif
#ifndef DBG_GEEK
#define DBG_GEEK(...)
#endif
#ifndef DBG_STAT
#define DBG_STAT(...)
#endif


/////////////////////////////////////////////////////////////////////////////
// HARDWARE ABSTRACTION LAYER
/////////////////////////////////////////////////////////////////////////////


//---------------------------------------------------------------------------
// Hardware Abstraction Layer for Steve
//
// This abstract class provides the communication with an EVE chip.
class SteveHAL
{
  friend class Steve;

protected:
  //-------------------------------------------------------------------------
  // Constructor
  //
  // The constructor is protected so it can only be called by subclasses.
  SteveHAL()
  {
    // Nothing
  }

protected:
  //-------------------------------------------------------------------------
  // Initialize the communication
  //
  // This is called by Steve to initialize the communication with the EVE
  // chip.
  //
  // According to some documentation, in slow mode (until the EVE clock is
  // running, the SPI clock should run no faster than 11 MHz. After the EVE
  // chip is initialized, the SPI bus can use up to 30 MHz.
  virtual void Init(
    bool slow = false) = 0;             // True=use slow speed for early init

protected:
  //-------------------------------------------------------------------------
  // Pause or resume communication
  //
  // This is called by Steve to pause or resume communication to the
  // EVE chip.
  virtual void Pause(
    bool pause) = 0;                    // True=pause, false=resume

protected:
  //-------------------------------------------------------------------------
  // Turn the power on or off
  //
  // This is called by Steve to reset the chip as part of the initialization
  // sequence.
  //
  // NOTE: The pin is marked !PD (Power Down Not) so the pin is set to LOW
  // for a 'false' parameter, HIGH for 'true'.
  virtual void Power(
    bool enable) = 0;                   // True=on (!PD high) false=off/reset

protected:
  //-------------------------------------------------------------------------
  // Select or de-select the chip
  //
  // This is called by Steve to select or de-select the chip.
  //
  // The SPI interface on the EVE chips is not just used to let the chip
  // listen or ignore the data on the SPI bus, but also resets a sequencer
  // inside the chip that makes it start listening to host commands.
  // Some host commands initiate transfers of multiple bytes, and !CS needs
  // to stay active during the entire transfer.
  //
  // The HAL class keeps track of whether the call to this function actually
  // changed the state of the !CS line or not, and the return value is
  // used by the Steve class to make sure that the chip is the correct state.
  virtual bool                          // Returns true if !CS line changed
  Select(
    bool enable) = 0;                   // True=select (!CS low) false=de-sel

protected:
  //-------------------------------------------------------------------------
  // Transfer data to and from the EVE chip
  virtual uint8_t                       // Returns received byte
  Transfer(
    uint8_t value) = 0;                 // Byte to send

protected:
  //-------------------------------------------------------------------------
  // Send an 8-bit value
  virtual void Send8(
    uint8_t value)                      // Value to send
  {
    Transfer(value);
  }

protected:
  //-------------------------------------------------------------------------
  // Send a 16-bit value in little-endian format
  //
  // The least significant byte is sent first.
  virtual void Send16(
    uint16_t value)                     // Value to send
  {
    Transfer((uint8_t)(value));
    Transfer((uint8_t)(value >> 8));
  }

protected:
  //-------------------------------------------------------------------------
  // Send a 32-bit value in little-endian format
  //
  // The least significant byte is sent first.
  virtual void Send32(
    uint32_t value)                     // Value to send
  {
    Transfer((uint8_t)(value));
    Transfer((uint8_t)(value >> 8));
    Transfer((uint8_t)(value >> 16));
    Transfer((uint8_t)(value >> 24));
  }

protected:
  //-------------------------------------------------------------------------
  // Receive an 8-bit value
  virtual uint8_t                       // Returns incoming value
  Receive8()
  {
    return Transfer(0);
  }

protected:
  //-------------------------------------------------------------------------
  // Receive a 16-bit value in little-endian format
  //
  // The least significant byte is received first.
  virtual uint16_t                      // Returns incoming value
  Receive16()
  {
    uint16_t  result;

    result =  (uint32_t)Transfer(0);
    result |= (uint32_t)Transfer(0) << 8;

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Receive a 32-bit value in little-endian format
  //
  // The least significant byte is received first.
  virtual uint32_t                      // Returns incoming value
  Receive32()
  {
    uint32_t  result;

    result =  (uint32_t)Transfer(0);
    result |= (uint32_t)Transfer(0) << 8;
    result |= (uint32_t)Transfer(0) << 16;
    result |= (uint32_t)Transfer(0) << 24;

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Receive a buffer
  virtual uint32_t                      // Returns number of bytes received
  ReceiveBuffer(
    uint8_t *buffer,                    // Buffer to receive to
    uint32_t len)                       // Number of bytes to receive
  {
    uint32_t result;
    uint8_t *t = buffer;

    for (result = 0; result < len; result++)
    {
      *t++ = Receive8();
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Send zero-bytes for alignment
  //
  // This takes a number of previously transmitted bytes and transmit the
  // required number of extra bytes to get the number to a multiple of 4.
  virtual uint32_t                      // Returns updated number bytes sent
  SendAlignmentBytes(
    uint32_t num)                       // Previous number of bytes sent
  {
    uint32_t result = num;

    while (result % 4)
    {
      Send8(0);
      result++;
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Send data from a RAM buffer to the chip
  //
  // The function sends a block of data of the given size.
  virtual uint32_t                      // Returns number of bytes sent
  SendBuffer(
    const uint8_t* data,                // Data buffer to send
    uint32_t len)                       // Buffer length
  {
    uint32_t result;

    const uint8_t* p = data;
    for (result = 0; result < len; result++)
    {
      Send8(*p++);
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Send a nul-terminated string
  //
  // The function reads a string from RAM, and transfers it to the EVE
  // It stops either when it finds the end of the source string, or when
  // it reaches the maximum length minus one. Then it sends a nul byte.
  //
  // The maximum length parameter includes the nul-terminator byte. If 0 is
  // used for the maximum length parameter, the value is interpreted as
  // "65536".
  //
  // If the pointer is NULL, an empty string is sent.
  virtual uint16_t                      // Returns number of bytes sent
  SendString(
    const char* message,                // Characters to send, '\0' is end
    uint16_t maxlen)                    // Max input length including \0
  {
    uint16_t result;
    const char* s = message;

    // Replace the pointer if it's NULL
    if (!s)
    {
      s = "";
    }

    // Send the non-nul characters. Note: if maxlen is 0, maxlen - 1
    // underflows to 65535.
    for (result = 0; result < maxlen - 1; result++)
    {
      char c = *s++;

      if (!c)
      {
        break;
      }

      Send8(c);
    }

    // Always send nul terminator byte
    Send8(0);
    result += 1;

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
  //
  // The function assumes that the message pointer points to a valid string
  // and functions such as strlen_F can be executed on it. This should be
  // safe for strings in ROM or Flash because you would normally initialize
  // them as a string literal. If the maximum length parameter is set to 0,
  // the actual length of the string is used. If the buffer size parameter
  // is set to 0, the length of the string is used.
  virtual uint16_t                    // Returns number of bytes sent
  SendStringF(
    const __FlashStringHelper* message, // Characters to send, '\0' is end
    uint16_t maxlen,                  // Max input length including \0
    uint16_t bufsize)                 // Buffer size to use
  {
    uint16_t result = 0;
    size_t len = strnlen_P((const char *)message, maxlen - 1);
    size_t actualbufsize = min((size_t)bufsize - 1, (size_t)len);
    uint8_t buf[actualbufsize];
    const char *s = (const char *)message;
    size_t remlen = len;

    while (remlen)
    {
      size_t copylen = min(remlen, actualbufsize);

      memcpy_P(buf, s, copylen);
      SendBuffer(buf, copylen);

      s += copylen;
      result += copylen;
      remlen -= copylen;
    }

    // Always send nul terminator byte
    Send8(0);
    result += 1;

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Wait for at least the requested time
  virtual void
  Delay(
    uint32_t ms) = 0;                   // Number of milliseconds to wait
};


/////////////////////////////////////////////////////////////////////////////
// HARDWARE ABSTRACTION LAYER SPECIFIC TO ARDUINO
/////////////////////////////////////////////////////////////////////////////


#ifdef ARDUINO

#include <SPI.h>

//---------------------------------------------------------------------------
// Hardware Abstraction Layer for Arduino
//
// This uses a single SPI port (not dual SPI or quad SPI). No optimizations
// were done for sending and receiving multiple successive bytes.
class SteveHAL_Arduino : public SteveHAL
{
private:
  //-------------------------------------------------------------------------
  // Constants and initialization parameters

  SPIClass         &_spi;               // SPI instance
  const SPISettings _spi_settings;      // SPI settings
  const int         _pin_cs;            // Chip Select Not Pin
  const int         _pin_pd;            // Power Down Not Pin
  const int         _pin_int;           // Interrupt Pin (-1=none)

private:
  //-------------------------------------------------------------------------
  // State data

  bool              _selected;          // True if chip currently selected

public:
  //-------------------------------------------------------------------------
  // Constructor
  SteveHAL_Arduino(
    SPIClass &spi,                      // SPI port
    uint32_t spi_clock,                 // SPI clock speed
    int pin_cs,                         // !CS pin
    int pin_pd,                         // !PD pin
    int pin_int)                        // Interrupt pin (-1=none)
    : SteveHAL()
    , _spi(spi)
    , _spi_settings(spi_clock, MSBFIRST, SPI_MODE0)
    , _pin_cs(pin_cs)
    , _pin_pd(pin_pd)
    , _pin_int(pin_int)
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
  }

protected:
  //-------------------------------------------------------------------------
  // Initialize the communication
  void Init(
    bool slow = false) override         // True=use slow speed for early init
  {
    // TODO: Implement slow/fast
    DBG_TRAFFIC("beginTransaction\n");
    _spi.beginTransaction(_spi_settings);
  }

protected:
  //-------------------------------------------------------------------------
  // Pause or resume communication
  void Pause(
    bool pause) override                // True=pause, false=resume
  {
    if (pause)
    {
      DBG_TRAFFIC("endTransaction\n");
      _spi.endTransaction();
    }
    else
    {
      Init();
    }
  }

protected:
  //-------------------------------------------------------------------------
  // Turn the power on or off
  void Power(
    bool enable) override               // True=on (!PD high) false=off/reset
  {
    // Set the pin HIGH to power up
    digitalWrite(_pin_pd, enable ? HIGH : LOW);
  }

protected:
  //-------------------------------------------------------------------------
  // Select or de-select the chip
  bool                                  // Returns true if !CS line changed
  Select(
    bool enable) override               // True=select (!CS low) false=de-sel
  {
    bool result = (enable != _selected);

    if (result)
    {
      _selected = enable;

      DBG_TRAFFIC("Select %u\n", !!enable);

      // Set the pin LOW to enable the chip
      digitalWrite(_pin_cs, enable ? LOW : HIGH);
    }

    return result;
  }

protected:
  //-------------------------------------------------------------------------
  // Transfer data to and from the EVE chip
  virtual uint8_t                       // Returns received byte
    Transfer(
      uint8_t value) override             // Byte to send
  {
    // TODO: more efficient 16/32 bit transfers; DMA...
    return _spi.transfer(value);
  }

protected:
  //-------------------------------------------------------------------------
  // Wait for at least the requested time
  virtual void
  Delay(
    uint32_t ms) override               // Number of milliseconds to wait
  {
    delay(ms);
  }
};

#endif // ARDUINO


/////////////////////////////////////////////////////////////////////////////
// STATIC TYPESAFE EVE CLASS
/////////////////////////////////////////////////////////////////////////////


class Steve
{
public:
    // Maximum string buffer size.
    //
    // The value was mostly chosen for memory considerations but can be
    // changed if necessary. However, other values were not tested.
    const static uint32_t MaxStringSize = 256;

public:
    //-----------------------------------------------------------------------
    // Convert RGB values to an RGB24 value
    uint32_t                            // Returns 24-bit RGB value
    static RGB(uint8_t r, uint8_t g, uint8_t b)
    {
        return (uint32_t)r << 16 | (uint32_t)g << 8 | (uint32_t)b;
    }

public:
    //-----------------------------------------------------------------------
    // Class to keep track of an index into the RAM area of the EVE chip
    //
    // The class works mostly like a uint16_t but whenever the value is
    // changed, a modulo is applied with the maximum value.
    template<uint16_t max_value> class Index
    {
    private:
        uint16_t    _index;             // Stored value, always < max_value

    private:
        //-------------------------------------------------------------------
        // Wrap the value around the maximum by performing a modulo operation
        uint16_t Wrap()
        {
            return _index %= max_value;
        }

    public:
        //-------------------------------------------------------------------
        // Constructor
        Index(
            uint16_t initial_value = 0) 
        : _index(initial_value)
        {
            Wrap();
        }

    public:
        //-------------------------------------------------------------------
        // Operator to add a signed integer in place
        //
        // The result is wrapped around the maximum value
        Index<max_value> &operator+=(int16_t value)
        {
            _index += value;
            Wrap();
            return *this;
        }

    public:
        //-------------------------------------------------------------------
        // Operator to subtract a signed integer in place
        //
        // The result is wrapped around the maximum value
        Index<max_value> &operator-=(int16_t value)
        {
            _index -= value;
            Wrap();
            return *this;
        }

    public:
        //-------------------------------------------------------------------
        // Operator to construct an index from the result of an addition
        Index<max_value> operator+(int16_t value) const
        {
            Index<max_value> result(_index);
            return result += value;
        }

    public:
        //-------------------------------------------------------------------
        // Operator to construct an index from the result of an subtraction
        Index<max_value> operator-(int16_t value) const
        {
            Index<max_value> result(_index);
            return result -= value;
        }

    public:
        //-------------------------------------------------------------------
        // Read the value
        uint16_t index() const
        {
            return _index;
        }
    };

public:
    //-----------------------------------------------------------------------
    // Memory map
    //
    // Note: Addresses are 22 bits.
    // [DS2 p42][DS3 p42][PG81X s2.1 p.14]
    const static uint32_t RAM_G                 = 0x000000;             // General Purpose RAM
    const static uint32_t ROM_FONT              = 0x1E0000;             // (EVE2) Font table and bitmap
    const static uint32_t ROM                   = 0x200000;             // (EVE3/EVE4) ROM codes, font table and bitmap
    const static uint32_t ROM_FONT_ADDR         = 0x2FFFFC;             // (EVE2) Font table pointer address
    const static uint32_t RAM_DL                = 0x300000;             // Display List RAM
    const static uint32_t RAM_REG               = 0x302000;             // Registers
    const static uint32_t RAM_CMD               = 0x308000;             // Co-processor command buffer
    const static uint32_t RAM_ERR_REPORT        = 0x309800;             // (EVE4) Co-processor fault report RAM
    const static uint32_t RAM_JTBOOT            = 0x30B000;             // (EVE4) Touch controller boot code
    const static uint32_t FLASH                 = 0x800000;             // (EVE3/EVE4) External Flash ROM (not directly accessible)


    // Memory area sizes (NOTE: these do NOT correspond to the differences
    // between the addresses above, because not all addresses are in use.
    // [DS2 p42][DS3 p42][PG81X s2.1 p.14]
    const static uint32_t RAM_G_SIZE            = 1024 * 1024;          // General Purpose RAM size
    const static uint32_t ROM_FONT_SIZE         = 1152 * 1024;          // (EVE2) ROM font size
    const static uint32_t ROM_SIZE              = 1024 * 1024;          // (EVE3/EVE4) ROM size
    const static uint32_t RAM_DL_SIZE           = 8 * 1024;             // Display List RAM size
    const static uint32_t RAM_REG_SIZE          = 4 * 1024;             // Registers size
    const static uint32_t RAM_CMD_SIZE          = 4 * 1024;             // Co-processor command buffer size
    const static uint32_t RAM_ERR_REPORT_SIZE   = 128;                  // (EVE4) Co-processor fault report size
    const static uint32_t RAM_JTBOOT_SIZE       = 2 * 1024;             // (EVE4) Touch controller boot code size
    const static uint32_t FLASH_SIZE            = 256 * 1024 * 1024;    // (EVE3/EVE4) Max external flash size

    // Pseudo address (index) for errors that occurred during a co-processor
    // command
    const static uint16_t READ_INDEX_ERROR      = 0x0FFF;

public:
    //-----------------------------------------------------------------------
    // Host commands and memory read/write operations
    //
    // Though these are presented in the documentation [DS* s4.1.3-4.1.5]
    // as distinct operation, host commands and memory read/write operations
    // are almost the same.
    //
    // Only the lower 24 bits are significant. The "ACTIVE" command is
    // partially identical to a "read memory location 0" operation but
    // doesn't require a dummy byte.
    enum HOSTCMD // 24 bits
    {
        // [DS2 p15][DS3 p15][DS4 p14]
        HOSTCMD_ACTIVE                  = 0x000000,     // Switch from Standby/Sleep/PWRDOWN to Active. Same as "read 0"
        HOSTCMD_READ                    = 0x000000,     // Read data. Add address, send dummy byte before reading.
        HOSTCMD_STANDBY                 = 0x410000,     // Put core in standby mode. Use ACTIVE to wake up again.
        HOSTCMD_SLEEP                   = 0x420000,     // Put core in sleep mode. Use ACTIVE to wake up again.
        HOSTCMD_PWRDOWN                 = 0x430000,     // Put core in power down mode. Use ACTIVE to wake up again.
        HOSTCMD_CLKEXT                  = 0x440000,     // Select PLL input from external xtal osc or external clock.
        HOSTCMD_CLKINT                  = 0x480000,     // Select PLL input from internal osc (default).
        HOSTCMD_PD_ROMS                 = 0x490000,     // (EVE2 only) Select power down to individual ROMs.
        HOSTCMD_PWRDOWN1                = 0x500000,     // Same as 0x430000.
        HOSTCMD_CLKSEL                  = 0x610000,     // Select system clock freq.
        HOSTCMD_CLKSEL1                 = 0x620000,     // Same as 0x610000.
        HOSTCMD_RST_PULSE               = 0x680000,     // Send reset pulse to core.
        HOSTCMD_PINDRIVE                = 0x700000,     // Set drive strength for various pins.
        HOSTCMD_PIN_PD_STATE            = 0x710000,     // Set pin state during power down.
        HOSTCMD_WRITE                   = 0x800000,     // Write data. Add address, send bytes.
    };

    //-----------------------------------------------------------------------
    // Parameter values for HOSTCMD_CLKSEL
    enum CLKSEL // 8 bits
    {
        // [DS2 p17][DS3 p16][DS4 p15]
        CLKSEL_DEFAULT                  = 0x00,         // Default for EVE1 compatibility (60 MHz)
        CLKSEL_X2                       = 0x02,         // 2x multiplier (24 MHz)
        CLKSEL_X3                       = 0x03,         // 3x multiplier (36 MHz)
        CLKSEL_X4                       = 0x44,         // 4x multiplier and high PLL range (48 MHz)
        CLKSEL_X5                       = 0x45,         // 5x multiplier and high PLL range (60 MHz)
        CLKSEL_X6                       = 0x46,         // (EVE3/EVE4) 6x multiplier and high PLL range (72 MHz)
        CLKSEL_X7                       = 0x47,         // (UNDOCUMENTED) 7x multiplier and high PLL range (84 MHz)
    };

    //-----------------------------------------------------------------------
    // Parameter values to set the pin driving strength (HOSTCMD_PINDRIVE)
    enum PINDRIVE_STRENGTH // 2 bits
    {
        // [DS2 p18][DS3 p17][DS4 p16]
        PINDRIVE_STRENGTH_LOW           = 0x00,         // 5 mA  (EVE3/EVE4: 1.2 mA for some pins)
        PINDRIVE_STRENGTH_MEDIUM        = 0x01,         // 10 mA (EVE3/EVE4: 2.4 mA for some pins)
        PINDRIVE_STRENGTH_HIGH          = 0x02,         // 15 mA (EVE3/EVE4: 3.6 mA for some pins)
        PINDRIVE_STRENGTH_MAXIMUM       = 0x03,         // 20 mA (EVE3/EVE4: 4.8 mA for some pins)
    };

    //-----------------------------------------------------------------------
    // Parameter values for pins to apply drive strength or power down state
    // (HOSTCMD_PINDRIVE and HOSTCMD_PD_STATE)
    //
    // These are flags; they may be combined.
    enum PINS // 6 bits
    {
        // [DS2 p18][DS3 p16][DS4 p15]
        PINS_GPIO0                      = 0x00,
        PINS_GPIO1                      = 0x01,
        PINS_GPIO2                      = 0x02,
        PINS_GPIO3                      = 0x03,
        PINS_DISP                       = 0x08,
        PINS_DE                         = 0x09,
        PINS_VSYNC_HSYNC                = 0x0A,
        PINS_PCLK                       = 0x0B,
        PINS_BACKLIGHT                  = 0x0C,
        PINS_RGB                        = 0x0D,
        PINS_AUDIO_L                    = 0x0E,
        PINS_INT_N                      = 0x0F,
        PINS_CTP_RST_N                  = 0x10,
        PINS_CTP_SCL                    = 0x11,
        PINS_CTP_SDA                    = 0x12,
        PINS_SPI                        = 0x13,

        // (EVE3/EVE4 only)
        PINS_SPIM_SCLK                  = 0x14,
        PINS_SPIM_SS_N                  = 0x15,
        PINS_SPIM_MISO                  = 0x16,
        PINS_SPIM_MOSI                  = 0x17,
        PINS_SPIM_IO2                   = 0x18,
        PINS_SPIM_IO3                   = 0x19,
    };

    //-----------------------------------------------------------------------
    // Parameter values for setting power-down pin state (HOSTCMD_PD_STATE)
    enum PD_STATE
    {
        // [DS2 p18][DS3 p18][DS4 p17]
        PD_STATE_FLOAT                  = 0x0,          // Float the pin
        PD_STATE_PULL_DOWN              = 0x1,          // Pull the pin down
        PD_STATE_PULL_UP                = 0x2,          // Pull the pin up
    };

    //-----------------------------------------------------------------------
    // Register addresses
    enum REG // 22 bits
    {
        // [DS2 p41][DS3 p42][DS3 p41]
        //
        // These are mostly in numerical order except where register groups
        // were split between different address areas. There is some overlap,
        // especially between the resistive and capacitive touch engines.

        // General configuration / status
        REG_ID                          = 0x302000,     // [PG2 p87][PG34 p49]  (ro8)   Identification; always 0x7C
        REG_FRAMES                      = 0x302004,     // [PG2 p87][PG34 p49]  (ro32)  Frame counter
        REG_CLOCK                       = 0x302008,     // [PG2 p87][PG34 p49]  (ro32)  Clock cycle counter
        REG_FREQUENCY                   = 0x30200C,     // [PG2 p86][PG34 p49]  (rw28)  Clock frequency as known by the chip
        REG_RENDERMODE                  = 0x302010,     //                      (rw1)   Single line rendering enable (not in [PG*])
        REG_SNAPY                       = 0x302014,     //                      (rw11)  Single line scanline selection (not in [PG*])
        REG_SNAPSHOT                    = 0x302018,     //                      (rw1)   Single line render mode trigger (not in [PG*])
        REG_SNAPFORMAT                  = 0x30201C,     //                      (rw6)   Pixel format for scanline readout (not in [PG*])
        REG_CPURESET                    = 0x302020,     // [PG2 p81][PG34 p46]  (rw3)   Audio/Touch/Graphics reset control
        REG_TAP_CRC                     = 0x302024,     //                      (ro32)  Live video tap CRC (not in [PG*])
        REG_TAP_MASK                    = 0x302028,     //                      (rw32)  Live video tap mask (not in [PG*])

        // LCD panel configuration
        REG_HCYCLE                      = 0x30202C,     // [PG2 p34][PG34 p32]  (rw12)  Horizontal total cycle count
        REG_HOFFSET                     = 0x302030,     // [PG2 p34][PG34 p31]  (rw12)  Horizontal display start offset
        REG_HSIZE                       = 0x302034,     // [PG2 p33][PG34 p31]  (rw12)  Horizontal display size
        REG_HSYNC0                      = 0x302038,     // [PG2 p32][PG34 p31]  (rw12)  Horizontal sync fall offset
        REG_HSYNC1                      = 0x30203C,     // [PG2 p32][PG34 p31]  (rw12)  Horizontal sync rise offset
        REG_VCYCLE                      = 0x302040,     // [PG2 p31][PG34 p31]  (rw12)  Vertical total cycle count
        REG_VOFFSET                     = 0x302044,     // [PG2 p31][PG34 p31]  (rw12)  Vertical display start offset
        REG_VSIZE                       = 0x302048,     // [PG2 p31][PG34 p31]  (rw12)  Vertical display line count
        REG_VSYNC0                      = 0x30204C,     // [PG2 p30][PG34 p30]  (rw10)  Vertical sync fall offset
        REG_VSYNC1                      = 0x302050,     // [PG2 p30][PG34 p30]  (rw10)  Vertical sync rise offset
        REG_DLSWAP                      = 0x302054,     // [PG2 p35][PG34 p30]  (rw2)   Display List swap control
        REG_ROTATE                      = 0x302058,     // [PG2 p29][PG34 p30]  (rw3)   Screen rotation
        REG_OUTBITS                     = 0x30205C,     // [PG2 p29][PG34 p29]  (rw9)   Output bits resolution
        REG_DITHER                      = 0x302060,     // [PG2 p28][PG34 p29]  (rw1)   Output dither enable
        REG_SWIZZLE                     = 0x302064,     // [PG2 p28][PG34 p29]  (rw4)   Output RGB bit order control
        REG_CSPREAD                     = 0x302068,     // [PG2 p27][PG34 p29]  (rw1)   Output clock spreading enable
        REG_PCLK_POL                    = 0x30206C,     // [PG2 p27][PG34 p29]  (rw1)   Pixel clock polarity
        REG_PCLK                        = 0x302070,     // [PG2 p26][PG34 p29]  (rw8)   Pixel clock divider

        // Touch control
        REG_TAG_X                       = 0x302074,     // [PG2 p36][PG34 p28]  (rw11)  Tag query X coordinate
        REG_TAG_Y                       = 0x302078,     // [PG2 p36][PG34 p28]  (rw11)  Tag query Y coordinate
        REG_TAG                         = 0x30207C,     // [PG2 p35][PG34 p28]  (ro8)   Tag query result

        // Audio
        REG_VOL_PB                      = 0x302080,     // [PG2 p38][PG34 p32]  (rw8)   Volume for playback
        REG_VOL_SOUND                   = 0x302084,     // [PG2 p38][PG34 p32]  (rw8)   Volume for synthesizer sound
        REG_SOUND                       = 0x302088,     // [PG2 p37][PG34 p32]  (rw16)  Sound effect select
        REG_PLAY                        = 0x30208C,     // [PG2 p37][PG34 p32]  (rw1)   Start effect playback

        // GPIO
        REG_GPIO_DIR                    = 0x302090,     // [PG2 p84][PG34 p47]  (rw8)   Legacy GPIO direction
        REG_GPIO                        = 0x302094,     // [PG2 p84][PG34 p48]  (rw8)   Legacy GPIO read/write
        REG_GPIOX_DIR                   = 0x302098,     // [PG2 p85][PG34 p48]  (rw16)  Extended GPIO direction
        REG_GPIOX                       = 0x30209C,     // [PG2 p86][PG34 p48]  (rw16)  Extended GPIO read/write

        //// 0x3020A0-0x3020A4 Reserved

        // Interrupt control
        REG_INT_FLAGS                   = 0x3020A8,     // [PG2 p83][PG34 p47]  (ro8)   Interrupt flags, clear by read
        REG_INT_EN                      = 0x3020AC,     // [PG2 p83][PG34 p47]  (rw1)   Global interrupt enable
        REG_INT_MASK                    = 0x3020B0,     // [PG2 p82][PG34 p47]  (rw8)   Interrupt mask

        // Audio playback
        REG_PLAYBACK_START              = 0x3020B4,     // [PG2 p42][PG34 p34]  (rw20)  Audio playback RAM start address
        REG_PLAYBACK_LENGTH             = 0x3020B8,     // [PG2 p42][PG34 p34]  (rw20)  Audio playback sample length
        REG_PLAYBACK_READPTR            = 0x3020BC,     // [PG2 p41][PG34 p33]  (ro20)  Audio playback current read ptr
        REG_PLAYBACK_FREQ               = 0x3020C0,     // [PG2 p41][PG34 p33]  (rw16)  Audio playback sample frequency
        REG_PLAYBACK_FORMAT             = 0x3020C4,     // [PG2 p40][PG34 p33]  (rw2)   Audio playback format
        REG_PLAYBACK_LOOP               = 0x3020C8,     // [PG2 p39][PG34 p33]  (rw1)   Audio playback loop enable
        REG_PLAYBACK_PLAY               = 0x3020CC,     // [PG2 p39][PG34 p33]  (rw1)   Start audio playback
        REG_PLAYBACK_PAUSE              = 0x3025EC,     //          [PG34 p34]  (rw1)   Audio playback pause (EVE3/EVE4)

        // Backlight control
        REG_PWM_HZ                      = 0x3020D0,     // [PG2 p82][PG34 p47]  (rw14)  Backlight PWM frequency
        REG_PWM_DUTY                    = 0x3020D4,     // [PG2 p81][PG34 p47]  (rw8)   Backlight PWM duty cycle

        // Display list macro commands
        REG_MACRO_0                     = 0x3020D8,     //          [PG34 p46]  (rw32)  Display list macro command 0 (not in [PG2])
        REG_MACRO_1                     = 0x3020DC,     //          [PG34 p46]  (rw32)  Display list macro command 1 (not in [PG2])

        //// 0x3020E0-0x3020F4 Reserved

        // Co-processor registers
        REG_CMD_READ                    = 0x3020F8,     // [PG2 p76][PG34 p45]  (rw12)  Command buffer read pointer
        REG_CMD_WRITE                   = 0x3020FC,     // [PG2 p75][PG34 p45]  (ro12)  Command buffer write pointer
        REG_CMD_DL                      = 0x302100,     // [PG2 p75][PG34 p45]  (rw13)  Command display list offset
        REG_CMDB_SPACE                  = 0x302574,     // [PG2 p76][PG34 p46]  (rw12)  Command DL (bulk) space available
        REG_CMDB_WRITE                  = 0x302578,     // [PG2 p77][PG34 p46]  (wo32)  Command DL (bulk) write

        // Resistive touch engine (FT810/FT812/BT816/BT818)
        REG_TOUCH_MODE                  = 0x302104,     // [PG2 p58][PG34 p40]  (rw2)   Touch screen sample mode
        REG_TOUCH_ADC_MODE              = 0x302108,     // [PG2 p58][PG34 p39]  (rw1)   Touch screen ADC mode
        REG_TOUCH_CHARGE                = 0x30210C,     // [PG2 p57][PG34 p39]  (rw16)  Touch charge time *6 clocks
        REG_TOUCH_SETTLE                = 0x302110,     // [PG2 p57][PG34 p39]  (rw4)   Touch settle time *6 clocks
        REG_TOUCH_OVERSAMPLE            = 0x302114,     // [PG2 p56][PG34 p39]  (rw4)   Touch oversample factor
        REG_TOUCH_RZTHRESH              = 0x302118,     // [PG2 p56][PG34 p39]  (rw16)  Touch resistance threshold
        REG_TOUCH_RAW_XY                = 0x30211C,     // [PG2 p55][PG34 p38]  (ro32)  (compatibility) Touch screen raw data
        REG_TOUCH_RZ                    = 0x302120,     // [PG2 p55][PG34 p38]  (ro16)  (compatibility) Touch screen resistance
        REG_TOUCH_SCREEN_XY             = 0x302124,     // [PG2 p53][PG34 p37]  (ro32)  (compatibility) Touch screen coordinates
        REG_TOUCH_TAG_XY                = 0x302128,     // [PG2 p52][PG34 p37]  (ro32)  Touch Tag 0 lookup
        REG_TOUCH_TAG                   = 0x30212C,     // [PG2 p51][PG34 p37]  (ro8)   Touch Tag 0 result
        REG_TOUCH_TRANSFORM_A           = 0x302150,     // [PG2 p49][PG34 p36]  (rw32)  Touch screen transform coefficient A
        REG_TOUCH_TRANSFORM_B           = 0x302154,     // [PG2 p48][PG34 p36]  (rw32)  Touch screen transform coefficient B
        REG_TOUCH_TRANSFORM_C           = 0x302158,     // [PG2 p47][PG34 p36]  (rw32)  Touch screen transform coefficient C
        REG_TOUCH_TRANSFORM_D           = 0x30215C,     // [PG2 p46][PG34 p36]  (rw32)  Touch screen transform coefficient D
        REG_TOUCH_TRANSFORM_E           = 0x302160,     // [PG2 p45][PG34 p36]  (rw32)  Touch screen transform coefficient E
        REG_TOUCH_TRANSFORM_F           = 0x302164,     // [PG2 p44][PG34 p35]  (rw32)  Touch screen transform coefficient F
        REG_TOUCH_CONFIG                = 0x302168,     // [PG2 p43][PG34 p35]  (rw16)  Touch configuration
        REG_TOUCH_DIRECT_XY             = 0x30218C,     // [PG2 p54][PG34 p38]  (ro32)  (compatibility) Touch screen direct conversions
        REG_TOUCH_DIRECT_Z1Z2           = 0x302190,     // [PG2 p54][PG34 p38]  (ro32)  (compatibility) Touch screen direct conversions

        // Capacitive touch engine (FT811/FT813/BT815/BT817)
        REG_CTOUCH_MODE                 = 0x302104,     // [PG2 p54][PG34 p41]  (rw2)   Touch screen sample mode
        REG_CTOUCH_EXTEND               = 0x302108,     // [PG2 p61][PG34 p41]  (rw1)   Touch screen mode (extended or compatibility)
        REG_CTOUCH_EXTENDED             = 0x302108,     // [PG2 p61][PG34 p41]  (rw1)   Touch screen mode (extended or compatibility) (Alias)
        REG_CTOUCH_RAW_XY               = 0x30211C,     // [PG2 p64][PG34 p42]  (ro32)  (compatibility) Touch screen raw data
        REG_CTOUCH_TOUCH1_XY            = 0x30211C,     // [PG2 p62][PG34 p41]  (ro32)  (extended)      Screen data for touch 1
        REG_CTOUCH_TOUCH4_Y             = 0x302120,     // [PG2 p64][PG34 p42]  (ro16)  (extended)      Screen Y data for touch 4
        REG_CTOUCH_TOUCH_XY             = 0x302124,     // [PG2 p61][PG34 p41]  (ro32)  (compatibility) Screen data for single touch
        REG_CTOUCH_TOUCH0_XY            = 0x302124,     // [PG2 p61][PG34 p41]  (ro32)  (extended)      Screen data for touch 0
        REG_CTOUCH_TAG_XY               = 0x302128,     // [PG2 p70][PG34 p44]  (ro32)  Touch Tag 0 lookup
        REG_CTOUCH_TAG                  = 0x30212C,     // [PG2 p65][PG34 p42]  (ro8)   Touch Tag 0 result
        REG_CTOUCH_TAG1_XY              = 0x302130,     // [PG2 p71][PG34 p44]  (ro32)  Touch Tag 1 lookup
        REG_CTOUCH_TAG1                 = 0x302134,     // [PG2 p66][PG34 p43]  (ro8)   Touch Tag 1 result
        REG_CTOUCH_TAG2_XY              = 0x302138,     // [PG2 p72][PG34 p44]  (ro32)  Touch Tag 2 lookup
        REG_CTOUCH_TAG2                 = 0x30213C,     // [PG2 p67][PG34 p43]  (ro8)   Touch Tag 2 result
        REG_CTOUCH_TAG3_XY              = 0x302140,     // [PG2 p73][PG34 p44]  (ro32)  Touch Tag 3 lookup
        REG_CTOUCH_TAG3                 = 0x302144,     // [PG2 p68][PG34 p43]  (ro8)   Touch Tag 3 result
        REG_CTOUCH_TAG4_XY              = 0x302148,     // [PG2 p74][PG34 p44]  (ro32)  Touch Tag 4 lookup
        REG_CTOUCH_TAG4                 = 0x30214C,     // [PG2 p69][PG34 p43]  (ro8)   Touch Tag 4 result
        REG_CTOUCH_TRANSFORM_A          = 0x302150,     // [PG2 p49][PG34 p36]  (rw32)  Touch screen transform coefficient A (Alias)
        REG_CTOUCH_TRANSFORM_B          = 0x302154,     // [PG2 p48][PG34 p36]  (rw32)  Touch screen transform coefficient B (Alias)
        REG_CTOUCH_TRANSFORM_C          = 0x302158,     // [PG2 p47][PG34 p36]  (rw32)  Touch screen transform coefficient C (Alias)
        REG_CTOUCH_TRANSFORM_D          = 0x30215C,     // [PG2 p46][PG34 p36]  (rw32)  Touch screen transform coefficient D (Alias)
        REG_CTOUCH_TRANSFORM_E          = 0x302160,     // [PG2 p45][PG34 p36]  (rw32)  Touch screen transform coefficient E (Alias)
        REG_CTOUCH_TRANSFORM_F          = 0x302164,     // [PG2 p44][PG34 p35]  (rw32)  Touch screen transform coefficient F (Alias)
        REG_CTOUCH_CONFIG               = 0x302168,     // [PG2 p43][PG34 p35]  (rw16)  Touch configuration (Alias)
        REG_CTOUCH_TOUCH4_X             = 0x30216C,     // [PG2 p63][PG34 p42]  (ro16)  (extended)      Screen X data for touch 4
        REG_CTOUCH_TOUCH2_XY            = 0x30218C,     // [PG2 p62][PG34 p41]  (ro32)  (extended)      Screen data for touch 2
        REG_CTOUCH_TOUCH3_XY            = 0x302190,     // [PG2 p63][PG34 p42]  (ro32)  (extended)      Screen data for touch 3

        // Touch host mode (not documented in [PG34])
        REG_EHOST_TOUCH_X               = 0x30210C,     //                      (rw16)  (touch host)    Touch X value updated by host (EVE3/EVE4))
        REG_EHOST_TOUCH_ID              = 0x302114,     //                      (rw4)   (touch host)    Touch ID 0-4 (EVE3/EVE4)
        REG_EHOST_TOUCH_ACK             = 0x302170,     //                      (rw4)   (touch host)    Acknowledgment (EVE3/EVE4)

        // Internal control
        REG_BIST_EN                     = 0x302174,     //                      (rw1)   BIST memory mapping enable (not in [PG*])
        REG_TRIM                        = 0x302180,     // [PG2 p88]            (rw8)   Internal relaxation clock trimming (not on EVE4?)
        REG_ANA_COMP                    = 0x302184,     //                      (rw8)   Analog control register (not documented in [PG*])
        REG_SPI_WIDTH                   = 0x302188,     // [PG2 p88][PG34 p50]  (rw3)   QSPI bus width setting

        //// 0x30902194 - 0x302560 Reserved

        // Date stamp
        REG_DATESTAMP                   = 0x302564,     //                      (ro128) 16 bytes of date stamp (not in [PG*])

        // EVE3/EVE4 features
        REG_ADAPTIVE_FRAMERATE          = 0x30257C,     //          [PG34 p50]  (rw1)   Reduce frame rate during complex drawing (EVE3/EVE4)

        // Flash registers
        REG_FLASH_STATUS                = 0x3025F0,     //          [PG34 p34]  (rw2)   Flash status (EVE3/EVE4)
        REG_FLASH_STATE                 = 0x3025F0,     //          [PG34 p34]  (rw2)   Flash status (EVE3/EVE4) (Alias)
        REG_FLASH_SIZE                  = 0x309024,     //          [PG34 p34]  (ro32)  Detected flash capacity in MB (EVE3/EVE4) (not in [DS3])

        // EVE4 features
        REG_UNDERRUN                    = 0x30260C,     //          [PG34 p50]  (ro32)  Line underrun counter (EVE4)
        REG_AH_CYCLE_MAX                = 0x302610,     //          [PG34 p50]  (rw12)  Adaptive hsync: max total PCLK cycles (EVE4)
        REG_PCLK_FREQ                   = 0x302614,     //          [PG34 p51]  (rw16)  Fractional PCLK (EVE4)
        REG_PCLK_2X                     = 0x302618,     //          [PG34 p51]  (rw1)   2 pixels per PCLK cycle (EVE4)
        REG_ANIM_ACTIVE                 = 0x30902C,     //          [PG34 p53]  (ro32)  Bitmask of currently playing animations (EVE4)
        REG_PLAY_CONTROL                = 0x30914E,     //          [PG34 p53]  (rw8)   Video playback control (EVE4)

        // Special Registers (documented in Programmer's Guides but not in
        // any data sheets except [DS4]).
        REG_TRACKER                     = 0x309000,     // [PG2 p77][PG34 p51]  (rw32)  Tracker register 0
        REG_TRACKER_1                   = 0x309004,     // [PG2 p78][PG34 p51]  (rw32)  Tracker register 1
        REG_TRACKER_2                   = 0x309008,     // [PG2 p78][PG34 p52]  (rw32)  Tracker register 2
        REG_TRACKER_3                   = 0x30900C,     // [PG2 p79][PG34 p52]  (rw32)  Tracker register 3
        REG_TRACKER_4                   = 0x309010,     // [PG2 p79][PG34 p52]  (rw32)  Tracker register 4
        REG_MEDIAFIFO_READ              = 0x309014,     // [PG2 p80][PG34 p52]  (ro32)  Media FIFO read offset
        REG_MEDIAFIFO_WRITE             = 0x309018,     // [PG2 p80][PG34 p52]  (rw32)  Media FIFO write offset

        // Undocumented constants found in other code
        //REG_BUSYBITS                    = 0x3020E8,     // Undocumented
        //REG_ROMSUB_SEL                  = 0x3020F0,     // Undocumented
        //REG_ANALOG                      = 0x30216C,     // Undocumented
        //REG_PATCHED_TOUCH_FAULT         = 0x30216C,     // Undocumented
        //REG_PATCHED_ANALOG              = 0x302170,     // Undocumented
        //REG_TOUCH_FAULT                 = 0x302170,     // Undocumented
        //REG_CRC                         = 0x302178,     // Undocumented
        //REG_SPI_EARLY_TX                = 0x30217C,     // Undocumented

        REG_COPRO_PATCH_PTR             = 0x307162,     //          [PG34 p53]  (ro16)  Co-processor patch pointer

        REG_CHIP_ID                     = 0x0C0000,     // [PG2 p11][PG34 p16]  (rw32)  Chip identifier in RAM_G [DS2 p46][DS3 p47][DS4 p45]
    };

    // Display list commands (ProgGuide ch.4)
    enum ENC_CMD
    {
        // Display List commands
        ENC_CMD_DISPLAY                 = 0x00000000,   // [PG2 p127][PG34 p80] End the display list
        ENC_CMD_BITMAP_SOURCE           = 0x01000000,   // [PG2 p106][PG34 p65] Specify the address of bitmap data
        ENC_CMD_CLEAR_COLOR_RGB         = 0x02000000,   // [PG2 p121][PG34 p76] Specify clear values for RGB
        ENC_CMD_CLEAR_COLOR             = 0x02000000,   // [PG2 p121][PG34 p76] Specify clear values for RGB (Alias for use with 24 bit RGB value)
        ENC_CMD_TAG                     = 0x03000000,   // [PG2 p143][PG34 p90] Attach tag value for following graphics objects drawn on screen
        ENC_CMD_COLOR_RGB               = 0x04000000,   // [PG2 p126][PG34 p79] Set the current color RGB
        ENC_CMD_COLOR                   = 0x04000000,   // [PG2 p126][PG34 p79] Set the current color RGB (Alias for use with 24-bit RGB value)
        ENC_CMD_BITMAP_HANDLE           = 0x05000000,   // [PG2 p96] [PG34 p58] Specify the bitmap handle
        ENC_CMD_CELL                    = 0x06000000,   // [PG2 p117][PG34 p74] Specify the bitmap cell number for the VERTEX2 command
        ENC_CMD_BITMAP_LAYOUT           = 0x07000000,   // [PG2 p97] [PG34 p59] Specify source bitmap memory format and layout
        ENC_CMD_BITMAP_SIZE             = 0x08000000,   // [PG2 p103][PG34 p63] Specify the screen drawing of bitmaps
        ENC_CMD_ALPHA_FUNC              = 0x09000000,   // [PG2 p92] [PG34 p56] Specify the Alpha test function
        ENC_CMD_STENCIL_FUNC            = 0x0A000000,   // [PG2 p139][PG34 p88] Set function and reference value for stencil testing
        ENC_CMD_BLEND_FUNC              = 0x0B000000,   // [PG2 p114][PG34 p72] Specify pixel arithmetic
        ENC_CMD_STENCIL_OP              = 0x0C000000,   // [PG2 p141][PG34 p89] Set stencil test actions
        ENC_CMD_POINT_SIZE              = 0x0D000000,   // [PG2 p133][PG34 p83] Specify the radius of points
        ENC_CMD_LINE_WIDTH              = 0x0E000000,   // [PG2 p130][PG34 p81] Specify the width of lines to be drawn
        ENC_CMD_CLEAR_COLOR_A           = 0x0F000000,   // [PG2 p120][PG34 p75] Specify clear value for the alpha channel
        ENC_CMD_COLOR_A                 = 0x10000000,   // [PG2 p124][PG34 p77] Set the current color alpha
        ENC_CMD_CLEAR_STENCIL           = 0x11000000,   // [PG2 p122][PG34 p77] Specify clear value for the stencil buffer
        ENC_CMD_CLEAR_TAG               = 0x12000000,   // [PG2 p123][PG34 p77] Specify clear value for the tag buffer
        ENC_CMD_STENCIL_MASK            = 0x13000000,   // [PG2 p140][PG34 p88] Control the writing of individual bits in stencil planes
        ENC_CMD_TAG_MASK                = 0x14000000,   // [PG2 p144][PG34 p90] Control the writing of the tag buffer
        ENC_CMD_BITMAP_TRANSFORM_A      = 0x15000000,   // [PG2 p108][PG34 p68] Specify the A coefficient of the bitmap transform matrix
        ENC_CMD_BITMAP_TRANSFORM_B      = 0x16000000,   // [PG2 p109][PG34 p69] Specify the B coefficient of the bitmap transform matrix
        ENC_CMD_BITMAP_TRANSFORM_C      = 0x17000000,   // [PG2 p110][PG34 p69] Specify the C coefficient of the bitmap transform matrix
        ENC_CMD_BITMAP_TRANSFORM_D      = 0x18000000,   // [PG2 p111][PG34 p70] Specify the D coefficient of the bitmap transform matrix
        ENC_CMD_BITMAP_TRANSFORM_E      = 0x19000000,   // [PG2 p112][PG34 p70] Specify the D coefficient of the bitmap transform matrix
        ENC_CMD_BITMAP_TRANSFORM_F      = 0x1A000000,   // [PG2 p113][PG34 p71] Specify the D coefficient of the bitmap transform matrix
        ENC_CMD_SCISSOR_XY              = 0x1B000000,   // [PG2 p138][PG34 p87] Specify the top left corner of the scissor clip rectangle
        ENC_CMD_SCISSOR_SIZE            = 0x1C000000,   // [PG2 p137][PG34 p86] Specify the size of the scissor clip rectangle
        ENC_CMD_CALL                    = 0x1D000000,   // [PG2 p116][PG34 p73] Execute a sequence at another location in the DL
        ENC_CMD_JUMP                    = 0x1E000000,   // [PG2 p129][PG34 p81] Execute commands at another location in the display list
        ENC_CMD_BEGIN                   = 0x1F000000,   // [PG2 p94] [PG34 p56] Begin drawing graphics primitive
        ENC_CMD_COLOR_MASK              = 0x20000000,   // [PG2 p125][PG34 p78] Enable or disable writing of color components
        ENC_CMD_END                     = 0x21000000,   // [PG2 p128][PG34 p80] End drawing a graphics primitive
        ENC_CMD_SAVE_CONTEXT            = 0x22000000,   // [PG2 p136][PG34 p85] Push the current graphics context on the context stack
        ENC_CMD_RESTORE_CONTEXT         = 0x23000000,   // [PG2 p134][PG34 p84] Restore the current graphics context
        ENC_CMD_RETURN                  = 0x24000000,   // [PG2 p135][PG34 p85] Return from a previous CALL command
        ENC_CMD_MACRO                   = 0x25000000,   // [PG2 p131][PG34 p82] Execute a single command from a macro register
        ENC_CMD_CLEAR                   = 0x26000000,   // [PG2 p118][PG34 p74] Clear buffers to preset values
        ENC_CMD_VERTEX_FORMAT           = 0x27000000,   // [PG2 p147][PG34 p92] Set the precision of the VERTEX2F format
        ENC_CMD_BITMAP_LAYOUT_H         = 0x28000000,   // [PG2 p103][PG34 p63] Specify 2 msb's of source bitmap memory format/layout
        ENC_CMD_BITMAP_SIZE_H           = 0x29000000,   // [PG2 p105][PG34 p64] Specify 2 msb's of bitmap dimensions
        ENC_CMD_PALETTE_SOURCE          = 0x2A000000,   // [PG2 p132][PG34 p83] Specify the base address of the palette
        ENC_CMD_VERTEX_TRANSLATE_X      = 0x2B000000,   // [PG2 p148][PG34 p93] Specify the vertex transformations X translation component
        ENC_CMD_VERTEX_TRANSLATE_Y      = 0x2C000000,   // [PG2 p149][PG34 p94] Specify the vertex transformations Y translation component
        ENC_CMD_NOP                     = 0x2D000000,   // [PG2 p131][PG34 p82] No operation
        ENC_CMD_BITMAP_EXT_FORMAT       = 0x2E000000,   //           [PG34 p57] Specify the extended format of the bitmap (EVE3/EVE4)
        ENC_CMD_BITMAP_SWIZZLE          = 0x2F000000,   //           [PG34 p66] Set source for RGBA channels of a bitmap (EVE3/EVE4)
        ENC_CMD_VERTEX2F                = 0x40000000,   // [PG2 p145][PG34 p91] Start operations of graphics primitives at spec'd coordinates based on VERTEX_FORMAT ([PG*] encodes this as 1<<30)
        ENC_CMD_VERTEX2II               = 0x80000000,   // [PG2 p146][PG34 p92] Start operations of graphics primitives at spec'd coords in pixel precision ([PG*] encodes this as 2<<30)

        // Co-processor commands
        ENC_CMD_DLSTART                 = 0xFFFFFF00,   // [PG2 p162][PG34 p112]    Start new Display List
        ENC_CMD_SWAP                    = 0xFFFFFF01,   // [PG2 p163][PG34 p114]    Swap current Display List
        ENC_CMD_INTERRUPT               = 0xFFFFFF02,   // [PG2 p164][PG34 p113]    Trigger interrupt CMDFLAG
//      ENC_CMD_CRC                     = 0xFFFFFF03,   //                          (EVE1?)
//      ENC_CMD_HAMMERAUX               = 0xFFFFFF04,   //                          (EVE1?)
//      ENC_CMD_MARCH                   = 0xFFFFFF05,   //                          (EVE1?)
//      ENC_CMD_IDCT_DELETED            = 0xFFFFFF06,   //                          (EVE1?)
//      ENC_CMD_EXECUTE                 = 0xFFFFFF07,   //                          (EVE1?)
//      ENC_CMD_GETPOINT                = 0xFFFFFF08,   //                          (EVE1?)
        ENC_CMD_BGCOLOR                 = 0xFFFFFF09,   // [PG2 p184][PG34 p130]    Set background color
        ENC_CMD_FGCOLOR                 = 0xFFFFFF0A,   // [PG2 p183][PG34 p129]    Set foreground color
        ENC_CMD_GRADIENT                = 0xFFFFFF0B,   // [PG2 p193][PG34 p134]    Draw a smooth color gradient
        ENC_CMD_TEXT                    = 0xFFFFFF0C,   // [PG2 p213][PG34 p148]    Draw text string
        ENC_CMD_TEXTF                   = 0xFFFFFF0C,   // [PG2 p213][PG34 p148]    Draw text string (Alias for use with program memory string)
        ENC_CMD_BUTTON                  = 0xFFFFFF0D,   // [PG2 p176][PG34 p125]    Draw a button
        ENC_CMD_KEYS                    = 0xFFFFFF0E,   // [PG2 p196][PG34 p137]    Draw a row of keys
        ENC_CMD_PROGRESS                = 0xFFFFFF0F,   // [PG2 p200][PG34 p140]    Draw a progress bar
        ENC_CMD_SLIDER                  = 0xFFFFFF10,   // [PG2 p205][PG34 p143]    Draw a slider
        ENC_CMD_SCROLLBAR               = 0xFFFFFF11,   // [PG2 p203][PG34 p141]    Draw a scrollbar
        ENC_CMD_TOGGLE                  = 0xFFFFFF12,   // [PG2 p210][PG34 p146]    Draw a toggle switch
        ENC_CMD_GAUGE                   = 0xFFFFFF13,   // [PG2 p187][PG34 p131]    Draw a gauge
        ENC_CMD_CLOCK                   = 0xFFFFFF14,   // [PG2 p179][PG34 p126]    Draw an analog clock
        ENC_CMD_CALIBRATE               = 0xFFFFFF15,   // [PG2 p227][PG34 p159]    Interactive touch screen calibration
        ENC_CMD_SPINNER                 = 0xFFFFFF16,   // [PG2 p229][PG34 p161]    Show an animated spinner
        ENC_CMD_STOP                    = 0xFFFFFF17,   // [PG2 p236][PG34 p164]    Stop SKETCH, SPINNER or SCREENSAVER
        ENC_CMD_MEMCRC                  = 0xFFFFFF18,   // [PG2 p173][PG34 p123]    Compute CRC-32 of given RAM_G memory block
        ENC_CMD_REGREAD                 = 0xFFFFFF19,   // [PG2 p166][PG34 p115]    Read a register value
        ENC_CMD_MEMWRITE                = 0xFFFFFF1A,   // [PG2 p167][PG34 p115]    Write memory or registers
        ENC_CMD_MEMSET                  = 0xFFFFFF1B,   // [PG2 p175][PG34 p124]    Fill block of memory with byte value
        ENC_CMD_MEMZERO                 = 0xFFFFFF1C,   // [PG2 p174][PG34 p123]    Fill block of memory with zeros
        ENC_CMD_MEMCPY                  = 0xFFFFFF1D,   // [PG2 p176][PG34 p124]    Copy block of memory
        ENC_CMD_APPEND                  = 0xFFFFFF1E,   // [PG2 p165][PG34 p114]    Append more commands from RAM_G
        ENC_CMD_SNAPSHOT                = 0xFFFFFF1F,   // [PG2 p245][PG34 p171]    Take a snapshot of the current screen
//      ENC_CMD_TOUCH_TRANSFORM         = 0xFFFFFF20,   //                          (EVE1?)
        ENC_CMD_BITMAP_TRANSFORM        = 0xFFFFFF21,   //           [PG34 p188]    Perform bitmap transformation based on 3 points
        ENC_CMD_INFLATE                 = 0xFFFFFF22,   // [PG2 p168][PG34 p116]    Decompress data in RAM_G
        ENC_CMD_GETPTR                  = 0xFFFFFF23,   // [PG2 p222][PG34 p153]    Returns first unallocated memory location
        ENC_CMD_LOADIMAGE               = 0xFFFFFF24,   // [PG2 p169][PG34 p117]    Load JPEG or PNG image into an EVE bitmap in RAM_G
        ENC_CMD_GETPROPS                = 0xFFFFFF25,   // [PG2 p223][PG34 p154]    Returns address and size of bitmap from LOADIMAGE
        ENC_CMD_LOADIDENTITY            = 0xFFFFFF26,   // [PG2 p220][PG34 p152]    Set the current matrix to the identity matrix
        ENC_CMD_TRANSLATE               = 0xFFFFFF27,   // [PG2 p226][PG34 p158]    Apply a translation to the current matrix
        ENC_CMD_SCALE                   = 0xFFFFFF28,   // [PG2 p223][PG34 p155]    Apply a scale to the current matrix
        ENC_CMD_ROTATE                  = 0xFFFFFF29,   // [PG2 p225][PG34 p156]    Apply a rotation to the current matrix
        ENC_CMD_SETMATRIX               = 0xFFFFFF2A,   // [PG2 p220][PG34 p152]    Assign current matrix values to graphics engine
        ENC_CMD_SETFONT                 = 0xFFFFFF2B,   // [PG2 p237][PG34 p165]    Register custom-designed font to co-processor
        ENC_CMD_TRACK                   = 0xFFFFFF2C,   // [PG2 p240][PG34 p168]    Track touches for a graphic object
        ENC_CMD_DIAL                    = 0xFFFFFF2D,   // [PG2 p207][PG34 p144]    Draw a rotary dial control
        ENC_CMD_NUMBER                  = 0xFFFFFF2E,   // [PG2 p217][PG34 p151]    Draw a number
        ENC_CMD_SCREENSAVER             = 0xFFFFFF2F,   // [PG2 p233][PG34 p162]    Start an animated screen saver
        ENC_CMD_SKETCH                  = 0xFFFFFF30,   // [PG2 p234][PG34 p163]    Let user sketch a drawing with the touch panel
        ENC_CMD_LOGO                    = 0xFFFFFF31,   // [PG2 p249][PG34 p174]    Show FTDI or Bridgetek logo animation
        ENC_CMD_COLDSTART               = 0xFFFFFF32,   // [PG2 p163][PG34 p113]    Set the co-processor to the default reset states
        ENC_CMD_GETMATRIX               = 0xFFFFFF33,   // [PG2 p221][PG34 p153]    Retrieve the current graphics engine matrix
        ENC_CMD_GRADCOLOR               = 0xFFFFFF34,   // [PG2 p185][PG34 p130]    Set 3D button highlight color
        ENC_CMD_CSKETCH                 = 0xFFFFFF35,   // [PG2 p249]               Let user sketch a drawing with resistive touch screen (deprecated)(EVE2)
        ENC_CMD_SETROTATE               = 0xFFFFFF36,   // [PG2 p228][PG34 p160]    Rotate the screen
        ENC_CMD_SNAPSHOT2               = 0xFFFFFF37,   // [PG2 p246][PG34 p171]    Take a snapshot of part of the screen
        ENC_CMD_SETBASE                 = 0xFFFFFF38,   // [PG2 p216][PG34 p150]    Set the base for number output
        ENC_CMD_MEDIAFIFO               = 0xFFFFFF39,   // [PG2 p170][PG34 p120]    Set up a streaming media FIFO in RAM_G
        ENC_CMD_PLAYVIDEO               = 0xFFFFFF3A,   // [PG2 p171][PG34 p120]    Play back MJPEG-encoded video
        ENC_CMD_SETFONT2                = 0xFFFFFF3B,   // [PG2 p237][PG34 p165]    Set custom-designed font to co-processor with extended parameters
        ENC_CMD_SETSCRATCH              = 0xFFFFFF3C,   // [PG2 p239][PG34 p166]    Set scratch bitmap handle for widget use
//      ENC_CMD_INT_RAMSHARED           = 0xFFFFFF3D,   //                          (EVE1?)
//      ENC_CMD_INT_SWLOADIMAGE         = 0xFFFFFF3E,   //                          (EVE1?)
        ENC_CMD_ROMFONT                 = 0xFFFFFF3F,   // [PG239 p2][PG34 p167]    Load a ROM font into a bitmap handle
        ENC_CMD_VIDEOSTART              = 0xFFFFFF40,   // [PG2 p172][PG34 p121]    Initialize video frame decoder
        ENC_CMD_VIDEOFRAME              = 0xFFFFFF41,   // [PG2 p172][PG34 p122]    Load next frame of video
        ENC_CMD_SYNC                    = 0xFFFFFF42,   //           [PG34 p187]    Wait for the end of the video scan out (EVE3/EVE4)
        ENC_CMD_SETBITMAP               = 0xFFFFFF43,   // [PG2 p247][PG34 p173]    Generate Display List commands for a bitmap
        ENC_CMD_FLASHERASE              = 0xFFFFFF44,   //           [PG34 p174]    Erase flash storage (EVE3/EVE4)
        ENC_CMD_FLASHWRITE              = 0xFFFFFF45,   //           [PG34 p174]    Write data from host to flash storage (EVE3/EVE4)
        ENC_CMD_FLASHREAD               = 0xFFFFFF46,   //           [PG34 p176]    Read data from flash to RAM_G (EVE3/EVE4)
        ENC_CMD_FLASHUPDATE             = 0xFFFFFF47,   //           [PG34 p177]    Write given data from RAM_G to flash, erasing and skipping as needed (EVE3/EVE4)
        ENC_CMD_FLASHDETACH             = 0xFFFFFF48,   //           [PG34 p177]    Put flash storage SPI lines into hi-Z mode (EVE3/EVE4)
        ENC_CMD_FLASHATTACH             = 0xFFFFFF49,   //           [PG34 p178]    Reconnect to the flash storage via SPI (EVE3/EVE4)
        ENC_CMD_FLASHFAST               = 0xFFFFFF4A,   //           [PG34 p178]    Drive the flash storage in full-speed mode if possible (EVE3/EVE4)
        ENC_CMD_FLASHSPIDESEL           = 0xFFFFFF4B,   //           [PG34 p179]    De-assert SPI CS for the flash storage device (EVE3/EVE4)
        ENC_CMD_FLASHSPITX              = 0xFFFFFF4C,   //           [PG34 p179]    Transmit data from host to flash storage SPI interface (EVE3/EVE4)
        ENC_CMD_FLASHSPIRX              = 0xFFFFFF4D,   //           [PG34 p179]    Receive data from flash storage SPI interface to RAM_G (EVE3/EVE4)
        ENC_CMD_FLASHSOURCE             = 0xFFFFFF4E,   //           [PG34 p181]    Specify source address for flash storage data (EVE3/EVE4)
        ENC_CMD_CLEARCACHE              = 0xFFFFFF4F,   //           [PG34 p180]    Clear graphics engine cache after changes in flash storage (EVE3/EVE4)
        ENC_CMD_INFLATE2                = 0xFFFFFF50,   //           [PG34 p117]    Decompress data in RAM_G with options (EVE3/EVE4)
        ENC_CMD_ROTATEAROUND            = 0xFFFFFF51,   //           [PG34 p157]    Apply a rotation around a specified coordinate (EVE3/EVE4)
        ENC_CMD_RESETFONTS              = 0xFFFFFF52,   //           [PG34 p167]    Load bitmap handles 16-31 with their default fonts (EVE3/EVE4)
        ENC_CMD_ANIMSTART               = 0xFFFFFF53,   //           [PG34 p181]    Start an animation from flash storage (EVE3/EVE4)
        ENC_CMD_ANIMSTOP                = 0xFFFFFF54,   //           [PG34 p184]    Stop an animation or all animations (EVE3/EVE4)
        ENC_CMD_ANIMXY                  = 0xFFFFFF55,   //           [PG34 p185]    Set the coordinates of an animation (EVE3/EVE4)
        ENC_CMD_ANIMDRAW                = 0xFFFFFF56,   //           [PG34 p185]    Draw one or more active animations (EVE3/EVE4)
        ENC_CMD_GRADIENTA               = 0xFFFFFF57,   //           [PG34 p136]    Draw smooth color gradient with transparency (EVE3/EVE4)
        ENC_CMD_FILLWIDTH               = 0xFFFFFF58,   //           [PG34 p147]    Set the pixel fill width for various commands (EVE3/EVE4)
        EMC_CMD_APPENDF                 = 0xFFFFFF59,   //           [PG34 p176]    Append data from flash storage to Display List (EVE3/EVE4)
        ENC_CMD_ANIMFRAME               = 0xFFFFFF5A,   //           [PG34 p185]    Draw the specified frame of an animation (EVE3/EVE4)
//      unused                          = 0xFFFFFF5B,
//      unused                          = 0xFFFFFF5C,
//      unused                          = 0xFFFFFF5D,
//      unused                          = 0xFFFFFF5E,
        ENC_CMD_VIDEOSTARTF             = 0xFFFFFF5F,   //           [PG34 p181]    Initialize video frame decoder from flash storage (EVE3/EVE4)
        ENC_CMD_CALIBRATESUB            = 0xFFFFFF60,   //           [PG34 p159]    Execute touch screen calibration for a sub-window (EVE3/EVE4)
        ENC_CMD_TESTCARD                = 0xFFFFFF61,   //           [PG34 p189]    Load a Display List with a test card graphic (EVE4)
        ENC_CMD_HSF                     = 0xFFFFFF62,   //           [PG34 p195]    Non-square pixel correction (EVE4)
        ENC_CMD_APILEVEL                = 0xFFFFFF63,   //           [PG34 p112]    Set the co-processor API level (EVE4)
        ENC_CMD_GETIMAGE                = 0xFFFFFF64,   //           [PG34 p194]    Get attributes of bitmap loaded previously (EVE4)
        ENC_CMD_WAIT                    = 0xFFFFFF65,   //           [PG34 p190]    Wait for specified number of microseconds (EVE4)
        ENC_CMD_RETURNLIST              = 0xFFFFFF66,   //           [PG34 p192]    End execution of a command list (NOTE: renamed because of conflict with 0x24000000) (EVE4)
        ENC_CMD_CALLLIST                = 0xFFFFFF67,   //           [PG34 p192]    Call a command list in RAM_G (EVE4)
        ENC_CMD_NEWLIST                 = 0xFFFFFF68,   //           [PG34 p190]    Start a command list in RAM_G (EVE4)
        ENC_CMD_ENDLIST                 = 0xFFFFFF69,   //           [PG34 p191]    Terminate command list in RAM_G (EVE4)
        ENC_CMD_PCLKFREQ                = 0xFFFFFF6A,   //           [PG34 p196]    Generate pixel clock as close as possible to the one requested (EVE4)
        ENC_CMD_FONTCACHE               = 0xFFFFFF6B,   //           [PG34 p193]    Enable font cache for extended flash-based fonts (EVE4)
        ENC_CMD_FONTCACHEQUERY          = 0xFFFFFF6C,   //           [PG34 p194]    Query the capacity and utilization of the font cache (EVE4)
        ENC_CMD_ANIMFRAMERAM            = 0xFFFFFF6D,   //           [PG34 p186]    Draw a specified frame of an animation from RAM_G (EVE4)
        ENC_CMD_ANIMSTARTRAM            = 0xFFFFFF6E,   //           [PG34 p182]    Start an animation from RAM_G (EVE4)
        ENC_CMD_RUNANIM                 = 0xFFFFFF6F,   //           [PG32 p183]    Wait until run-once animation is complete (EVE4)
        ENC_CMD_FLASHPROGRAM            = 0xFFFFFF70,   //           [PG34 p175]    Write data from RAM_G to flash storage (EVE3/EVE4)

    };

    // Alpha test function for ALPHA_FUNC (ProgGuide 4.4 p.92)
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
        FORMAT_ARGB1555                 = 0,
        FORMAT_L1                       = 1,
        FORMAT_L4                       = 2,
        FORMAT_L8                       = 3,
        FORMAT_RGB332                   = 4,
        FORMAT_ARGB2                    = 5,
        FORMAT_ARGB4                    = 6,
        FORMAT_RGB565                   = 7,
        FORMAT_TEXT8X8                  = 9,
        FORMAT_TEXTVGA                  = 10,
        FORMAT_BARGRAPH                 = 11,
        FORMAT_PALETTED565              = 14,
        FORMAT_PALETTED4444             = 15,
        FORMAT_PALETTED8                = 16,
        FORMAT_L2                       = 17,

        FORMAT_ARGB8                    = 0x20,     // Used by SNAPSHOT2 only

        COMPRESSED_RGBA_ASTC_4x4_KHR    = 37808,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_5x4_KHR    = 37809,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_5x5_KHR    = 37810,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_6x5_KHR    = 37811,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_6x6_KHR    = 37812,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_8x5_KHR    = 37813,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_8x6_KHR    = 37814,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_8x8_KHR    = 37815,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_10x5_KHR   = 37816,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_10x6_KHR   = 37817,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_10x8_KHR   = 37818,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_10x10_KHR  = 37819,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_12x10_KHR  = 37820,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)
        COMPRESSED_RGBA_ASTC_12x12_KHR  = 37821,    // Used by BITMAP_EXT_FORMAT only (EVE3/EVE4)

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
    enum BLEND
    {
        BLEND_ZERO                  = 0,
        BLEND_ONE                   = 1,
        BLEND_SRC_ALPHA             = 2,
        BLEND_DST_ALPHA             = 3,
        BLEND_ONE_MINUS_SRC_ALPHA   = 4,
        BLEND_ONE_MINUS_DST_ALPHA   = 5,
    };

    // Stencil operations for STENCIL_OP (ProgGuide 4.44 Figure 10 p.141)
    enum STENCIL
    {
        STENCIL_ZERO                = 0,
        STENCIL_KEEP                = 1,
        STENCIL_REPLACE             = 2,
        STENCIL_INCR                = 3,
        STENCIL_DECR                = 4,
        STENCIL_INVERT              = 5,
    };

    // Options for coprocessor commands (ProgGuide 5.8 p.158)
    // NOTE: These are flags; they may be combined.
    // Remarks show commands for which the options are valid
    enum OPT {
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
    }; // 16 bits

    // Values for REG_DLSWAP (see ProgGuide p.30)
    enum DLSWAP {
        // Don't store this value; wait until the register is this value
        // before storing another value.
        DLSWAP_DONE                 = 0x0,

        DLSWAP_LINE                 = 0x1,    // Start reading from current DL after current line
        DLSWAP_FRAME                = 0x2,    // Start reading from current DL after current frame
    }; // 2 bits

    // Values for REG_INT_EN and REG_INT_FLAGS (see Datasheet 4.1.6 p.20)
    // NOTE: These are flags; they may be combined.
    enum INT // 8 bits (EVE2/EVE3), 9 bits (EVE4)
    {
        // [DS2 p20][DS3 p19]
        INT_SWAP                    = 0x001,    // DL swap occurred
        INT_TOUCH                   = 0x002,    // Touch detected
        INT_TAG                     = 0x004,    // Touch screen tag value changed
        INT_SOUND                   = 0x008,    // Sound effect ended
        INT_PLAYBACK                = 0x010,    // Audio playback ended
        INT_CMDEMPTY                = 0x020,    // Command FIFO empty
        INT_CMDFLAG                 = 0x040,    // Flag set by command
        INT_CONVCOMPLETE            = 0x080,    // Touch screen conversion complete
        INT_UNDERRUN                = 0x100,    // (EVE4 only) Graphics pipeline underrun

        //INT_L8C                     = 0x0C,   // 
        //INT_VGA                     = 0x0D,   // 
        //INT_G8                      = 0x12,   // 
    }; // 8 bits

    // Values for REG_PLAYBACK_FORMAT (see ProgGuide p.40)
    enum SAMPLES {
        SAMPLES_LINEAR                = 0x0,   // Linear audio samples
        SAMPLES_ULAW                  = 0x1,   // uLaw audio samples
        SAMPLES_ADPCM                 = 0x2,   // IMA ADPCM audio samples
    }; // 2 bits

    // Values for REG_TOUCH_MODE (see ProgGuide p.58)
    enum TOUCHMODE {
        TOUCHMODE_OFF                 = 0x0,   // Touch mode off
        TOUCHMODE_ONESHOT             = 0x1,   // Read one touch sample
        TOUCHMODE_FRAME               = 0x2,   // Read one touch sample each frame
        TOUCHMODE_CONTINUOUS          = 0x3,   // Continuous touch mode up to 1000 Hz
    }; // 2 bits

    // Chip identifiers
    enum CHIPID
    {
        // Use this value in the init parameters to skip chip ID checking
        CHIPID_ANY                  = 0,

        // Following are values in the chip ID register just after the
        // processor has been started.
        CHIPID_FT810                = 0x00011008,
        CHIPID_FT811                = 0x00011108,
        CHIPID_FT812                = 0x00011208,
        CHIPID_FT813                = 0x00011308,
    };

    // Values for CMD_APILEVEL (EVE4 only)
    enum APILEVEL
    {
        APILEVEL_BT815                  = 1,    // BT815 (EVE3) mode
        APILEVEL_BT817_BT818            = 2,    // BT817/818 (EVE4) mode
    };

    // Values for CMD_ANIMSTART (EVE4 only)
    //
    // Not documented; found in EVE_defines.h from Bridgetek.
    enum ANIM
    {
        ANIM_ONCE                       = 0,    // Play animation once
        ANIM_LOOP                       = 1,    // Keep looping
        ANIM_HOLD                       = 2,
    };

    //-----------------------------------------------------------------------
    // This struct is used to describe the hardware parameters for a 
    // particular LCD display panel.
    struct DisplayProfile
    {
        bool        _clkext;            // True=external clock
        CLKSEL      _clksel;            // Clock multiplier
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

    public:
        //-------------------------------------------------------------------
        // Constructor
        //
        // This generates some of the timing values based on the given
        // parameters. 
        DisplayProfile(
            uint16_t width,             // Horizontal number of pixels
            uint16_t height,            // Vertical number of pixels
            uint16_t hfrontporch,       // Num clocks from display to sync
            uint16_t hsyncwidth,        // Number of clocks in hsync
            uint16_t hbackporch,        // Num clocks from hsync to display
            uint16_t vfrontporch,       // Num lines from display to vsync
            uint16_t vsyncheight,       // Number of lines in vsync
            uint16_t vbackporch,        // Num lines from vsync to display
            uint16_t hpadding,          // Num additional clocks per line
            uint16_t vpadding,          // Num additional lines per frame
            uint8_t pclk,               // Clock divisor
            uint8_t pclkpol = 1,        // Clock policy
            uint8_t swizzle = 0)        // Pin order
        : _clkext(false)
        , _clksel(CLKSEL_DEFAULT)
        , _chipid(CHIPID_ANY)
        , _frequency(0)
        , _lcd10ma(false)
        , _cspread(false)
        , _dither(false)
        , _outbits(0)
        , _hsize(width)
        , _hcycle(hfrontporch + hsyncwidth + hbackporch + width + hpadding)
        , _hoffset(hfrontporch + hsyncwidth + hbackporch)
        , _hsync0(hfrontporch)
        , _hsync1(hfrontporch + hsyncwidth)
        , _vsize(height)
        , _vcycle(vfrontporch + vsyncheight + vbackporch + height + vpadding)
        , _voffset(vfrontporch + vsyncheight + vbackporch)
        , _vsync0(vfrontporch)
        , _vsync1(vfrontporch + vsyncheight)
        , _swizzle(swizzle)
        , _pclkpol(pclkpol)
        , _pclk(pclk)
        {
            // Nothing
        }
    };

public:
    typedef Index<RAM_CMD_SIZE> CmdIndex;
    typedef Index<RAM_DL_SIZE> DLIndex;

    /////////////////////////////////////////////////////////////////////////
    // DATA
    /////////////////////////////////////////////////////////////////////////

protected:
    const DisplayProfile
                   &_profile;           // Display init parameters
    SteveHAL       &_hal;               // Communication functions

    // Cached constants
    const uint16_t  _hcenter;           // Horizontal center in pixels
    const uint16_t  _vcenter;           // Vertical center in pixels_

    // State variables
    CmdIndex        _cmd_index;         // Graphics engine cmd write index
                                        //   (offset from RAM_CMD)
    DLIndex         _dl_index;          // Display list write index
                                        //   (offset from RAM_DL)

    /////////////////////////////////////////////////////////////////////////
    // CONSTRUCTOR
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Constructor
    //
    // NOTE: Although the pin numbers are optional, the display probably
    // won't function without them, unless you create a subclass that
    // controls those lines by overriding the appropriate functions.
    Steve(
      const DisplayProfile &profile,    // Display profile
      SteveHAL &hal)                    // Hardware functions
      : _profile(profile)
      , _hal(hal)
      , _hcenter(profile._hsize / 2)
      , _vcenter(profile._vsize / 2)
      , _cmd_index()
      , _dl_index()
    {
      // Nothing here
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
    // INITIALIZATION
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Initialize the clock from a single enum value
    //
    // NOTE: The clock must be stopped before calling this.
    void InitClock(
        bool extclock,
        CLKSEL clksel)
    {
        DBG_TRAFFIC("Using %s clock and PLL mode 0x%X\n",
            extclock ? "external" : "internal", clksel);

        // Select the internal or external clock, and select the PLL
        // multiplier for an external clock as necessary.
        if (extclock)
        {
            HostCommand(HOSTCMD_CLKEXT);
        }
        else
        {
            HostCommand(HOSTCMD_CLKINT);
        }

        HostCommand(HOSTCMD_CLKSEL, (uint8_t)clksel);
    }

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
      _hal.Delay(20);                   // Wait a few ms before waking it up
      _hal.Power(false);                // Reset
      _hal.Delay(6);                    // Hold for a little while
      _hal.Power(true);                 // Power on
      _hal.Delay(21);

      // Select the chip and initialize the SPI bus in slow mode until
      // the EVE clock is initialized
      _hal.Select(true);
      _hal.Init(true);

      // Initialize the clock
      InitClock(_profile._clkext, _profile._clksel);

      // Activate the FT81X and give it some time to initialize
      HostCommand(HOSTCMD_ACTIVE, 0);
      _hal.Delay(40);

      // Re-init in fast mode
      _hal.Init(false);

      // Repeatedly poll REG_ID with up to 250 maximum retries and a 1 ms
      // delay between retries.
      // The register should return 0x7C when the processor is running.
      if (!RegWait8(REG_ID, 0x7C, 250, 1))
      {
        // TODO: report problem and return false
        DBG_STAT("Timeout waiting for ID of 0x7C.\n");
        DBG_STAT("Is the device connected? Is the right EVE device selected?\n");
        return false;
      }

      // Repeatedly poll REG_CPURESET until it returns 0 meaning the reset
      // is complete
      while (!RegWait8(REG_CPURESET, 0, 250, 1))
      {
        // TODO: report problem and return false
        DBG_STAT("Timeout waiting for EVE CPU reset.\n");
        DBG_STAT("Is the device connected? Is the right EVE device selected?\n");
        // TODO: in at the top, out at the bottom
        return false;
      }

      // Read the chip ID and match it with the expected value
      if (_profile._chipid != CHIPID_ANY)
      {
        uint32_t chip_id = RegRead32(REG_CHIP_ID);
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
        RegWrite32(REG_FREQUENCY, _profile._frequency);
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
      RegWrite8(REG_PCLK, 0);

      // Turn off backlight
      RegWrite8(REG_PWM_DUTY, 0);

      // Initialize display parameters
      RegWrite16(REG_HSIZE,    _profile._hsize);   // active display width
      RegWrite16(REG_HCYCLE,   _profile._hcycle);  // total number of clocks per line, incl front/back porch
      RegWrite16(REG_HOFFSET,  _profile._hoffset); // start of active line
      RegWrite16(REG_HSYNC0,   _profile._hsync0);  // start of horizontal sync pulse
      RegWrite16(REG_HSYNC1,   _profile._hsync1);  // end of horizontal sync pulse
      RegWrite16(REG_VSIZE,    _profile._vsize);   // active display height
      RegWrite16(REG_VCYCLE,   _profile._vcycle);  // total number of lines per screen, incl pre/post
      RegWrite16(REG_VOFFSET,  _profile._voffset); // start of active screen
      RegWrite16(REG_VSYNC0,   _profile._vsync0);  // start of vertical sync pulse
      RegWrite16(REG_VSYNC1,   _profile._vsync1);  // end of vertical sync pulse
      RegWrite8( REG_SWIZZLE,  _profile._swizzle); // FT800 output to LCD - pin order
      RegWrite8( REG_PCLK_POL, _profile._pclkpol); // LCD data is clocked in on this PCLK edge
      // Don't set PCLK yet - wait for just after the first display list

      // Set 10 mA or 5 mA drive for PCLK, DISP, VSYNC, DE, RGB lines and
      // back light PWM.
      if (_profile._lcd10ma)
      {
        RegWrite16(REG_GPIOX, RegRead16(REG_GPIOX) | 0x1000);
      }
      else
      {
        RegWrite16(REG_GPIOX, RegRead16(REG_GPIOX) & ~0x1000);
      }

      // Enable or disable RGB clock spreading for reduced noise
      RegWrite8(REG_CSPREAD, _profile._cspread ? 1 : 0);

      // Enable or disable dithering
      RegWrite8(REG_DITHER, _profile._dither ? 1 : 0);

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
      dl_CLEAR_COLOR(0);
      dl_CLEAR(1, 1, 1); // color, stencil, tag
      dl_DISPLAY();

      // Tell the EVE that it can swap display lists at the next available
      // frame boundary.
      // TODO: Make this a parameter for Begin()?
      RegWrite32(REG_DLSWAP, DLSWAP_FRAME);

      // Enable the DISP line of the LCD.
      // TODO: Is this specific to CFA10099?
      RegWrite16(REG_GPIOX, RegRead16(REG_GPIOX) | 0x8000);

      // Now start clocking the data to the LCD panel
      RegWrite8(REG_PCLK, _profile._pclk);

      // Initialize backlight
      // TODO: Make these into parameter for Begin()?
      RegWrite16(REG_PWM_HZ, 300);
      RegWrite8(REG_PWM_DUTY, 32);

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
        RegWrite8(REG_TOUCH_MODE, 0);
        // Eliminate any false touches
        RegWrite16(REG_TOUCH_RZTHRESH, 0);

        return true;
    }

    /////////////////////////////////////////////////////////////////////////
    // START AND END TRANSACTIONS
    /////////////////////////////////////////////////////////////////////////

protected:
    //-----------------------------------------------------------------------
    // Begin a transaction, i.e. a Host Command, or a Memory Read/Write
    //
    // In most cases, this shouldn't be called directly. Call the other
    // functions below instead to send a host command or read/write from/to
    // memory.
    void BeginTransaction(
        uint32_t data24)                // 24-bit value to send
    {
        DBG_TRAFFIC("*** Transaction %06X\n", data24);

        // Make sure the previous transaction has ended.
        // Then start a new transaction by selecting the chip.
        EndTransaction();

        // TODO: Delay?
        _hal.Select(true);

        // Send the lower 3 bytes of the command in BIG ENDIAN order.
        _hal.Send8((uint8_t)(data24 >> 16));
        _hal.Send8((uint8_t)(data24 >> 8));
        _hal.Send8((uint8_t)(data24));
    }

protected:
    //-----------------------------------------------------------------------
    // End a transaction by de-selecting the chip.
    //
    // This is usually not necessary: Beginning a new transaction will end
    // the previous transaction.
    void EndTransaction()
    {
        _hal.Select(false);
    }

protected:
    //-----------------------------------------------------------------------
    // Read or write data
    //
    // After calling this, subsequent transfers will copy data to/from
    // consecutive memory locations.
    void BeginMemoryTransaction(
        uint32_t address22,             // Address (22 bits, not checked)
        bool write)                     // False = read, true = write
    {
        DBG_TRAFFIC("Address %X %s\n", address22, write ? "WRITE" : "READ");

        // The address is passed by or-ing it to the 24 bit command.
        BeginTransaction((uint32_t)(write ? HOSTCMD_WRITE : HOSTCMD_READ) | address22);

        // In read mode, a dummy byte must be sent to the EVE before
        // receiving the data.
        if (!write)
        {
            _hal.Send8(0);
        }
    }

protected:
    //-----------------------------------------------------------------------
    // Send a Host Command
    void HostCommand(
        HOSTCMD hostcmd,                // Host command (not READ or WRITE)
        uint8_t parameter = 0)          // Parameter, if any
    {
        // The parameter is passed as the second byte of the 24-bit host
        // command value.
        return BeginTransaction((uint32_t)hostcmd | (uint32_t)parameter << 8);
    }

    /////////////////////////////////////////////////////////////////////////
    // MEMORY OPERATIONS
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Read a one-byte register from FT81X memory
    //
    // Referred to as "rd8" in the documentation
    uint8_t                             // Returns data at given address
    RegRead8(
        uint32_t address22)             // Address (22 bits; not checked)
    {
        uint8_t result;

        // Send the 22-bit address and operation flag.
        BeginMemoryTransaction(address22, false);

        // Get the value
        result = _hal.Receive8();

        DBG_TRAFFIC("Reg %X = %X\n", address22, result);

        return result;
    }

public:
    //-----------------------------------------------------------------------
    // Read a 2-byte register from FT81X memory
    //
    // Referred to as "rd16" in the documentation
    uint16_t                            // Returns data at given address
    RegRead16(
        uint32_t address22)             // Address (22 bits; not checked)
    {
        uint16_t result;

        // Send the 22-bit address and operation flag.
        BeginMemoryTransaction(address22, false);

        // Get the value
        result = _hal.Receive16();

        DBG_TRAFFIC("Reg %X = %X\n", address22, result);

        return result;
    }

public:
    //-----------------------------------------------------------------------
    // Read a 4-byte register from FT81X memory
    //
    // Referred to as "rd32" in the documentation
    uint32_t                            // Returns data at given address
    RegRead32(
        uint32_t address22)             // Address (22 bits; not checked)
    {
        uint32_t result;

        // Send the 22-bit address and operation flag.
        BeginMemoryTransaction(address22, false);

        // Get the value
        result = _hal.Receive32();

        DBG_TRAFFIC("Reg %X = %X\n", address22, result);

        return result;
    }

public:
    //-----------------------------------------------------------------------
    // Repeat reading a register until it matches the given value
    //
    // The function returns 0 if the register didn't contain the expected
    // value after the given number of retries. In other words, the result
    // is successful if the function returns nonzero.
    uint8_t                             // Returns num retries remaining
    RegWait8(
        uint32_t address22,             // Address (22 bits; not checked)
        uint8_t value,                  // Expected value
        uint8_t maxtries,               // Maximum number of tries
        uint32_t delay_between_tries)   // Delay between tries in ms
    {
        uint8_t result = maxtries;
        uint8_t read_value = 0;

        while (result)
        {
            read_value = RegRead8(address22);

            if (read_value == value)
            {
                DBG_TRAFFIC("Match after %u tries\n", maxtries - result);
                return result;
            }

            _hal.Delay(delay_between_tries);

            result--;
        }

        DBG_GEEK("Timeout waiting for %X to become %X, last read value was %X\n", address22, value, read_value);
        return result;
    }

public:
    //-----------------------------------------------------------------------
    // Read a block of memory
    uint32_t                            // Returns next address to read from
    RegReadBuffer(
        uint32_t address22,             // Address (22 bits; not checked)
        uint32_t length,                // Number of bytes to read
        uint8_t *destination)           // Destination buffer
    {
        DBG_TRAFFIC("Reading %X length %X (%u dec)\n", address22, length, length);

        BeginMemoryTransaction(address22, false);

        address22 += _hal.ReceiveBuffer(destination, length);

        return address22;
    }

public:
    //-----------------------------------------------------------------------
    // Write an 8 bit register
    //
    // Referred to as "wr8" in the documentation
    void RegWrite8(
        uint32_t address22,             // Address (22 bits; not checked)
        uint8_t value)                  // Value to store
    {
        DBG_TRAFFIC("Writing %X = %02X\n", address22, value);

        BeginMemoryTransaction(address22, true);

        _hal.Send8(value);
    }

public:
    //-----------------------------------------------------------------------
    // Write a 16 bit register
    //
    // Referred to as "wr16" in the documentation
    void RegWrite16(
        uint32_t address22,             // Address (22 bits; not checked)
        uint16_t value)                 // Value to store
    {
        DBG_TRAFFIC("Writing %X = %04X\n", address22, value);

        BeginMemoryTransaction(address22, true);

        _hal.Send16(value);
    }

public:
    //-----------------------------------------------------------------------
    // Write a 32 bit register
    //
    // Referred to as "wr32" in the documentation
    void RegWrite32(
        uint32_t address22,             // Address (22 bits; not checked)
        uint32_t value)                 // Value to store
    {
        DBG_TRAFFIC("Writing %X = %08X\n", address22, value);

        BeginMemoryTransaction(address22, true);

        _hal.Send32(value);
    }

public:
    //-----------------------------------------------------------------------
    // Write a block of memory
    //
    // NOTE: It may be necessary to send alignment bytes if the length
    // is not a multiple of 4.
    uint32_t                            // Returns next address to write to
    RegWriteBuffer(
        uint32_t address22,             // Address (22 bits; not checked)
        uint32_t length,                // Number of bytes to read
        const uint8_t *source)          // Source buffer
    {
        DBG_TRAFFIC("Writing %X length %X (%u dec)\n", address22, length, length);

        BeginMemoryTransaction(address22, true);

        address22 += _hal.SendBuffer(source, length);

        return address22;
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

public:
    //-----------------------------------------------------------------------
    // Reset the display list index
    //
    // NOTE: It shouldn't be necessary to get the display list index
    // without storing a command first, so there is no function to get the
    // current index without storing anything.
    void DLResetIndex(
        uint16_t index = 0)             // New index
    {
        DBG_TRAFFIC("DL reset %u\n", index);

        // The optimizer should be smart enough to turn the mod operator
        // into an AND operation.
        _dl_index = index % RAM_DL_SIZE;
    }

public:
    //-----------------------------------------------------------------------
    // Store a 32 bit display list command
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    //
    // Referred to as "dl" in the documentation.
    DLIndex                             // Returns updated DL index
    DLAdd(
        uint32_t value)                 // Value to write
    {
        DBG_TRAFFIC("dl(%08X)\n", value);

        RegWrite32(RAM_DL + _dl_index.index(), value);

        _dl_index += 4;

        return _dl_index;
    }

    /////////////////////////////////////////////////////////////////////////
    // CO-PROCESSOR SUPPORT
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Read a 32 bit value from the given Cmd index
    //
    // This can be used to retrieve a value that gets stored by the
    // co-processor into the output parameters of a command.
    uint32_t                            // Returns value
    CmdRead32(
        CmdIndex cmdindex)              // Command index to read from
    {
        return RegRead32(RAM_CMD + cmdindex.index());
    }

public:
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
    CmdIndex                            // Returns updated Cmd index
    CmdInitWriteIndex()
    {
        DBG_TRAFFIC("Reading REG_CMD_WRITE\n");

        _cmd_index = CmdIndex(RegRead16(REG_CMD_WRITE));

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Get amount of free space in the command queue
    //
    // The number is based on the location where the chip is reading
    // (not writing). So this can be called repeatedly to check if there's
    // enough space for a certain command, e.g. when sending large amounts
    // of data such as a bitmap.
    //
    // See also App Note 240 p.21
    uint16_t                            // Returns number of bytes free
    CmdGetFreeCmdSpace()
    {
        // Calculate the used space by subtracting the read index from
        // our write index. This value is wrapped around the maximum value.
        uint16_t used_space = (_cmd_index - RegRead16(REG_CMD_READ)).index();

        // Subtract the used space from the total space but reduce the
        // total space by 4 to avoid wrapping the maximum value to zero.
        uint16_t result = (RAM_CMD_SIZE - 4) - used_space;

        DBG_TRAFFIC("Free command space is %u", result);

        return result;
    }

public:
    //-----------------------------------------------------------------------
    // Store a co-processor command with no parameters
    //
    // The class keeps track of the current location and updates it to the
    // next location. Normally it's not necessary to do anything with the
    // return value.
    CmdIndex                            // Returns updated Cmd index
    Cmd(
        uint32_t command)               // Command to queue
    {
        DBG_GEEK("cmd(%08X)\n", command);

        BeginMemoryTransaction(RAM_CMD + _cmd_index.index(), true);

        // Send the command
        _hal.Send32(command);

        return _cmd_index += 4;
    }

public:
    //-----------------------------------------------------------------------
    // Check if the co-processor is busy
    bool                                // Returns true=busy, false=ready
    CmdIsBusy(
        bool *pError = NULL)            // Optional output true=error
    {
        uint16_t readindex = RegRead16(REG_CMD_READ);

        bool error = (readindex == READ_INDEX_ERROR);

        if (pError)
        {
            *pError = error;
        }

        return (!error) && (readindex != _cmd_index.index());
    }

public:
    //-----------------------------------------------------------------------
    // Wait until the co-processor has caught up.
    //
    // If the co-processor has nothing to do, the function will return
    // immediately.
    //
    // NOTE: Simply adding commands doesn't start the co-processor. You must
    // call the CmdExecute() function below.
    //
    // This can be used to wait for the end of a frame (if REG_DL_SWAP is
    // in mode DLSWAP_FRAME), and to retrieve the location where the next
    // command will be stored, without storing another command first.
    CmdIndex                            // Returns updated Cmd index
    CmdWaitComplete(
        bool *pError = NULL)            // Optional output true=error
    {
        DBG_TRAFFIC("Waiting for coprocessor\n");

        while (CmdIsBusy(pError))
        {
            // Nothing
        }

        return _cmd_index;
    }

public:
    //-----------------------------------------------------------------------
    // Tell the co-processor to start executing commands
    //
    // This updates the write pointer on the engine to the current write
    // location so that the coprocessor starts executing commands in the
    // command queue.
    CmdIndex                            // Returns updated Cmd index
    CmdExecute(
        bool waituntilcomplete = false, // True = wait until done
        bool *pError = NULL)            // Optional output true=error
    {
        DBG_TRAFFIC("Executing command queue\n");

        RegWrite16(REG_CMD_WRITE, _cmd_index.index());

        if (waituntilcomplete)
        {
            CmdWaitComplete(pError);
        }

        return _cmd_index;
    }

    /////////////////////////////////////////////////////////////////////////
    // COMMAND ENCODING
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Macro to encode a bit field in a uint32_t.
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
    // The macro may look pretty daunting but the optimizer will reduce it
    // to code that's basically equivalent to (value & constant) << constant
    // so this should compile to some pretty small.
    #define N(value, leftbit, rightbit) ((((uint32_t)value) & ((1 << (((leftbit) - (rightbit)) + 1)) - 1)) << rightbit)

    //-----------------------------------------------------------------------
    // Functions for Display List commands
    //
    // A single invocation of the macro expands to:
    // * A function (starting with ENC_...) to encode the bit fields into a
    //   uint32_t value
    // * A function (starting with  dl_...) to add an encoded command to the
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
        DLIndex   dl_##name declaration { return DLAdd(ENC_##name parameters); } \
        CmdIndex cmd_##name declaration { return Cmd(ENC_##name parameters); }

    ENC(ALPHA_FUNC,         (FUNC     func,     uint8_t  ref8),                                                             (func, ref8),                               N(func,     10,  8) | N(ref8,      7,  0)                                                            ) // ProgGuide 4.4 p.92
    ENC(BITMAP_HANDLE,      (uint8_t  handle5),                                                                             (handle5),                                  N(handle5,   4,  0)                                                                                  ) // ProgGuide 4.6 p.96
    ENC(BEGIN,              (BEGIN    prim),                                                                                (prim),                                     N(prim,      3,  0)                                                                                  ) // ProgGuide 4.5 p.94
    ENC(BITMAP_EXT_FORMAT,  (FORMAT   format),                                                                              (format),                                   N(format,   15,  0)                                                                                  ) //          [PG34 p57] (EVE3/EVE4)
    ENC(BITMAP_LAYOUT,      (FORMAT   format,   uint32_t stride10, uint32_t height9),                                       (format, stride10, height9),                N(format,   23, 19) | N(stride10, 18,  9) | N(height9,  8,  0)                                       ) // ProgGuide 4.7 p.97
    ENC(BITMAP_LAYOUT_H,    (uint32_t strideh2, uint32_t heighth2),                                                         (strideh2, heighth2),                       N(strideh2,  3,  2) | N(heighth2,  1,  0)                                                            ) // ProgGuide 4.8 p.103
    ENC(BITMAP_SIZE,        (FILTER   filter,   WRAP     wrapx,    WRAP     wrapy,   uint16_t width9, uint16_t height9),    (filter, wrapx, wrapy, width9, height9),    N(filter,   20, 20) | N(wrapx,    19, 19) | N(wrapy,   18, 18) | N(width9, 17,  9) | N(height9, 8, 0)) // ProgGuide 4.9 p.103
    ENC(BITMAP_SIZE_H,      (uint16_t widthh2,  uint16_t heighth2),                                                         (widthh2, heighth2),                        N(widthh2,   3,  2) | N(heighth2,  1,  0)                                                            ) // ProgGuide 4.10 p.105
    ENC(BITMAP_SOURCE,      (uint32_t addr22),                                                                              (addr22),                                   N(addr22,   21,  0)                                                                                  ) // ProgGuide 4.11 p.106
    ENC(BITMAP_TRANSFORM_A, (uint32_t a17),                                                                                 (a17),                                      N(a17,      16,  0)                                                                                  ) // ProgGuide 4.12 p.108
    ENC(BITMAP_TRANSFORM_B, (uint32_t b17),                                                                                 (b17),                                      N(b17,      16,  0)                                                                                  ) // ProgGuide 4.13 p.109
    ENC(BITMAP_TRANSFORM_C, (uint32_t c24),                                                                                 (c24),                                      N(c24,      23,  0)                                                                                  ) // ProgGuide 4.14 p.110
    ENC(BITMAP_TRANSFORM_D, (uint32_t d17),                                                                                 (d17),                                      N(d17,      16,  0)                                                                                  ) // ProgGuide 4.15 p.111
    ENC(BITMAP_TRANSFORM_E, (uint32_t e17),                                                                                 (e17),                                      N(e17,      16,  0)                                                                                  ) // ProgGuide 4.16 p.112
    ENC(BITMAP_TRANSFORM_F, (uint32_t f24),                                                                                 (f24),                                      N(f24,      23,  0)                                                                                  ) // ProgGuide 4.17 p.113
    ENC(BLEND_FUNC,         (BLEND    src,      BLEND    dst),                                                              (src, dst),                                 N(src,       5,  3) | N(dst,       2,  0)                                                            ) // ProgGuide 4.18 p.114
    ENC(CALL,               (uint32_t dest22),                                                                              (dest22),                                   N(dest22,   15,  0)                                                                                  ) // ProgGuide 4.19 p.116
    ENC(CELL,               (uint8_t  cell7),                                                                               (cell7),                                    N(cell7,     6,  0)                                                                                  ) // ProgGuide 4.20 p.117
    ENC(CLEAR,              (uint8_t  color1,   uint8_t  stencil1, uint8_t  tag1),                                          (color1, stencil1, tag1),                   N(color1,    2,  2) | N(stencil1,  1,  1) | N(tag1,     0,  0)                                       ) // ProgGuide 4.21 p.118
    ENC(CLEAR_COLOR_A,      (uint8_t  alpha8),                                                                              (alpha8),                                   N(alpha8,    7,  0)                                                                                  ) // ProgGuide 4.22 p.120
    ENC(CLEAR_COLOR_RGB,    (uint8_t  red8,     uint8_t  green8,   uint8_t  blue8),                                         (red8, green8, blue8),                      N(red8,     23, 16) | N(green8,   15,  8) | N(blue8,    7,  0)                                       ) // ProgGuide 4.23 p.121
    ENC(CLEAR_COLOR,        (uint32_t rgb24),                                                                               (rgb24),                                    N(rgb24,    23,  0)                                                                                  ) // ProgGuide 4.23 p.121
    ENC(CLEAR_STENCIL,      (uint8_t  stencil8),                                                                            (stencil8),                                 N(stencil8,  7,  0)                                                                                  ) // ProgGuide 4.24 p.122
    ENC(CLEAR_TAG,          (uint8_t  tag8),                                                                                (tag8),                                     N(tag8,      7,  0)                                                                                  ) // ProgGuide 4.25 p.123
    ENC(COLOR_A,            (uint8_t  alpha8),                                                                              (alpha8),                                   N(alpha8,    7,  0)                                                                                  ) // ProgGuide 4.26 p.124
    ENC(COLOR_MASK,         (uint8_t  red1,     uint8_t  green1,   uint8_t  blue1,   uint8_t  alpha1),                      (red1, green1, blue1, alpha1),              N(red1,      3,  3) | N(green1,    2,  2) | N(blue1,    1,  1) | N(alpha1,  0,  0)                   ) // ProgGuide 4.27 p.125
    ENC(COLOR_RGB,          (uint8_t  red8,     uint8_t  green8,   uint8_t  blue8),                                         (red8, green8, blue8),                      N(red8,     23, 16) | N(green8,   15,  8) | N(blue8,    7,  0)                                       ) // ProgGuide 4.28 p.126
    ENC(COLOR,              (uint32_t rgb24),                                                                               (rgb24),                                    N(rgb24,    23,  0)                                                                                  ) // ProgGuide 4.28 p.126
    ENC(DISPLAY,            (),                                                                                             (),                                         0                                                                                                    ) // ProgGuide 4.29 p.127
    ENC(END,                (),                                                                                             (),                                         0                                                                                                    ) // ProgGuide 4.30 p.128
    ENC(JUMP,               (uint16_t dest16),                                                                              (dest16),                                   N(dest16,   15,  0)                                                                                  ) // ProgGuide 4.31 p.129
    ENC(LINE_WIDTH,         (uint16_t width12),                                                                             (width12),                                  N(width12,  11,  0)                                                                                  ) // ProgGuide 4.32 p.130
    ENC(MACRO,              (uint8_t  index1),                                                                              (index1),                                   N(index1,    0,  0)                                                                                  ) // ProgGuide 4.33 p.131
    ENC(NOP,                (),                                                                                             (),                                         0                                                                                                    ) // ProgGuide 4.34 p.131
    ENC(PALETTE_SOURCE,     (uint32_t addr22),                                                                              (addr22),                                   N(addr22,   21,  0)                                                                                  ) // ProgGuide 4.35 p.132
    ENC(POINT_SIZE,         (uint16_t size13),                                                                              (size13),                                   N(size13,   12,  0)                                                                                  ) // ProgGuide 4.36 p.133
    ENC(RESTORE_CONTEXT,    (),                                                                                             (),                                         0                                                                                                    ) // ProgGuide 4.37 p.134
    ENC(RETURN,             (),                                                                                             (),                                         0                                                                                                    ) // ProgGuide 4.38 p.135
    ENC(SAVE_CONTEXT,       (),                                                                                             (),                                         0                                                                                                    ) // ProgGuide 4.39 p.136
    ENC(SCISSOR_SIZE,       (uint16_t width12,  uint16_t height12),                                                         (width12, height12),                        N(width12,  23, 12) | N(height12, 11,  0)                                                            ) // ProgGuide 4.40 p.137
    ENC(SCISSOR_XY,         (uint16_t x11,      uint16_t y11),                                                              (x11, y11),                                 N(x11,      21, 11) | N(y11,      10,  0)                                                            ) // ProgGuide 4.41 p.138
    ENC(STENCIL_FUNC,       (FUNC     func,     uint8_t  ref8,     uint8_t  mask8),                                         (func, ref8, mask8),                        N(func,     19, 16) | N(ref8,     15,  8) | N(mask8,    7,  0)                                       ) // ProgGuide 4.42 p.139
    ENC(STENCIL_MASK,       (uint8_t  mask8),                                                                               (mask8),                                    N(mask8,     7,  0)                                                                                  ) // ProgGuide 4.43 p.140
    ENC(STENCIL_OP,         (STENCIL  sfail,    STENCIL  spass),                                                            (sfail, spass),                             N(sfail,     5,  3) | N(spass,     2,  0)                                                            ) // ProgGuide 4.44 p.141
    ENC(TAG,                (uint8_t  tag8),                                                                                (tag8),                                     N(tag8,      7,  0)                                                                                  ) // ProgGuide 4.45 p.143
    ENC(TAG_MASK,           (uint8_t  mask1),                                                                               (mask1),                                    N(mask1,     0,  0)                                                                                  ) // ProgGuide 4.46 p.144
    ENC(VERTEX2F,           (int16_t  x15,      int16_t  y15),                                                              (x15, y15),                                 N(x15,      29, 15) | N(y15,      14,  0)                                                            ) // ProgGuide 4.47 p.145
    ENC(VERTEX2II,          (uint16_t x9,       uint16_t y9,       uint8_t  handle5, uint8_t  cell6),                       (x9, y9, handle5, cell6),                   N(x9,       29, 21) | N(y9,       20, 12) | N(handle5, 11,  7) | N(cell6,  6,  0)                    ) // ProgGuide 4.48 p.146
    ENC(VERTEX_FORMAT,      (uint8_t  frac3),                                                                               (frac3),                                    N(frac3,     2,  0)                                                                                  ) // ProgGuide 4.49 p.147
    ENC(VERTEX_TRANSLATE_X, (uint32_t x17),                                                                                 (x17),                                      N(x17,      16,  0)                                                                                  ) // ProgGuide 4.50 p.148
    ENC(VERTEX_TRANSLATE_Y, (uint32_t y17),                                                                                 (y17),                                      N(y17,      16,  0)                                                                                  ) // ProgGuide 4.51 p.149

    #undef ENC
    #undef N

    //-----------------------------------------------------------------------
    // Graphics co-processor commands
    //
    // Co-processor commands are encoded as bytes (not bits) so they can't
    // be encoded the same way as display list commands.
    //
    // The following macros are used to generate functions that start with
    // cmd_ and send the given command and parameters to the co-processor.
    //
    // 2 byte input value
    #define V2(value) (_hal.Send16((uint16_t)(value)), result += 2)
    // 4 byte input value
    #define V4(value) (_hal.Send32((uint32_t)(value)), result += 4)
    // String value
    #define SS(value, maxlen) (result += _hal.SendAlignmentBytes(_hal.SendString(value, maxlen)))
    // Program memory string value
    #define SF(value, maxlen, bufsize) (result += _hal.SendAlignmentBytes(_hal.SendStringF(value, maxlen, bufsize)))
    // Transfer from host RAM
    #define MM(value, len) (result += _hal.SendAlignmentBytes(_hal.SendBuffer(value, len)))
    // 4 byte output value: Store Cmd index to parameter and bump cmd index
    // Use NULL as parameter to ignore the output
    #define Q4(name) ((name ? (*name = _cmd_index + result) : 0), V4(0))
    // Send command and data
    #define CMD(name, declaration, value) \
        CmdIndex cmd_##name declaration { Cmd(ENC_CMD_##name); int16_t result = 0; (void)(value); return _cmd_index += result; }
    // Send command that has outputs (May possibly be changed later)
    // (The caller should start the co-processor and wait until it executes
    // the commands, then retrieve the output data using CmdRead with the
    // command index values that are returned by the function)
    #define CMDOUT CMD
    // Send command that only works for some types of EVE chips
    // (May possibly be changed later)
    #define CMD2 CMD
    #define CMD34 CMD
    #define CMD4 CMD

    CMD4(APILEVEL,          (APILEVEL level32),                                                                                                             (V4(level32)                                                                        )) //           [PG34 p112] (EVE4)
    CMD(DLSTART,            (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.11 p.162
    CMD(SWAP,               (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.12 p.163
    CMD(COLDSTART,          (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.13 p.163
    CMD(INTERRUPT,          (uint32_t ms32),                                                                                                                (V4(ms32)                                                                           )) // ProgGuide 5.14 p.164
    CMD(APPEND,             (uint32_t ptr32, uint32_t num32),                                                                                               (V4(ptr32),V4(num32)                                                                )) // ProgGuide 5.15 p.165
    CMD(REGREAD,            (uint32_t ptr32, uint32_t result32),                                                                                            (V4(ptr32),V4(result32)                                                             )) // ProgGuide 5.16 p.166
    CMD(MEMWRITE,           (uint32_t ptr32, uint32_t num, const uint8_t *data),                                                                            (V4(ptr32),V4(num),MM(data, num)                                                    )) // ProgGuide 5.17 p.167
    CMD(INFLATE,            (uint32_t ptr32, uint32_t num, const uint8_t *data),                                                                            (V4(ptr32),MM(data, num)                                                            )) // ProgGuide 5.18 p.168
    CMD34(INFLATE2,         (uint32_t ptr32, OPT options, uint32_t num, const uint8_t *data),                                                               (V4(ptr32),V4(options),MM(data,num)                                                 )) //           [PG34 p117] (EVE3/EVE4)
    CMD(LOADIMAGE,          (uint32_t ptr32, OPT options, uint32_t num, const uint8_t *data),                                                               (V4(ptr32),V4(options),MM(data, num)                                                )) // ProgGuide 5.19 p.169
    CMD(MEDIAFIFO,          (uint32_t ptr32, uint32_t size32),                                                                                              (V4(ptr32),V4(size32)                                                               )) // ProgGuide 5.20 p.170
    CMD(PLAYVIDEO,          (OPT options),                                                                                                                  (V4(options)                                                                        )) // ProgGuide 5.21 p.171
    CMD(VIDEOSTART,         (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.22 p.172
    CMD(VIDEOFRAME,         (uint32_t dst32, uint32_t ptr32),                                                                                               (V4(dst32),V4(ptr32)                                                                )) // ProgGuide 5.23 p.173
    CMDOUT(MEMCRC,          (uint32_t ptr32, uint32_t num32, CmdIndex *xresult32),                                                                          (V4(ptr32),V4(num32),Q4(xresult32)                                                  )) // ProgGuide 5.24 p.173
    CMD(MEMZERO,            (uint32_t ptr32, uint32_t num32),                                                                                               (V4(ptr32),V4(num32)                                                                )) // ProgGuide 5.25 p.174
    CMD(MEMSET,             (uint32_t ptr32, uint32_t value8, uint32_t num32),                                                                              (V4(ptr32),V4(value8),V4(num32)                                                     )) // ProgGuide 5.26 p.175
    CMD(MEMCPY,             (uint32_t dest32, uint32_t src32, uint32_t num32),                                                                              (V4(dest32),V4(src32),V4(num32)                                                     )) // ProgGuide 5.27 p.176
    CMD(BUTTON,             (int16_t x16, int16_t y16, int16_t w16, int16_t h16, int16_t font5, OPT options, const char *message, uint16_t len = 0),        (V2(x16),V2(y16),V2(w16),V2(h16),V2(font5),V2(options),SS(message, len)             )) // ProgGuide 5.28 p.176
    CMD(CLOCK,              (int16_t x16, int16_t y16, int16_t r16, OPT options, uint16_t h16, uint16_t m16, uint16_t s16, uint16_t ms16),                  (V2(x16),V2(y16),V2(r16),V2(options),V2(h16),V2(m16),V2(s16),V2(ms16)               )) // ProgGuide 5.29 p.179
    CMD(FGCOLOR,            (uint32_t c24),                                                                                                                 (V4(c24)                                                                            )) // ProgGuide 5.30 p.183
    CMD(BGCOLOR,            (uint32_t c24),                                                                                                                 (V4(c24)                                                                            )) // ProgGuide 5.31 p.184
    CMD(GRADCOLOR,          (uint32_t c24),                                                                                                                 (V4(c24)                                                                            )) // ProgGuide 5.32 p.185
    CMD(GAUGE,              (int16_t x16, int16_t y16, int16_t r16, OPT options, uint16_t major16, uint16_t minor16, uint16_t val16, uint16_t range16),     (V2(x16),V2(y16),V2(r16),V2(options),V2(major16),V2(minor16),V2(val16),V2(range16)  )) // ProgGuide 5.33 p.187
    CMD(GRADIENT,           (int16_t x016, int16_t y016, uint32_t rgb024, int16_t x116, int16_t y116, int32_t rgb124),                                      (V2(x016),V2(y016),V4(rgb024),V2(x116),V2(y116),V4(rgb124)                          )) // ProgGuide 5.34 p.193
    CMD34(GRADIENTA,        (int16_t x016, int16_t y016, uint32_t argb032, int16_t x116, int16_t y116, int32_t argb132),                                    (V2(x016),V2(y016),V4(argb032),V2(x116),V2(y116),V4(argb132)                        )) //           [PG34 p136] (EVE3/EVE4)
    CMD(KEYS,               (int16_t x16, int16_t y16, int16_t w16, int16_t h16, int16_t font5, OPT options, const char *message, uint16_t len = 0),        (V2(x16),V2(y16),V2(w16),V2(h16),V2(options),SS(message, len)                       )) // ProgGuide 5.35 p.196
    CMD(PROGRESS,           (int16_t x16, int16_t y16, int16_t w16, int16_t h16, OPT options, uint16_t val16, uint16_t range16),                            (V2(x16),V2(y16),V2(w16),V2(h16),V2(options),V2(val16),V2(range16),V2(0)            )) // ProgGuide 5.36 p.200
    CMD(SCROLLBAR,          (int16_t x16, int16_t y16, int16_t w16, int16_t h16, OPT options, uint16_t val16, uint16_t size16, uint16_t range16),           (V2(x16),V2(y16),V2(w16),V2(h16),V2(options),V2(val16),V2(size16),V2(range16)       )) // ProgGuide 5.37 p.201
    CMD(SLIDER,             (int16_t x16, int16_t y16, int16_t w16, int16_t h16, OPT options, uint16_t val16, uint16_t range16),                            (V2(x16),V2(y16),V2(w16),V2(h16),V2(options),V2(val16),V2(range16),V2(0)            )) // ProgGuide 5.38 p.205
    CMD(DIAL,               (int16_t x16, int16_t y16, int16_t r16, OPT options, uint16_t val16),                                                           (V2(x16),V2(y16),V2(r16),V2(options),V2(val16),V2(0)                                )) // ProgGuide 5.39 p.207
    CMD(TOGGLE,             (int16_t x16, int16_t y16, int16_t w16, uint16_t font5, OPT options, uint16_t state16, const char *message, uint16_t len = 0),  (V2(x16),V2(y16),V2(w16),V2(font5),V2(options),V2(state16),SS(message, len)         )) // ProgGuide 5.40 p.210
    CMD34(FILLWIDTH,        (uint32_t s),                                                                                                                   (V4(s)                                                                              )) //           [PG34 p147] (EVE3/EVE4)
    CMD(TEXT,               (int16_t x16, int16_t y16, int16_t font5, OPT options, const char *message, uint16_t len = 0),                                  (V2(x16),V2(y16),V2(font5),V2(options),SS(message, len)                             )) // ProgGuide 5.41 p.213
    CMD(TEXTF,              (int16_t x16, int16_t y16, int16_t font5, OPT options, const __FlashStringHelper *message),                                     (V2(x16),V2(y16),V2(font5),V2(options),SF(message, 0, 0)                            )) // ProgGuide 5.41 p.213
    CMD(SETBASE,            (uint32_t b6),                                                                                                                  (V4(b6)                                                                             )) // ProgGuide 5.42 p.216
    CMD(NUMBER,             (int16_t x16, uint16_t y16, int16_t font5, OPT options, int32_t n32),                                                           (V2(x16),V2(y16),V2(font5),V2(options),V4(n32)                                      )) // ProgGuide 5.43 p.217
    CMD(LOADIDENTITY,       (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.44 p.220
    CMD(SETMATRIX,          (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.45 p.220
    CMDOUT(GETMATRIX,       (CmdIndex *xa32, CmdIndex *xb32, CmdIndex *xc32, CmdIndex *xd32, CmdIndex *xe32, CmdIndex *xf32),                               (Q4(xa32),Q4(xb32),Q4(xc32),Q4(xd32),Q4(xe32),Q4(xf32)                              )) // ProgGuide 5.46 p.221
    CMDOUT(GETPTR,          (CmdIndex *xptr),                                                                                                               (Q4(xptr)                                                                           )) // ProgGuide 5.47 p.222
    CMDOUT(GETPROPS,        (CmdIndex *xptr32, CmdIndex *xwidth32, CmdIndex *xheight32),                                                                    (Q4(xptr32),Q4(xwidth32),Q4(xheight32)                                              )) // ProgGuide 5.48 p.223
    CMD(SCALE,              (int32_t sx32, int32_t sy32),                                                                                                   (V4(sx32),V4(sy32)                                                                  )) // ProgGuide 5.49 p.223
    CMD(ROTATE,             (int32_t a32),                                                                                                                  (V4(a32)                                                                            )) // ProgGuide 5.50 p.225
    CMD34(ROTATEAROUND,     (int32_t x32, int32_t y32, int32_t a32, int32_t s32),                                                                           (V4(x32), V4(y32),V4(a32),V4(s32)                                                   )) //           [PG34 p157] (EVE3/EVE4)
    CMD(TRANSLATE,          (int32_t tx32, int32_t ty32),                                                                                                   (V4(tx32),V4(ty32)                                                                  )) // ProgGuide 5.51 p.226
    CMDOUT(CALIBRATE,       (CmdIndex *xresult32),                                                                                                          (Q4(xresult32)                                                                      )) // ProgGuide 5.52 p.227
    CMD34(CALIBRATESUB,     (uint16_t x16, uint16_t y16, uint16_t w16, uint16_t h16, CmdIndex *xresult32),                                                  (V2(x16),V2(y16),V2(w16),V2(h16),Q4(xresult32)                                      )) //           [PG34 p159] (EVE3/EVE4)
    CMD(SETROTATE,          (uint32_t r32),                                                                                                                 (V4(r32)                                                                            )) // ProgGuide 5.53 p.228
    CMD(SPINNER,            (int16_t x16, int16_t y16, uint16_t style2, uint16_t scale2),                                                                   (V2(x16),V2(y16),V2(style2),V2(scale2)                                              )) // ProgGuide 5.54 p.229
    CMD(SCREENSAVER,        (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.55 p.233
    CMD(SKETCH,             (int16_t x16, int16_t y16, uint16_t w16, uint16_t h16, uint32_t ptr32, FORMAT format),                                          (V2(x16),V2(y16),V2(w16),V2(h16),V4(ptr32),V2(format),V2(0)                         )) // ProgGuide 5.55 p.234
    CMD(STOP,               (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.57 p.236
    CMD(SETFONT,            (uint32_t font5, uint32_t ptr32),                                                                                               (V4(font5),V4(ptr32)                                                                )) // ProgGuide 5.58 p.237
    CMD(SETFONT2,           (uint32_t font5, uint32_t ptr32, uint32_t firstchar8),                                                                          (V4(font5),V4(ptr32),V4(firstchar8)                                                 )) // ProgGuide 5.59 p.237
    CMD(SETSCRATCH,         (uint32_t handle5),                                                                                                             (V4(handle5)                                                                        )) // ProgGuide 5.60 p.239
    CMD(ROMFONT,            (uint32_t font5, uint32_t romslot6),                                                                                            (V4(font5),V4(romslot6)                                                             )) // ProgGuide 5.61 p.240
    CMD(RESETFONTS,         (),                                                                                                                             (0                                                                                  )) //           [PG34 p167]
    CMD(TRACK,              (int16_t x16, int16_t y16, int16_t w16, int16_t h16, int16_t tag8),                                                             (V2(x16),V2(y16),V2(w16),V2(h16),V2(tag8),V2(0)                                     )) // ProgGuide 5.62 p.240
    CMD(SNAPSHOT,           (uint32_t ptr32),                                                                                                               (V4(ptr32)                                                                          )) // ProgGuide 5.63 p.245
    CMD(SNAPSHOT2,          (FORMAT format, uint32_t ptr32, int16_t x16, int16_t y16, int16_t w16, int16_t h16),                                            (V4(format),V4(ptr32),V2(x16),V2(y16),V2(w16),V2(h16)                               )) // ProgGuide 5.64 p.246
    CMD(SETBITMAP,          (uint32_t addr32, FORMAT format, uint16_t width16, uint16_t height16),                                                          (V4(addr32),V2(format),V2(width16),V2(height16),V2(0)                               )) // ProgGuide 5.65 p.247
    CMD(LOGO,               (),                                                                                                                             (0                                                                                  )) // ProgGuide 5.66 p.249
    CMD2(CSKETCH,           (int16_t x16, int16_t y16, uint16_t w16, uint16_t h16, uint32_t ptr32, FORMAT format, uint16_t freq16),                         (V2(x16),V2(y16),V2(w16),V2(h16),V4(ptr32),V2(format),V2(freq16)                    )) // ProgGuide 5.67 p.249 (EVE2)
    CMD34(FLASHERASE,       (),                                                                                                                             (0                                                                                  )) //           [PG34 p174] (EVE3/EVE4)
    CMD34(FLASHWRITE,       (uint32_t ptr32, uint32_t num32, const uint8_t *data),                                                                          (V4(ptr32),V4(num32),MM(data,num32)                                                 )) //           [PG34 p174] (EVE3/EVE4)
    CMD34(FLASHPROGRAM,     (uint32_t dst32, uint32_t src32, uint32_t num32),                                                                               (V4(dst32),V4(src32),V4(num32)                                                      )) //           [PG34 p175] (EVE3/EVE4)
    CMD34(FLASHREAD,        (uint32_t dst32, uint32_t src32, uint32_t num32),                                                                               (V4(dst32),V4(src32),V4(num32)                                                      )) //...........[PG34 p176] (EVE3/EVE4)
    CMD34(FLASHUPDATE,      (uint32_t dst32, uint32_t src32, uint32_t num32),                                                                               (V4(dst32),V4(src32),V4(num32)                                                      )) //...........[PG34 p177] (EVE3/EVE4)
    CMD34(FLASHDETACH,      (),                                                                                                                             (0                                                                                  )) //           [PG34 p177] (EVE3/EVE4)
    CMD34(FLASHATTACH,      (),                                                                                                                             (0                                                                                  )) //           [PG34 p178] (EVE3/EVE4)
    CMD34(FLASHFAST,        (CmdIndex *xresult32),                                                                                                          (Q4(xresult32)                                                                      )) //           [PG34 p178] (EVE3/EVE4)
    CMD34(FLASHSPIDESEL,    (),                                                                                                                             (0                                                                                  )) //           [PG34 p179] (EVE3/EVE4)
    CMD34(FLASHSPITX,       (uint32_t num32, const uint8_t *data),                                                                                          (V4(num32),MM(data,num32)                                                           )) //           [PG34 p179] (EVE3/EVE4)
    CMD34(FLASHSPIRX,       (uint32_t ptr32, uint32_t num32),                                                                                               (V4(ptr32),V4(num32)                                                                )) //           [PG34 p179] (EVE3/EVE4)
    CMD34(CLEARCACHE,       (),                                                                                                                             (0                                                                                  )) //           [PG34 p180] (EVE3/EVE4)
    CMD34(FLASHSOURCE,      (uint32_t ptr32),                                                                                                               (V4(ptr32)                                                                          )) //           [PG34 p181] (EVE3/EVE4)
    CMD34(VIDEOSTARTF,      (),                                                                                                                             (0                                                                                  )) //           [PG34 p181] (EVE3/EVE4)
    CMD34(ANIMSTART,        (int32_t ch5, uint32_t aoptr32, ANIM loop),                                                                                     (V4(ch5),V4(aoptr32),V4(loop)                                                       )) //           [PG34 p181] (EVE3/EVE4)
    CMD4(ANIMSTARTRAM,      (int32_t ch5, uint32_t aoptr32, ANIM loop),                                                                                     (V4(ch5),V4(aoptr32),V4(loop)                                                       )) //           [PG34 p182] (EVE4)
    CMD4(RUNANIM,           (uint32_t waitmask32, uint32_t play32),                                                                                         (V4(waitmask32),V4(play32)                                                          )) //           [PG34 p183] (EVE4)
    CMD34(ANIMSTOP,         (int32_t ch5),                                                                                                                  (V4(ch5)                                                                            )) //           [PG34 p184] (EVE3/EVE4)
    CMD34(ANIMXY,           (int32_t ch5, int16_t x16, int16_t y16),                                                                                        (V4(ch5),V2(x16),V2(y16)                                                            )) //           [PG34 p185] (EVE3/EVE4)
    CMD34(ANIMDRAW,         (int32_t ch5),                                                                                                                  (V4(ch5)                                                                            )) //           [PG34 p185] (EVE3/EVE4)
    CMD34(ANIMFRAME,        (int16_t x16, int16_t y16, uint32_t aoptr32, uint32_t frame32),                                                                 (V2(x16),V2(y16),V4(aoptr32),V4(frame32)                                            )) //           [PG34 p186] (EVE3/EVE4)
    CMD34(ANIMFRAMERAM,     (int16_t x16, int16_t y16, uint32_t aoptr32, uint32_t frame32),                                                                 (V2(x16),V2(y16),V4(aoptr32),V4(frame32)                                            )) //           [PG34 p186] (EVE4)
    CMD34(SYNC,             (),                                                                                                                             (0                                                                                  )) //           [PG34 p187] (EVE3/EVE4)
    CMD34(BITMAP_TRANSFORM, (int32_t x032, int32_t y032, int32_t x132, int32_t y132, int32_t x232, int32_t y232, int32_t tx032, int32_t ty032, int32_t tx132, int32_t ty132, int32_t tx232, int32_t ty232, CmdIndex *xresult16),
                                                                                                                                                            (V4(x032),V4(y032),V4(x132),V4(y132),V4(x232),V4(y232),V4(tx032),V4(ty032),V4(tx132),V4(ty132),V4(tx232),V4(ty232),Q4(xresult16)
                                                                                                                                                                                                                                                )) //           [PG34 p188] (EVE3/EVE4)
    CMD4(TESTCARD,          (),                                                                                                                             (0                                                                                  )) //           [PG34 p189] (EVE4)
    CMD4(WAIT,              (uint32_t us32),                                                                                                                (V4(us32)                                                                           )) //           [PG34 p190] (EVE4)
    CMD4(NEWLIST,           (uint32_t a32),                                                                                                                 (V4(a32)                                                                            )) //           [PG34 p190] (EVE4)
    CMD4(ENDLIST,           (),                                                                                                                             (0                                                                                  )) //           [PG34 p191] (EVE4)
    CMD4(CALLLIST,          (uint32_t a32),                                                                                                                 (V4(a32)                                                                            )) //           [PG34 p192] (EVE4)
    CMD4(RETURNLIST,        (),                                                                                                                             (0                                                                                  )) //           [PG34 p192] (EVE4)
    CMD4(FONTCACHE,         (uint32_t font32, int32_t ptr32, uint32_t num32),                                                                               (V4(font32),V4(ptr32),V4(num32)                                                     )) //           [PG34 p193] (EVE4)
    CMD4(FONTCACHEQUERY,    (CmdIndex *xtotal32, CmdIndex *xused32),                                                                                        (Q4(xtotal32),Q4(xused32)                                                           )) //           [PG34 p194] (EVE4)
    CMD4(GETIMAGE,          (CmdIndex *xsrc32, CmdIndex *xfmt32, CmdIndex *xw32, CmdIndex *xh32, CmdIndex *xpalette32),                                     (Q4(xsrc32),Q4(xfmt32),Q4(xw32),Q4(xh32),Q4(xpalette32)                             )) //           [PG34 p194] (EVE4)
    CMD4(HSF,               (uint32_t w32),                                                                                                                 (V4(w32)                                                                            )) //           [PG34 p195] (EVE4)
    CMD4(PCLKFREQ,          (uint32_t ftarget32, int32_t rounding32, CmdIndex *xfactual32),                                                                 (V4(ftarget32),V4(rounding32),Q4(xfactual32)                                        )) //           [PG34 p196] (EVE4)

    #undef CMD4
    #undef CMD34
    #undef CMDOUT
    #undef Q4
    #undef MM
    #undef SF
    #undef SS
    #undef V4
    #undef V2

    /////////////////////////////////////////////////////////////////////////
    // HELPER FUNCTIONS
    /////////////////////////////////////////////////////////////////////////

public:
    //-----------------------------------------------------------------------
    // Get pointer to first available byte in RAM_G
    uint32_t                            // Returns pointer in RAM_G
    CmdGetPtr()
    {
        uint32_t result;

        // Ignore error; if necessary the caller can use IsBusy() first to
        // get error state.
        CmdWaitComplete();

        CmdIndex p; // Cmd index of output stored here
        cmd_GETPTR(&p);

        // Execute the command; this should not generate an error.
        CmdExecute(true);

        // Retrieve the result
        result = CmdRead32(p);

        DBG_TRAFFIC("RAM_G first free byte is at %08X\n", result);

        return result;
    }

public:
    //-----------------------------------------------------------------------
    // Finish the current display list and swap and execute
    CmdIndex                            // Returns updated Cmd index
    CmdDlFinish(
        bool waituntilcomplete = false) // True = wait until done
    {
        cmd_DISPLAY();

        cmd_SWAP();

        // Ignore errors: if necessary, caller can use IsBusy() first to get
        // error state
        return CmdExecute(waituntilcomplete);
    }

public:
    //-----------------------------------------------------------------------
    // Set clearing color and optionally clear the screen, stencil and tag
    CmdIndex                            // Returns updated Cmd index
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
    CmdIndex                            // Returns updated Cmd index
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
    CmdIndex                            // Returns updated Cmd index
    CmdColor(
        uint32_t rgb24)                 // red/green/blue combination
    {
        return cmd_COLOR(rgb24);
    }

public:
    //-----------------------------------------------------------------------
    // Set alpha (transparency) for next commands
    CmdIndex                            // Returns updated Cmd index
    CmdAlpha(
        uint8_t alpha)                  // Alpha value
    {
        return cmd_COLOR_A(alpha);
    }

public:
    //-----------------------------------------------------------------------
    // Draw a point at the given location
    CmdIndex                            // Returns updated Cmd index
    Point(
        uint16_t point_x,               // X coordinate
        uint16_t point_y,               // Y coordinate
        uint16_t ball_size)             // Diameter
    {
        // Set the size of the dot to draw
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
    CmdIndex                            // Returns updated Cmd index
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
    CmdIndex                            // Returns updated Cmd index
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
    CmdIndex                            // Returns updated Cmd index
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

public:
    //-----------------------------------------------------------------------
    // Display a blank screen with a text and a spinner
    CmdIndex                            // Returns updated Cmd index
    CmdStartSpinner(
        uint32_t clearcolor24,          // Clear color (RGB)
        uint32_t textcolor24,           // Text color (RGB)
        uint32_t spinnercolor24,        // Spinner color (RGB)
        const __FlashStringHelper *message) // Text to display
    {
        //Make sure that the chip is caught up.
        CmdWaitComplete();

        //========== START THE DISPLAY LIST ==========
        // Start the display list
        cmd_DLSTART();

        // Set the default clear color
        cmd_CLEAR_COLOR(clearcolor24);

        // Clear the screen - this and the previous prevent artifacts between lists
        cmd_CLEAR(1, 1, 1);

        //Solid color -- not transparent
        cmd_COLOR_A(255);

        //========== ADD GRAPHIC ITEMS TO THE DISPLAY LIST ==========
        // Set the drawing for the text
        cmd_COLOR(textcolor24);

        // Display the caller's message at the center of the screen using bitmap handle 27
        cmd_TEXTF(_hcenter, _vcenter, 27, OPT_CENTER, message);

        // Set the drawing color for the spinner
        cmd_COLOR(spinnercolor24);

        //Send the spinner go command
        cmd_SPINNER(_hcenter, _vcenter, 0, 1);

        // Instruct the graphics processor to show the list
        return CmdDlFinish();
    }

public:
    //-----------------------------------------------------------------------
    // Stop the spinner if one is displayed
    CmdIndex                            // Returns updated Cmd index
    CmdStopSpinner(
        uint32_t clearcolor24,          // Clear color (RGB)
        uint32_t textcolor24,           // Text color (RGB)
        const __FlashStringHelper *message) // Text to display
    {
        //Make sure that the chip is caught up.
        CmdWaitComplete();

        //========== START THE DISPLAY LIST ==========
        // Start the display list
        cmd_DLSTART();

        // Set the default clear color
        cmd_CLEAR_COLOR(clearcolor24);

        // Clear the screen - this and the previous prevent artifacts between lists
        cmd_CLEAR(1, 1, 1);

        //Solid color -- not transparent
        cmd_COLOR_A(255);

        //========== STOP THE SPINNER ==========
        cmd_STOP();

        //========== ADD GRAPHIC ITEMS TO THE DISPLAY LIST ==========
        // Set the drawing for the text
        cmd_COLOR(textcolor24);

        // Display the caller's message at the center of the screen using bitmap handle 27
        cmd_TEXTF(_hcenter, _vcenter, 27, OPT_CENTER, message);

        // Instruct the graphics processor to show the list
        return CmdDlFinish();
    }
};
