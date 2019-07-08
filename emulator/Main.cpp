#include <widget.h>
#include <plot.h>
#include <emulator.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

typedef display_t<240, 240> display;
static char tmp_buf[256];

struct unit_t {};

void run()
{
    display::initialize("Display Emulator", 1);
    display::clear(slate_gray);
    pen_t<display> pen(dark_red);

    pen.rectangle(0, 0, display::width(), display::height());

    bool quit = false;

    widget_t<display, unit_t> b0(fontlib::cmunss_20, yellow, crimson, 50, 50, 100, 25, [](auto _) { return "-"; }, 0, true);
    widget_t<display, unit_t> b1(fontlib::cmunss_20, yellow, rebecca_purple, 130, 80, 80, 25, [](auto _) { return "Hello World!"; }, 0, true);
    widget_t<display, unit_t> b2(fontlib::cmunss_20, yellow, olive_drab, 130, 150, 10, 15, [](auto _) { return "-----H j-g-----"; }, 0, true);
    widget_t<display, unit_t> b3(fontlib::cmunss_20, yellow, lime_green, 20, 200, 50, 25, [](auto _) { return "//"; }, 0, true);
    widget_t<display, unit_t> b4(fontlib::cmunss_20, yellow, orange_red, 130, 200, 50, 25, [](auto _) { return "l//g"; }, 0, true);

    widget_t<display, char*> txbox(fontlib::cmunrm_48, dim_gray, wheat, 160, 10, 50, 40, [](auto s) { return s; }, 0);
    widget_t<display, int> ibox(fontlib::cmuntt_24, white, web_gray, 10, 10, 50, 30, [](int x) { sprintf(tmp_buf, "%d", x); return tmp_buf; },0,  true);

    xy_plot_t<display> plot;

    plot.setup(10, 100, 100, 75, black);
    display::render();

    static const float pi = 3.141592654;
    static const uint16_t n = 50;
    static float xs[n], ys[n];

    plot.viewport(0, -1, 2 * pi, 1);

    for (uint16_t i = 0; i < n; ++i)
    {
        xs[i] = i * 2 * pi / n;
        ys[i] = sin(xs[i]);
    }

    plot.line_plot(xs, ys, n, red);
    display::render();

    while (!quit)
    {
        int x;

        switch (poll_event(x))
        {
        case ev_quit:
            quit = true;
            break;
        case ev_key:
            {
                char c[2] = { static_cast<char>(x), 0 };
                txbox = c;
                display::render();
                break;
            }
        case ev_wheel:
            ibox = x;
            display::render();
            break;
        default: ;
        }
    }

    display::shutdown();
}

int main()
{
    try
    {
        run();
    }
    catch (const char *s)
    {
        printf("exception: %s\n", s);
    }
}

