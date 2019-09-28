#include "../display.h"
#include <stdlib.h>
#include <cstdio>
#include <gpio.h>
#include <text.h>

using namespace gpio;
using namespace fontlib;
using namespace text;

constexpr double pi = 3.14159265358979323846;

void loop(const font_t&, color_t, color_t);

int main()
{
    display::setup<display_spi_prescale>();

    font_t ft = fontlib::cmunss_32;

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
        char buf[128];

        tr.set_pos(50, 100);
        tr.write(itoa(i, buf, 10));
        tr.set_pos(50, 200);
        sprintf(buf, "%.7f", pi);
        tr.write(buf);
        sys_tick::delay_ms(2);
    }
}

