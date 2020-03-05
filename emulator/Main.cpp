#include <widget.h>
#include <plot.h>
#include <emulator.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

typedef display_t<240, 240> display;

template<typename DISPLAY>
struct sub_gui_t: window_t<DISPLAY>
{
    typedef valuebox_t<DISPLAY, show_str> label;
    typedef valuebox_t<DISPLAY, show_int, edit_int> intbox;
    typedef valuebox_t<DISPLAY, show_float<2>, edit_float<25> > floatbox;

    void setup(const theme_t& t)
    {
        c1.setup();
        c1.append(l1.setup(t, "x"));
        c1.append(l2.setup(t, "y"));
        c2.setup();
        c2.append(i1.setup(t));
        c2.append(f1.setup(t));
        q1.setup();
        q1.append(&c1);
        q1.append(&c2);
        b.setup(&q1, t.border_color, 3);

        list<ifocus*> navigation;

        navigation.push_back(&i1);
        navigation.push_back(&f1);

        window_t<DISPLAY>::setup(&b, navigation, t, rect_t(50, 50, 140, 140));
    }

    virtual action_t handle_message(const message_t& m)
    {
        if (m.index() == button_press)
            switch (std::get<button_press>(m))
            {
            case 1:
                return action_t().emplace<pop_window>(0);
            default: ;  // unhandled button
            }
        return window_t<DISPLAY>::handle_message(m);
    }

    intbox i1;
    floatbox f1;
    label l1, l2;
    vertical_t<DISPLAY> c1, c2;
    horizontal_t<DISPLAY> q1;
    border_t<DISPLAY> b;
};

template<typename DISPLAY>
struct gui_t: window_t<DISPLAY>
{
    typedef valuebox_t<DISPLAY, show_str> label;
    typedef valuebox_t<DISPLAY, show_int, edit_int> intbox;
    typedef valuebox_t<DISPLAY, show_float<2>, edit_float<25> > floatbox;

    void setup(const theme_t& t)
    {
        c1.setup();
        c1.append(i1.setup(t));
        c1.append(l1.setup(t, "foo"));
        c1.append(l2.setup(t, "bar"));
        c1.append(l3.setup(t, "baz!"));
        c2.setup();
        c2.append(f1.setup(t));
        c2.append(r1.setup(t, "ofo"));
        c2.append(r2.setup(t, "abr"));
        c2.append(r3.setup(t, "abz!"));
        q1.setup();
        q1.append(&c1);
        q1.append(&c2);

        list<ifocus*> navigation;

        navigation.push_back(&i1);
        navigation.push_back(&f1);

        window_t<DISPLAY>::setup(&q1, navigation, t);

        sub.setup(t);
    }

    virtual action_t handle_message(const message_t& m)
    {
        if (m.index() == button_press)
            switch (std::get<button_press>(m))
            {
            case 1:
                return action_t().emplace<push_window>(&sub);
            default: ;  // unhandled button
            }
        return window_t<DISPLAY>::handle_message(m);
    }

    intbox i1;
    floatbox f1;
    label l1, l2, l3;
    label r1, r2, r3;
    vertical_t<DISPLAY> c1, c2;
    horizontal_t<DISPLAY> q1;
    sub_gui_t<DISPLAY> sub;
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

    theme_t theme = { white, slate_gray, dim_gray, yellow, orange_red, fontlib::cmunss_20 };

    gui.setup(theme);

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

