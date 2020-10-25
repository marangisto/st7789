////
//
//      Demo ST7789-based TFT display
//
////

#include <gpio.h>
#include <textio.h>
#include <text.h>
#include "../display.h"

using namespace fontlib;
using namespace text;

using console = console_t<display>;

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

int main()
{
    display::setup<display_spi_prescale>();
    console::setup(fontlib::cmuntx_20, color::green, color::black);
    console::clear();

    for (;;)
        for (uint8_t i = 0; i < sizeof(lines) / sizeof(*lines); ++i)
        {
            printf<console>("%s\n", lines[i]);
            sys_tick::delay_ms(100);
        }
}

