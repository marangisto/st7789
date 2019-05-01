////
//
//      Demo ST7789-based TFT display
//
////

#include <spi.h>
#include <gpio.h>
#include <st7789.h>
#include <fontlib.h>

using namespace stm32f0;
using namespace gpio;
using namespace st7789;
using namespace fontlib;

typedef st7789_t<1, PA5, PA7, PC5, PC4> display;
typedef output_t<PC8> led_a;
typedef output_t<PC9> led_b;
typedef output_t<PB12> probe;

void loop(const font_t&);

int main()
{
    display::setup();
    led_a::setup();
    led_b::setup();
    probe::setup();

    display::clear(color::yellow);

    font_t ft = fontlib::font;

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

    text_renderer_t<display> txr(ft, color::black, color::yellow);

    static const char *lines[] =
        { ""    // FIXME: we need to start at font height!
        , "Julius Caesar is our travel"
        , "guide as he takes us through"
        , "his murderous subjugation of"
        , "the native Celtic tribal"
        , "peoples of ancient Gaul."
        , "It sounds vaguely like other,"
        , "recent European colonial"
        , "conquests until the natives"
        , "nearly win."
        };

    for (uint8_t i = 0; i < sizeof(lines) / sizeof(*lines); ++i)
        txr.writeln(lines[i]);

    for (;;);   // stop here forever!
}

