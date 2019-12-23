#include <widget.h>
#include <plot.h>
#include <emulator.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

typedef display_t<240, 240> display;
static char tmp_buf[256];

struct unit_t
{
    unit_t() {}
    unit_t(int) {}
    constexpr unit_t(const unit_t&) {}
    unit_t(volatile const unit_t&) {}
    constexpr void operator=(const unit_t&) {}
    void operator=(const unit_t&) volatile {}
};

struct show_str
{
    typedef const char *T;
    static const char *show(T x) { return x; }
};

struct show_int
{
    typedef int T;
    static const char *show(T x) { sprintf(tmp_buf, "%d", x); return tmp_buf; }
};

struct edit_int
{
    static void edit(volatile int& x, int i) {}
};

template<typename DISPLAY>
struct gui_t
{
    void setup()
    {
        i1.setup(fontlib::cmunss_20, yellow, blue);
        l1.setup("foo", fontlib::cmunss_20, yellow, crimson);
        l2.setup("bar", fontlib::cmunss_20, yellow, slate_gray);
        l3.setup("baz!", fontlib::cmunss_20, yellow, crimson);
        c1.setup();
        c1.append(&i1);
        c1.append(&l1);
        c1.append(&l2);
        c1.append(&l3);
        b1.setup(&c1, white, 1);
        r1.setup("ofo", fontlib::cmunss_20, yellow, crimson);
        r2.setup("abr", fontlib::cmunss_20, yellow, slate_gray);
        r3.setup("abz!", fontlib::cmunss_20, yellow, crimson);
        c2.setup();
        c2.append(&r1);
        c2.append(&r2);
        c2.append(&r3);
        b2.setup(&c2, black, 1);
        q1.setup();
        q1.append(&b1);
        q1.append(&b2);
        q1.constrain(10, 120, 10, 240); // fixme: what about zero min?
        q1.layout(120, 0);
    }

    void render()
    {
        q1.render();
    }

    valuebox_t<DISPLAY, show_int> i1;
    label_t<DISPLAY> l1, l2, l3;
    label_t<DISPLAY> r1, r2, r3;
    border_t<DISPLAY> b1, b2;
    vertical_t<DISPLAY> c1, c2;
    horizontal_t<DISPLAY> q1;
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

    /*
    widget_t<display, unit_t> b0(fontlib::cmunss_20, yellow, crimson, 50, 50, 100, 25, [](auto _) { return "-"; }, 0, true);
    widget_t<display, unit_t> b1(fontlib::cmunss_20, yellow, rebecca_purple, 130, 80, 80, 25, [](auto _) { return "Hello World!"; }, 0, true);
    widget_t<display, unit_t> b2(fontlib::cmunss_20, yellow, olive_drab, 130, 150, 10, 15, [](auto _) { return "-----H j-g-----"; }, 0, true);
    widget_t<display, unit_t> b3(fontlib::cmunss_20, yellow, lime_green, 20, 200, 50, 25, [](auto _) { return "//"; }, 0, true);
    widget_t<display, unit_t> b4(fontlib::cmunss_20, yellow, orange_red, 130, 200, 50, 25, [](auto _) { return "l//g"; }, 0, true);
    */

    widget_t<display, char*> txbox(fontlib::cmunrm_48, dim_gray, wheat, 160, 10, 50, 40, [](auto s) { return s; }, 0);
    widget_t<display, int> ibox(fontlib::cmuntt_24, white, web_gray, 10, 10, 50, 30, [](int x) { sprintf(tmp_buf, "%d", x); return tmp_buf; },0,  true);

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

