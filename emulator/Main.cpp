#include <widget.h>
#include <plot.h>
#include <emulator.h>
#include "../examples/Widget/gui.h"

typedef display_t<240, 240> display;

static void print_message(const message_t& m)
{
    switch (m.index())
    {
        case button_press: printf("button_press: %d\n", std::get<button_press>(m)); break;
        case encoder_delta: printf("encoder_delta: %d\n", std::get<encoder_delta>(m)); break;
        case encoder_press: printf("encoder_press: .\n"); break;
        default: printf("illegal message\n");
    }
}

void run()
{
    display::initialize("Display Emulator", 1);
    display::clear(slate_gray);
    pen_t<display> pen(dark_red);

    pen.rectangle(0, 0, display::width(), display::height());

    bool quit = false;

    theme_t theme = { white, slate_gray, dim_gray, yellow, orange_red, fontlib::cmunss_20, false };

    static gui_t<display> gui(theme);

    xy_plot_t<display> plot;

    plot.setup((theme.normal_bg = 0x2f2f2f, theme.normal_fg = dim_gray, theme));
    plot.constrain(0, display::width()-20, 0, 100);
    plot.place(10, 100);

    static const float pi = 3.141592654;
    static const uint16_t n = 100;
    static float xs[n], ys[n];

    plot.viewport(-15, -0.20, 15, 1);
    plot.clear();
    display::render();

    auto sinc = [](float x) { return x != 0 ? sin(x) / x : 1; };

    for (uint16_t i = 0; i < n; ++i)
    {
        xs[i] = 30 * i / (n - 1) - 15;
        ys[i] = sinc(xs[i]);
    }

    plot.line_plot(xs, ys, n, red);
    display::render();

    window_manager wm(&gui);
    message_t m;
    event_t e;

    display::render();

    while ((e = poll_event(m)) != ev_quit)
        if (e == ev_message)
        {
            print_message(m);
            wm.handle_message(m);
            display::render();
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

