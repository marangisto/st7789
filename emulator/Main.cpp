#include <text.h>
#include <fontlib.h>
#include <draw.h>
#include <emulator.h>

using namespace text;
using namespace color;
using namespace graphics;
using namespace fontlib;

typedef display_t<240, 240> display;

void text_box
    ( uint16_t      x
    , uint16_t      y
    , uint16_t      w
    , uint16_t      h
    , const font_t& font
    , color_t       fg
    , color_t       bg
    , const char    *s
    )
{
    text_renderer_t<display> tr(font, fg, bg, true);
    pen_t<display> pen(bg);
    uint16_t tw, th;
 
    tr.bounding_box(s, tw, th);

    uint16_t rpad = tw < w ? (w - tw) >> 1 : 0;
    uint16_t lpad = tw < w ? w - tw - rpad : 0;

    if (lpad)
        pen.fill_rectangle(x, y, lpad, h);
    if (rpad)
        pen.fill_rectangle(x + lpad + tw, y, rpad, h);

    uint16_t bpad = th < h ? (h - th) >> 1 : 0;
    uint16_t tpad = th < h ? h - th - bpad : 0;

    if (tpad)
        pen.fill_rectangle(x + lpad, y, tw, tpad);
    if (bpad)
        pen.fill_rectangle(x + lpad, y + th + tpad, tw, bpad);

    //pen_t<display>(black).rectangle(x-1+lpad, y-1+tpad, tw+2, th+2);
    pen_t<display>(black).rectangle(x-1, y-1, w+2, h+2);
    tr.set_pos(x + lpad, y + tpad - font.min_y);
    tr.write(s);
    display::render();
}

void run()
{
    display::initialize(3);
    display::clear(slate_gray);
    pen_t<display> pen(white);

    pen.rectangle(0, 0, display::width(), display::height());
    display::render();

    text_box(50,  50, 100, 25, fontlib::cmunss_20, yellow, dark_red, "-");
    text_box(50, 100, 100, 25, fontlib::cmunss_20, yellow, rebecca_purple, "Hello World!");
    text_box(50, 150, 50, 25, fontlib::cmunss_20, yellow, teal, "j-g");
    text_box(130, 150, 10, 15, fontlib::cmunss_20, yellow, olive_drab, "-----H j-g-----");
    text_box(20, 200, 50, 25, fontlib::cmunss_20, yellow, lime_green, "//");
    text_box(130, 200, 50, 25, fontlib::cmunss_20, yellow, orange_red, "l//g");

    bool quit = false;
    font_t font = fontlib::cmunrm_48;
    text_renderer_t<display> tr(font, white, red, true);

    uint16_t r = font.start_row();

    tr.set_pos(0, r);

    while (!quit)
    {
        char c[2] = { 0, 0 };

        if (keyboard_poll(c[0]))
            switch (c[0])
            {
            case 0:
                quit = true;
                break;
            case '\r':
                r += font.line_spacing();
                tr.set_pos(0, r);
                break;
            default:
                text_box(160, 50, 50, 40, font, dim_gray, wheat, c);
                display::render();
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

