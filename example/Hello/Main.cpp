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

extern font_t fontlib::font;

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

    const glyph_t *g = get_glyph(ft, 'A');

    if (!g)         // bail out if we don't have a glyph
        return;

    uint8_t r = 50, c = 50;
    uint8_t w = g->width, h = g->height;
    uint16_t n = w * h;

    display::set_col_addr(c, c + w - 1);
    display::set_row_addr(r, c + h - 1);
    display::start();

    uint16_t fg = swap_bytes(from_rgb(255, 255, 255));
    uint16_t bg = swap_bytes(from_rgb(255, 0, 0));

    for (uint16_t i = 0; i < n; ++i)
    {
        display::write(g->bitmap[i] ? fg : bg);
    }
}

