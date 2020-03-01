#include <widget.h>
#include <plot.h>
#include <emulator.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

typedef display_t<240, 240> display;

template<typename DISPLAY>
struct gui_t: public screen_t<DISPLAY>
{
    typedef valuebox_t<DISPLAY, show_str> label;
    typedef valuebox_t<DISPLAY, show_int, edit_int> intbox;
    typedef valuebox_t<DISPLAY, show_float<2>, edit_float<25> > floatbox;

    void setup()
    {
        i1.setup(fontlib::cmunss_20, yellow, steel_blue);
        l1.setup(fontlib::cmunss_20, yellow, dark_green, "foo");
        l2.setup(fontlib::cmunss_20, yellow, dark_green, "bar");
        l3.setup(fontlib::cmunss_20, yellow, dark_green, "baz!");
        c1.setup();
        c1.append(&i1);
        c1.append(&l1);
        c1.append(&l2);
        c1.append(&l3);
        f1.setup(fontlib::cmunss_20, yellow, steel_blue);
        r1.setup(fontlib::cmunss_20, yellow, dark_green, "ofo");
        r2.setup(fontlib::cmunss_20, yellow, dark_green, "abr");
        r3.setup(fontlib::cmunss_20, yellow, dark_green, "abz!");
        c2.setup();
        c2.append(&f1);
        c2.append(&r1);
        c2.append(&r2);
        c2.append(&r3);
        q1.setup();
        q1.append(&c1);
        q1.append(&c2);

        list<ifocus*> navigation;

        navigation.push_back(&i1);
        navigation.push_back(&f1);

        screen_t<DISPLAY>::setup(&q1, navigation, yellow, orange_red);
    }

    intbox i1;
    floatbox f1;
    label l1, l2, l3;
    label r1, r2, r3;
    vertical_t<DISPLAY> c1, c2;
    horizontal_t<DISPLAY> q1;
};

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

    static gui_t<display> gui;

    gui.setup();
    gui.render();

    xy_plot_t<display> plot;

    plot.setup(10, 100, 100, 75, gray, black);

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
    message_t m;
    event_t e;

    while ((e = poll_event(m)) != ev_quit)
        if (e == ev_message)
        {
            print_message(m);
            gui.handle_message(m);
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

