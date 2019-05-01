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

    display::clear(swap_bytes(from_rgb(255, 0, 0)));

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

    text_renderer_t<display> txr(ft);

    txr.set_pos(50, 100);
    txr.write("Hello World!");
}

