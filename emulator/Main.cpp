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
    static void edit(volatile int& x, int i) { x += i; }
};

template<typename DISPLAY>
struct gui_t
{
    typedef valuebox_t<DISPLAY, show_str> label;
    typedef valuebox_t<DISPLAY, show_int, edit_int> intbox;

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
        r1.setup(fontlib::cmunss_20, yellow, crimson, "ofo");
        r2.setup(fontlib::cmunss_20, yellow, crimson, "abr");
        r3.setup(fontlib::cmunss_20, yellow, crimson, "abz!");
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

    intbox i1;
    label l1, l2, l3;
    label r1, r2, r3;
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
                gui.r3 = c;
                display::render();
                break;
            }
        case ev_wheel:
            gui.i1.edit(x);
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

