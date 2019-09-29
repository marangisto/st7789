#include "../display.h"
#include <widget.h>
#include <plot.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

static char tmp_buf[256];

struct gui_t
{
    static const uint16_t w = display::width() / 2;
    static const uint16_t h = 30;

    widget_t<display, int> fbox;
    xy_plot_t<display> plot;

    gui_t()
        : fbox
            ( fontlib::cmuntt_24, yellow, dark_green, 0, 0, w, h
            , [](auto x) { sprintf(tmp_buf, "%d", x); return tmp_buf; }
            , [](auto& x, int i) { x += i; }
            , true
            )
        {
            plot.setup(0, h, display::width(), display::height() - h, web_gray, 0x101010);
            plot.viewport(-1., -1., 1., 1.);
        }

    void render()
    {
        fbox.render();
        plot.clear();
    }
};

int main()
{
    display::setup<display_spi_prescale>(dark_red);
    gui_t gui;

    gui.render();

    for (;;)
    {
        static uint8_t i = 0;
        gui.fbox = ++i;
    }
}

