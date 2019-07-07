////
//
//      Line graphics demo
//
////

#include <spi.h>
#include <gpio.h>
#include <st7789.h>
#include <draw.h>

using namespace hal;
using namespace gpio;
using namespace st7789;
using namespace color;
using namespace graphics;

//typedef st7789_t<1, PA5, PA7, PC5, PC4> display;    // STM32F051 DISCOVERY
typedef st7789_t<3, PC10, PC12, PC11, PD2> display;    // NUCLEO STM32G431

static void squares(color_t bg, color_t fg)
{
    display::clear(bg);
    pen_t<display> pen(fg);

    int16_t x1 = display::width() - 1;
    int16_t y1 = display::height() - 1;
    int16_t xc = x1 >> 1, yc = y1 >> 1;

    for (int i = 0; i <= y1 >> 1; i += 8)
        pen.rectangle(xc - i, yc - i, i << 1, i << 1);
}

static void circles(color_t bg, color_t fg)
{
    display::clear(bg);
    pen_t<display> pen(fg);

    int16_t x1 = display::width() - 1;
    int16_t y1 = display::height() - 1;
    int16_t xc = x1 >> 1, yc = y1 >> 1;

    for (int r = 0; r <= y1 >> 1; r += 8)
        pen.circle(xc, yc, r);
}

int main()
{
    display::setup();

    for (;;)
    {
        squares(color::black, color::white);
        sys_tick::delay_ms(1000);
        circles(color::white, color::blue);
        sys_tick::delay_ms(1000);
        squares(color::blue, color::yellow);
        sys_tick::delay_ms(1000);
        circles(color::yellow, color::black);
        sys_tick::delay_ms(1000);
    }

    for (;;);   // never return
}

