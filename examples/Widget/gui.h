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

    sub_gui_t(const theme_t& t)
        : l1(t, "x"), l2(t, "y"), c1(&l1, &l2)
        , i1(t), f1(t), c2(&i1, &f1)
        , q1(&c1, &c2), b(&q1, t.border_color, 5)
    {
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

    label l1, l2;
    vertical_t<DISPLAY> c1;
    intbox i1;
    floatbox f1;
    vertical_t<DISPLAY> c2;
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

    gui_t(const theme_t& t)
        : l1(t, "foo"), l2(t, "bar"), l3(t, "baz!"), c1(&l1, &l2, &l3)
        , i1(t), f1(t), f2(t, 0), c2(&i1, &f1, &f2)
        , q1(&c1, &c2), sub(t)
    {
        list<ifocus*> navigation;

        navigation.push_back(&i1);
        navigation.push_back(&f1);

        window_t<DISPLAY>::setup(&q1, navigation, t);
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

    label l1, l2, l3;
    vertical_t<DISPLAY> c1;
    intbox i1;
    floatbox f1;
    floatbox2 f2;
    vertical_t<DISPLAY> c2;
    horizontal_t<DISPLAY> q1;
    sub_gui_t<DISPLAY> sub;
};

