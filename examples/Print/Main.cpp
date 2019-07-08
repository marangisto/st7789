#include <stdlib.h>
#include <cstdio>
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

