#include <stdlib.h>
#include <gpio.h>
#include <st7789.h>
#include <text.h>

using namespace stm32f0;
using namespace gpio;
using namespace st7789;
using namespace fontlib;

typedef st7789_t<1, PA5, PA7, PC5, PC4> display;

void loop(const font_t&, color_t, color_t);

int main()
{
    display::setup();

    font_t ft = fontlib::font;

    for (;;)
        loop(ft, color::yellow, color::blue);
}

void loop(const font_t& ft, color_t fg, color_t bg)
{
    text_renderer_t<display> tr(ft, fg, bg);

    display::clear(bg);

    tr.set_pos(50, 50);
    tr.write("Hello World!");

    for (int i = 0; i < 1000; ++i)
    {
        char buf[32];

        tr.set_pos(50, 100);
        tr.write(itoa(i, buf, 10));
        sys_tick::delay_ms(2);
    }
}
