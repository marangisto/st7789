////
//
//      Demo ST7789-based TFT display
//
////

#include <spi.h>
#include <gpio.h>

using namespace stm32f0;
using namespace gpio;
using namespace spi;

typedef spi_t<1, PA5, PA7> ctrl;
typedef output_t<PC4> res;
typedef output_t<PC5> dcx;
typedef output_t<PC8> led_a;
typedef output_t<PC9> led_b;
typedef output_t<PB12> probe;

// rgb color using conventional 0..255 ranges
__attribute__((always_inline))
static inline uint16_t from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

// Swap byte order for 16-bit color scheme
__attribute__((always_inline))
static inline uint16_t swap_bytes(uint16_t x)
{
    uint16_t l = x & 0xff;

    return (l << 8) | (x >> 8);
}

static const uint8_t TFT_WIDTH  = 240;
static const uint8_t TFT_HEIGHT = 240;

namespace st7789
{
enum command_t
    { CABCCTRL     = 0xC7   // CABC control
    , CASET        = 0x2A
    , CMD2EN       = 0xDF   // Command 2 enable
    , COLMOD       = 0x3A
    , DGMEN        = 0xBA   // Digital gamma enable
    , DGMLUTB      = 0xE3   // Digital gamma look-up table for blue
    , DGMLUTR      = 0xE2   // Digital gamma look-up table for red
    , DISPOFF      = 0x28
    , DISPON       = 0x29
    , EQCTRL       = 0xE9   // Equalize time control
    , FRCTR2       = 0xC6   // FR Control 2
    , FRCTRL1      = 0xB3   // Frame rate control
    , GAMSET       = 0x26   // Gamma set
    , GATECTRL     = 0xE4   // Gate control
    , GCTRL        = 0xB7   // Gate control
    , GTADJ        = 0xB8   // Gate on timing adjustment
    , IDMOFF       = 0x38   // Idle mode off
    , IDMON        = 0x39   // Idle mode on
    , IDSET        = 0xC1   // ID setting
    , INVOFF       = 0x20
    , INVON        = 0x21
    , LCMCTRL      = 0xC0   // LCM control
    , MADCTL       = 0x36   // Memory data access control
    , NOP          = 0x00
    , NORON        = 0x13
    , NVGAMCTRL    = 0xE1   // Negative voltage gamma control
    , NVMSET       = 0xFC   // NVM setting
    , PARCTRL      = 0xB5   // Partial mode control
    , PORCTRL      = 0xB2   // Porch control
    , PROMACT      = 0xFE   // Program action
    , PROMCTRL     = 0xEC   // Program control
    , PROMEN       = 0xFA   // Program mode enable
    , PTLAR        = 0x30
    , PTLON        = 0x12
    , PVGAMCTRL    = 0xE0   // Positive voltage gamma control
    , PWCTRL1      = 0xD0   // Power control 1
    , PWCTRL2      = 0xE8   // Power control 2
    , PWMFRSEL     = 0xCC   // PWM frequency selection
    , RAMCTRL      = 0xB0   // RAM control
    , RAMRD        = 0x2E
    , RAMRDC       = 0x3E   // Memory read continue
    , RAMWR        = 0x2C
    , RAMWRC       = 0x3C   // Memory write continue
    , RASET        = 0x2B
    , RDD_COLMOD   = 0x0C   // Read display pixel format
    , RDD_MADCTL   = 0x0B   // Read display MADCTL
    , RDDID        = 0x04
    , RDDIM        = 0x0D   // Read display image mode
    , RDDPM        = 0x0A   // Read display power mode
    , RDDSM        = 0x0E   // Read display signal mode
    , RDDSR        = 0x0F   // Read display self-diagnostic result
    , RDDST        = 0x09
    , REGSEL1      = 0xC8   // Register value section 1
    , REGSEL2      = 0xCA   // Register value section 2
    , RGBCTRL      = 0xB1   // RGB control
    , RGBSET       = 0x2D   // Color setting for 4096, 64K and 262K colors
    , SLPIN        = 0x10
    , SLPOUT       = 0x11
    , SPI2EN       = 0xE7   // SPI2 enable
    , SWRESET      = 0x01
    , TEOFF        = 0x34   // Tearing effect line off
    , TEON         = 0x35   // Tearing effect line on
    , VAPVANEN     = 0xD2   // Enable VAP/VAN signal output
    , VCMOFSET     = 0xC5   // VCOMS offset set
    , VCOMS        = 0xBB   // VCOMS setting
    , VDVSET       = 0xC4   // VDV setting
    , VDVVRHEN     = 0xC2   // VDV and VRH command enable
    , VRHS         = 0xC3   // VRH set
    , VSCRDEF      = 0x33   // Vertical scrolling definition
    };
}

template<typename... Args>
static inline void write_cmd(st7789::command_t cmd, Args&&... args)
{
    dcx::clear();
    ctrl::write(static_cast<uint8_t>(cmd));
    ctrl::wait_idle();
    dcx::set();
    (ctrl::write(static_cast<uint8_t>(args)), ...);
}

void loop();

int main()
{
    led_a::setup();
    led_b::setup();
    probe::setup();

    ctrl::setup<mode_3, msb_first, fpclk_2, high_speed>();

    res::setup();                                       // display controller reset
    dcx::setup();                                       // data / command selection

    res::clear();                                       // reset display controller (active low)
    sys_tick::delay_ms(1);                              // FIXME: minumum pulse is 10us!
    res::set();                                         // release reset
    sys_tick::delay_ms(5);                              // this seems to work (some modes require 125ms)

    write_cmd(st7789::SLPOUT);                          // stop sleeping (in case we were)
    write_cmd(st7789::NORON);                           // normal-on
    write_cmd(st7789::MADCTL, 0x08);                    // RGB order
    write_cmd(st7789::COLMOD, 0x55);                    // 65k RGB & 16 bits per pixel
    write_cmd(st7789::LCMCTRL, 0x04);                   // reverse source output order
    write_cmd(st7789::CASET, 0, 0, 0, TFT_WIDTH - 1);   // column address set
    write_cmd(st7789::RASET, 0, 0, 0, TFT_HEIGHT - 1);  // row address set
    write_cmd(st7789::DISPON);                          // display-on
    write_cmd(st7789::INVON);                           // inverted mode

    write_cmd(st7789::RAMWR);                           // write pixel data
    for (uint32_t i = 0; i < 240*240; ++i)
        ctrl::write(static_cast<uint16_t>(0));

    for (;;)
        loop();
}

void loop()
{
    static uint8_t i = 0;

    if (!(i & 0xf))
    {
        led_a::toggle();
        if (led_a::read())
            led_b::toggle();
    }

    for (size_t k = 0; k < 8; ++k)
    {
        uint16_t pixel;

        switch (k)
        {
            case 0: pixel = from_rgb(127, 127, 127); break;
            case 1: pixel = from_rgb(255,   0,   0); break;
            case 2: pixel = from_rgb(  0, 255,   0); break;
            case 3: pixel = from_rgb(  0,   0, 255); break;
            case 4: pixel = from_rgb(255,   0, 255); break;
            case 5: pixel = from_rgb(  0, 255, 255); break;
            case 6: pixel = from_rgb(255, 255,   0); break;
            case 7: pixel = from_rgb(255, 255, 255); break;
        }

        pixel = swap_bytes(pixel);

        probe::set();                                   // to measure frame rate
        write_cmd(st7789::RAMWR);                       // write pixel data
        probe::clear();

        for (uint32_t i = 0; i < 240*240; ++i)
            ctrl::write(pixel);

        //sys_tick::delay_ms(250);
    }
}

