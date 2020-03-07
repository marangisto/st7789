#pragma once

#include <widget.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

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
    typedef valuebox_t<DISPLAY, show_float<2> > floatbox2;

    void setup(const theme_t& t)
    {
        quiet = false;

        c1.setup();
        c1.append(l1.setup(t, "foo"));
        c1.append(l2.setup(t, "bar"));
        c1.append(l3.setup(t, "baz!"));
        c2.setup();
        c2.append(i1.setup(t));
        c2.append(f1.setup(t));
        c2.append(f2.setup(t, 0, &quiet));
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

    bool quiet;
    intbox i1;
    floatbox f1;
    floatbox2 f2;
    label l1, l2, l3;
    label r1, r2, r3;
    vertical_t<DISPLAY> c1, c2;
    horizontal_t<DISPLAY> q1;
    sub_gui_t<DISPLAY> sub;
};

