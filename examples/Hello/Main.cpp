////
//
//      Demo ST7789-based TFT display
//
////

#include <spi.h>
#include <gpio.h>
#include <st7789.h>
#include <text.h>

using namespace hal;
using namespace gpio;
using namespace st7789;
using namespace fontlib;
using namespace text;

#if defined(STM32F051)
typedef st7789_t<1, PA5, PA7, PC5, PC4> display;    // STM32F051-DISCOVERY
static const spi::spi_clock_divider_t display_spi_prescale = spi::fpclk_2;
#elif defined(STM32G431)
typedef st7789_t<1, PA5, PA7, PC7, PB6> display;    // NUCLEO-STM32G431
static const spi::spi_clock_divider_t display_spi_prescale = spi::fpclk_8;
#endif

typedef output_t<PC8> led_a;
typedef output_t<PC9> led_b;
typedef output_t<PB12> probe;

void loop(const font_t&);

int main()
{
    display::setup<display_spi_prescale>();
    display::set_scroll_area(0, 240);
    led_a::setup();
    led_b::setup();
    probe::setup();

    display::clear(color::yellow);

    font_t ft = fontlib::cmuntx_20;

    for (;;)
    {
        probe::set();
        loop(ft);
        probe::clear();
    }
}

void loop(const font_t& ft)
{
    static uint8_t i = 0;

    if (!(i & 0xf))
    {
        led_a::toggle();
        if (led_a::read())
            led_b::toggle();
    }

    text_renderer_t<display> txr(ft, color::black, color::yellow, true);

    static const char *lines[] =
        { "Julius Caesar is our travel"
        , "guide as he takes us through"
        , "his murderous subjugation of"
        , "the native Celtic tribal"
        , "peoples of ancient Gaul."
        , "It sounds vaguely like other,"
        , "recent European colonial"
        , "conquests until the natives"
        , "nearly win."
        };

    for (;;)
        for (uint8_t i = 0; i < sizeof(lines) / sizeof(*lines); ++i)
        {
//            txr.set_pos(0, 40);
//            txr.clear_line();
            txr.writeln(lines[i]);
            sys_tick::delay_ms(1000);
        }

/*
    for (;;)
        for (uint16_t l = 0; l < 320; ++l)
        {
            sys_tick::delay_ms(5);
            display::scroll(l);
        }
*/
    for (;;);   // stop here forever!
}

