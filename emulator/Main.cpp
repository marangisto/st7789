#include <widget.h>
#include <plot.h>
#include <emulator.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

typedef display_t<240, 240> display;

template<typename DISPLAY>
struct gui_t
{
    typedef valuebox_t<DISPLAY, show_str> label;
    typedef valuebox_t<DISPLAY, show_int, edit_int> intbox;
    typedef valuebox_t<DISPLAY, show_float<2>, edit_float<25> > floatbox;

    void setup()
    {
        i1.setup(fontlib::cmunss_20, yellow, blue);
        l1.setup(fontlib::cmunss_20, yellow, crimson, "foo");
        l2.setup(fontlib::cmunss_20, yellow, crimson, "bar");
        l3.setup(fontlib::cmunss_20, yellow, crimson, "baz!");
        c1.setup();
        c1.append(&i1);
        c1.append(&l1);
        c1.append(&l2);
        c1.append(&l3);
        b1.setup(&c1, white, 1);
        f1.setup(fontlib::cmunss_20, yellow, blue);
        r1.setup(fontlib::cmunss_20, yellow, crimson, "ofo");
        r2.setup(fontlib::cmunss_20, yellow, crimson, "abr");
        r3.setup(fontlib::cmunss_20, yellow, crimson, "abz!");
        c2.setup();
        c2.append(&f1);
        c2.append(&r1);
        c2.append(&r2);
        c2.append(&r3);
        b2.setup(&c2, black, 1);
        q1.setup();
        q1.append(&b1);
        q1.append(&b2);
        q1.constrain(10, 120, 10, 240); // fixme: what about zero min?
        q1.layout(120, 0);
        focus[0] = &i1;
        focus[1] = &f1;
    }

    void render()
    {
        q1.render();
    }

    void navigate(int dir)
    {
        static constexpr uint8_t npos = sizeof(focus) / sizeof(*focus);

        focus[pos]->defocus();
        if (dir > 0 && ++pos >= npos)
            pos = 0;
        if (dir < 0 && pos-- == 0)
            pos = npos - 1;
        focus[pos]->focus(light_green);
    }

    void edit_state(bool b)
    {
        focus[pos]->focus(b ? orange_red : light_green);
    }

    void edit(int i)
    {
        focus[pos]->edit(i);
    }

    intbox i1;
    floatbox f1;
    label l1, l2, l3;
    label r1, r2, r3;
    border_t<DISPLAY> b1, b2;
    vertical_t<DISPLAY> c1, c2;
    horizontal_t<DISPLAY> q1;

    ifocus *focus[2];
    uint8_t pos = 0;
};

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
    gui.i1 = 55;

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
    bool navigate = true;

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
                gui.r3 = c;
                display::render();
                navigate = !navigate;
                gui.edit_state(!navigate);
                display::render();
                break;
            }
        case ev_wheel:
            if (navigate)
                gui.navigate(x);
            else
                gui.edit(x);
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

